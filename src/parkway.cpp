#include "parkway.h"
#include <fstream>
#include <iostream>
#include "data_structures/internal/table_utils.hpp"
#include "coarseners/parallel/coarsener.hpp"
#include "coarseners/parallel/restrictive_coarsening.hpp"
#include "refiners/parallel/refiner.hpp"
#include "internal/serial_controller.hpp"
#include "internal/parallel_controller.hpp"
#include "hypergraph/hypergraph.hpp"
#include "utility/logging.hpp"
#include "options.hpp"
#include "Utils.h"

namespace parallel = parkway::parallel;
namespace serial = parkway::serial;
namespace ds = parkway::data_structures;

int k_way_partition(const parkway::options &options, MPI_Comm comm) {
  char shuffle_file[512];
  char message[512];

  parallel::coarsener *coarsener = nullptr;
  parallel::restrictive_coarsening *restrC = nullptr;
  parallel::refiner *refiner = nullptr;
  serial::controller *seqController = nullptr;
  parallel::controller *controller = nullptr;

  ds::internal::table_utils tableUtils;

  int number_of_processors;
  MPI_Comm_size(comm, &number_of_processors);
  options.set_number_of_processors(number_of_processors);

  int rank;
  MPI_Comm_rank(comm, &rank);

  Utils::check_parts_and_processors(options, comm);

  bool output_partition_tofile = options.get<bool>("write-partitions-to-file");

/* init pseudo-random number generator */
#ifdef USE_SPRNG
  int seed;
  if (init_options[1] == 0) {
    seed = make_sprng_seed();
  } else {
    seed = init_options[1];
  }

  if (!init_sprng(0, seed, SPRNG_DEFAULT)) {
    sprintf(
        message,
        "p[%d] could not initialise sprng random number generator - abort\n",
        rank);
    std::cout << message;
    MPI_Abort(comm, 0);
  }
#else
  if (options.get<int>("sprng-seed") == 0)
    srand48((rank + 1) * RAND_SEED);
  else
    srand48(options.get<int>("sprng-seed"));
#endif


  const char *file_name = options.get<std::string>("hypergraph").c_str();
  sprintf(shuffle_file, "%s.part.%d", file_name, number_of_processors);

  Funct::printIntro();
  parallel::hypergraph *hgraph = new parallel::hypergraph(
      rank, number_of_processors, file_name, comm);

  if (!hgraph) {
    error_on_processor("p[%d] not able to build local hypergraph from %s - "
                       "abort\n", rank, file_name);
    MPI_Abort(comm, 0);
  }

  ds::internal::table_utils::set_scatter_array(hgraph->total_number_of_vertices());

  int num_parts = options.get<int>("number-of-parts");
  double constraint = options.get<double>("balance-constraint");
  coarsener = Utils::buildParaCoarsener(rank, options, hgraph, comm);
  restrC = Utils::buildParaRestrCoarsener(rank, options, hgraph, comm);
  refiner = Utils::buildParaRefiner(rank, options, hgraph, comm);
  seqController = Utils::buildSeqController(rank, options);
  MPI_Barrier(comm);

  if (!coarsener) {
    error_on_processor("p[%d] not able to build ParaCoarsener - abort\n", rank);
    MPI_Abort(comm, 0);
  }

  if (!refiner) {
    error_on_processor("p[%d] not able to build ParaRefiner - abort\n", rank);
    MPI_Abort(comm, 0);
  }

  if (!seqController) {
    error_on_processor("p[%d] not able to build SeqController - abort\n", rank);
    MPI_Abort(comm, 0);
  }

  controller = Utils::buildParaController(
      rank, hgraph->total_number_of_vertices(), coarsener, restrC, refiner,
      seqController, options, comm);

  if (!controller) {
    error_on_processor("p[%d] not able to build ParaController - "
                       "abort\n", rank);
    MPI_Abort(comm, 0);
  }

  Funct::printEnd();

  hgraph->compute_balance_warnings(num_parts, constraint, comm);

  controller->set_hypergraph(hgraph);
  controller->set_prescribed_partition(shuffle_file, comm);
  controller->set_weight_constraints(comm);
  controller->run(comm);

  if (options.get<bool>("write-partitions-to-file")) {
  char part_file[512];
    sprintf(part_file, "%s.part.%d", file_name,
            options.get<int>("number-of-parts"));
    controller->partition_to_file(part_file, comm);
  }

  return controller->best_cut_size();
}

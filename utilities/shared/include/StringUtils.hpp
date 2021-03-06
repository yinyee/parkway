
#ifndef _STRING_UTILS_HPP
#define _STRING_UTILS_HPP

// ### StringUtils.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 01/12/2004: Last Modified
//
// ###

#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

class StringUtils {
protected:
public:
  StringUtils();
  ~StringUtils();

  static void skipNonDigits(char *&strPtr, char comm);
  static void skipNonDigits(char *&strPtr);
  static void skipDigits(char *&strPtr);
  static int stringToDigit(char *&strPtr);
  static int getParameterAsInteger(int argc, char **argv, const char *cmpr);
  static int getParameterAsInteger(int argc, char **argv, const char *cmpr,
                                   int def);
  static char *getParameterAsCharPtr(int argc, char **argv, const char *cmpr,
                                     char *def);
  static char getFirstChar(char *strPtr);
};

#endif

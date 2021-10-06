/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include "src/OMLibrary.hpp"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace onnx_mlir;

std::string testFileName;
std::string outputBaseName;
std::string mcpu;
std::string mtriple;
bool compileFromFile = false;

#define PARSE_ARG(NAME, FLAG)                                                  \
  if (arg.find(FLAG) == 0) {                                                   \
    NAME = arg.substr(sizeof(FLAG));                                           \
    return;                                                                    \
  }

#define PARSE_FLAG(NAME, FLAG)                                                 \
  if (arg.find(FLAG) == 0) {                                                   \
    NAME = true;                                                               \
    return;                                                                    \
  }

void ReadArg(const std::string &arg) {
  PARSE_ARG(mcpu, "--mcpu=");
  PARSE_ARG(mtriple, "--mtriple=");
  PARSE_ARG(mtriple, "--mtriple=");
  PARSE_ARG(outputBaseName, "-o");
  PARSE_FLAG(compileFromFile, "--fromfile");
  testFileName = arg;
}

void ReadCommandLine(int argc, char *argv[]) {
  for (int i = 0; i < argc; ++i) {
    ReadArg(std::string(argv[i]));
  }
}

int main(int argc, char *argv[]) {
  ReadCommandLine(argc, argv);

  if (outputBaseName == "") {
    outputBaseName = testFileName.substr(0, testFileName.find_last_of("."));
  }
  int retVal = 0;
  if (compileFromFile) {
    retVal = CompileFromFile(testFileName.c_str(), outputBaseName.c_str(),
        onnx_mlir::EmitLib, mcpu.empty() ? nullptr : mcpu.c_str(),
        mtriple.empty() ? nullptr : mtriple.c_str());
  } else {
    std::ifstream inFile(testFileName);
    std::string line((std::istreambuf_iterator<char>(inFile)),
        std::istreambuf_iterator<char>());
    retVal = CompileFromArray(line.data(), line.size(), outputBaseName.c_str(),
        onnx_mlir::EmitLib, mcpu.empty() ? nullptr : mcpu.c_str(),
        mtriple.empty() ? nullptr : mtriple.c_str());
  }
  if (retVal != 0) {
    std::cerr << "Compiling " << testFileName << "failed with code" << retVal;
  }
  return retVal;
}

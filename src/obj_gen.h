//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_OBJ_GEN_H
#define TINY_C_COMPILER_OBJ_GEN_H

#include <string>
#include "code_gen.h"

namespace tcc {

void ObjGen(CodeGenContext &context, const std::string &obj_file);
}

#endif  // TINY_C_COMPILER_OBJ_GEN_H

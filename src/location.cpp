//
// Created by kaiser on 18-12-10.
//

#include "location.h"

namespace tcc {

std::string SourceLocation::ToString() const {
  return file_name_ + ":" + std::to_string(row_) + ":" +
         std::to_string(column_) + ":";
}

}  // namespace tcc

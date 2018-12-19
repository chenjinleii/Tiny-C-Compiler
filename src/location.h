//
// Created by kaiser on 18-12-10.
//

#ifndef TINY_C_COMPILER_SOURCE_LOCATION_H
#define TINY_C_COMPILER_SOURCE_LOCATION_H

#include <string>
#include <cstdint>

namespace tcc {

class SourceLocation {
 public:
  std::string ToString() const;

  std::string file_name_;
  std::int32_t row_{1};
  std::int32_t column_{};
};

}

#endif //TINY_C_COMPILER_SOURCE_LOCATION_H

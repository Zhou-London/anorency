#pragma once

#include <string>
namespace Anorency {

class AnoInterface {
 public:
  AnoInterface(std::string& str);
  ~AnoInterface() = default;

  void set_str(const std::string& new_str);
  std::string get_str();

 private:
  std::string& str;
};

}  // namespace Anorency
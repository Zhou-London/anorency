#pragma once

#include <exception>
#include <string>
namespace Anorency {

class AnoException : public std::exception {
 private:
  std::string info;
  std::string message;

 public:
  explicit AnoException(const std::string& message)
      : info("Anorency General Error"), message(info + ": " + message) {}

  const char* what() const noexcept override { return message.c_str(); }
};
}  // namespace Anorency
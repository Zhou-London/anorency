
#include <exception>
#include <string>

namespace Anorency {

class AnoException : public std::exception {
 private:
  std::string msg;

 public:
  AnoException(std::string msg) : msg(msg) {}

  const char* what() const noexcept override { return msg.c_str(); }
};

}  // namespace Anorency
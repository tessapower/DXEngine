#ifndef ENGINE_EXCEPTION_H
#define ENGINE_EXCEPTION_H

#include "ChiliWin.h"
#include <exception>
#include <sstream>
#include <string>

class EngineException : public std::exception {
 public:
  EngineException(const LPCSTR file, int line) noexcept
      :  _file(file), _line(line) {}
  ~EngineException() = default;

  const char* what() const noexcept override {
    std::ostringstream oss;
    oss << type()
        << std::endl
        << source();

    _whatBuffer = oss.str();

    return _whatBuffer.c_str();
  }

  virtual const char *type() const noexcept { return _type; }

  int line() const noexcept { return _line; }

  std::string const& file() const noexcept { return _file; }

  std::string source() const noexcept {
    std::ostringstream oss;
    oss << "File: " << _file
        << std::endl
        << "Line: " << _line;

    return oss.str();
  }

 private:
  const char *_type{"Engine Exception"};
  std::string _file;
  int _line;

 protected:
  mutable std::string _whatBuffer;
};

#endif  // ENGINE_EXCEPTION_H

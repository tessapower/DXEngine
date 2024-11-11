#ifndef ENGINE_EXCEPTION_H
#define ENGINE_EXCEPTION_H

#include "stdafx.h"
#include <exception>
#include <sstream>
#include <string>

class EngineException : public std::exception {
 public:
  EngineException(const LPCWSTR file, int line) noexcept
      : _file(file), _line(line) {}
  ~EngineException() = default;

  const char* what() const noexcept override {
    return "Engine Exception";
  }

  virtual LPCWSTR msg() const noexcept {
    std::wostringstream oss;

    oss << type() << std::endl << source();

    _whatBuffer = oss.str();

    return _whatBuffer.c_str();
  }

  LPCWSTR type() const noexcept { return _type; }

  int line() const noexcept { return _line; }

  std::wstring file() const noexcept { return _file; }

  std::wstring source() const noexcept {
    std::wostringstream oss;
    oss << L"File: " << file()
        << std::endl
        << L"Line: " << _line;

    return oss.str();
  }

 private:
  LPCWSTR _file;
  int _line;

 protected:
  LPCWSTR _type{L"Engine Exception"};
  mutable std::wstring _whatBuffer;
};

#endif  // ENGINE_EXCEPTION_H

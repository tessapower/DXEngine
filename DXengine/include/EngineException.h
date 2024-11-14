#ifndef ENGINE_EXCEPTION_H
#define ENGINE_EXCEPTION_H

#include "stdafx.h"
#include <exception>
#include <sstream>
#include <string>

class engine_exception : public std::exception {
 public:
  engine_exception(const LPCWSTR file, const int line) noexcept
      : file_(file), line_(line) {}
  ~engine_exception() = default;

  const char* what() const noexcept override { return "Engine Exception"; }

  virtual LPCWSTR msg() const noexcept {
    std::wostringstream oss;

    oss << type() << "\n" << source();

    what_buffer_ = oss.str();

    return what_buffer_.c_str();
  }

  LPCWSTR type() const noexcept { return type_; }

  int line() const noexcept { return line_; }

  std::wstring file() const noexcept { return file_; }

  std::wstring source() const noexcept {
    std::wostringstream oss;
    oss << L"File: " << file()
        << L"\n"
        << L"Line: " << line_;

    return oss.str();
  }

 private:
  LPCWSTR file_;
  int line_;

 protected:
  LPCWSTR type_{L"Engine Exception"};
  mutable std::wstring what_buffer_;
};

#endif  // ENGINE_EXCEPTION_H

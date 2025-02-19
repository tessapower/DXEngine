#ifndef ENGINE_EXCEPTION_H
#define ENGINE_EXCEPTION_H

#include "stdafx.h"
#include <exception>
#include <sstream>
#include <string>

class engine_exception : public std::exception {
 public:
  engine_exception(const LPCSTR file, const int line) noexcept
      : file_(file), line_(line) {}

  auto what() const noexcept -> const char* override {
    std::ostringstream oss;
    oss << source() << "\n";
    what_buffer_ = oss.str();

    return what_buffer_.c_str();
  }

  virtual auto type() const noexcept -> LPCSTR { return type_; }

  auto source() const noexcept -> std::string {
    std::ostringstream oss;
    oss << "[File] " << file_ << "\n" << "[Line #] " << line_;

    return oss.str();
  }

  static auto translate_error_code(const HRESULT hr) noexcept -> std::string {
    char* msg_buf = nullptr;
    DWORD msg_len = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&msg_buf), 0, nullptr);

    if (msg_len == 0) return "Unidentified error code";

    std::string error_str = msg_buf;
    LocalFree(msg_buf);

    return error_str;
  }

 private:
  LPCSTR file_;
  int line_;

 protected:
  LPCSTR type_{"Engine Exception"};
  mutable std::string what_buffer_;
};

#endif  // ENGINE_EXCEPTION_H

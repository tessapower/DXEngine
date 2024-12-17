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

  auto what() const noexcept -> const char* override {
    return "Engine Exception";
  }

  /**
   * Returns the message associated with this exception. Intended use:
   * <example>
   * try {
   *   // ...
   * } * catch (const& engine_exception e) {
   *   // "Engine Exception: Lorem ipsum sit dolor amet."
   *   std::cerr << e.what() << ": " << e.msg() << "\n";
   * }
   * </example>
   */
  virtual auto msg() const noexcept -> LPCWSTR {
    std::wostringstream oss;
    oss << source() << "\n";
    what_buffer_ = oss.str();

    return what_buffer_.c_str();
  }

  virtual auto type() const noexcept -> LPCWSTR { return type_; }

  auto source() const noexcept -> std::wstring {
    std::wostringstream oss;
    oss << L"[File] " << file_ << L"\n" << L"[Line #] " << line_;

    return oss.str();
  }

  static auto translate_error_code(const HRESULT hr) noexcept -> std::wstring {
    wchar_t* msg_buf = nullptr;
    DWORD msg_len = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&msg_buf), 0, nullptr);

    if (msg_len == 0) return L"Unidentified error code";

    std::wstring error_str = msg_buf;
    LocalFree(msg_buf);

    return error_str;
  }

 private:
  LPCWSTR file_;
  int line_;

 protected:
  LPCWSTR type_{L"Engine Exception"};
  mutable std::wstring what_buffer_;
};

#endif  // ENGINE_EXCEPTION_H

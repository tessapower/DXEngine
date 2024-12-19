#ifndef MESSAGE_MAP_H
#define MESSAGE_MAP_H

#include "stdafx.h"
#include <unordered_map>
#include <string>

class message_map {
 public:
  message_map() = default;
  ~message_map() = default;
  std::string operator()(DWORD u_msg, WPARAM w_param, LPARAM l_param) const;
 private:
  static std::unordered_map<DWORD, std::string> messages_;
};

#endif  // MESSAGE_MAP_H

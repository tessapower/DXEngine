#ifndef MESSAGEMAP_H
#define MESSAGEMAP_H

#include <unordered_map>
#include <string>
#include "Windows.h"

class MessageMap {
 public:
  MessageMap() = default;
  ~MessageMap() = default;
  std::string operator()(DWORD uMsg, WPARAM wParam, LPARAM lPARAM) const;

 private:
  static std::unordered_map<DWORD, std::string> messages;
};

#endif  // MESSAGEMAP_H

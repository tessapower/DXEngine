#ifndef MESSAGEMAP_H
#define MESSAGEMAP_H

#include "ChiliWin.h"
#include <unordered_map>
#include <string>

class MessageMap {
 public:
  MessageMap() = default;
  ~MessageMap() = default;
  std::string operator()(DWORD uMsg, WPARAM wParam, LPARAM lPARAM) const;
 private:
  static std::unordered_map<DWORD, std::string> messages;
};

#endif  // MESSAGEMAP_H

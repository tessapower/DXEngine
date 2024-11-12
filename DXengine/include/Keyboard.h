#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <bitset>
#include <optional>
#include <queue>

class Keyboard {
 public:
  Keyboard() = default;
  Keyboard(const Keyboard&) = delete;
  Keyboard& operator=(const Keyboard&) = delete;
  ~Keyboard() = default;

  struct KeyEvent {
    enum class Type { PRESSED, RELEASED };

    KeyEvent(Type type, unsigned char keyCode)
        : _type(type), _keyCode(keyCode) {}

    Type type() const noexcept { return _type; }

    unsigned char keyCode() const noexcept { return _keyCode; }

    friend bool operator==(KeyEvent const& lhs, KeyEvent const& rhs) {
      return lhs._type == rhs._type && lhs._keyCode == rhs._keyCode;
    }

   private:
    const Type _type;
    const unsigned char _keyCode;
  };

  std::optional<KeyEvent> nextEvent() {
    if (!keyEvents.empty()) {
      KeyEvent& event = keyEvents.front();
      keyEvents.pop();
      return std::optional<KeyEvent>{event};
    }

    return {};
  }

  std::optional<const unsigned char> nextChar() {
    if (!charBuffer.empty()) {
      auto ch = charBuffer.front();
      charBuffer.pop();
      return std::optional<unsigned char>{ch};
    }

    return {};
  }

  void clearEvents() noexcept {
    if (!keyEvents.empty()) {
      keyEvents = std::queue<KeyEvent>{};
    }
  }

  void clearKeyStates() noexcept { keyStates.reset(); }

  void clearCharBuffer() noexcept {
    if (!charBuffer.empty()) {
      charBuffer = std::queue<unsigned char>{};
    }
  }

  bool isPressed(unsigned char keyCode) const noexcept {
    return keyStates.test(keyCode);
  }

 protected:
  friend class app;
  // These functions are intended to be used
  // by the Window in response to key events
  void onKeyPress(const unsigned char keyCode) noexcept {
    keyEvents.emplace(KeyEvent::Type::PRESSED, keyCode);
    keyStates.set(keyCode);
  }

  void onKeyRelease(const unsigned char keyCode) noexcept {
    keyEvents.emplace(KeyEvent::Type::RELEASED, keyCode);
    keyStates.reset(keyCode);
  }

  void onChar(const unsigned char c) {
    charBuffer.push(c);
  }

 private:
  static constexpr unsigned int numKeys = 256;
  std::bitset<numKeys> keyStates;

  std::queue<KeyEvent> keyEvents;
  std::queue<unsigned char> charBuffer;
};

#endif // KEYBOARD_H

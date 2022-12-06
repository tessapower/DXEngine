#include "pch.h"
#include "CppUnitTest.h"
#include "Keyboard.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class MockKbd : public Keyboard {
 public:
  MockKbd() = default;

  void press(unsigned char keyCode) noexcept { onKeyPress(keyCode); }

  void release(unsigned char keyCode) noexcept { onKeyRelease(keyCode); }
  
  void onCh(unsigned char c) { onChar(c); }
};

namespace KeyboardTests {
  TEST_CLASS(KeyboardTests) {
   public:
    TEST_METHOD(ReceivePressEvents){
      MockKbd kbd;
      for (const unsigned char c : "henlo") {
        Assert::IsFalse(kbd.isPressed(c));
        kbd.press(c);
        Assert::IsTrue(kbd.isPressed(c));

        auto maybeEvent = kbd.nextEvent();
        Assert::IsTrue(maybeEvent.has_value());
        MockKbd::KeyEvent& event = maybeEvent.value();
        Assert::IsTrue(event.type() == MockKbd::KeyEvent::Type::PRESSED);
        Assert::AreEqual(event.keyCode(), c);
      }
    }

    TEST_METHOD(ReceiveReleaseEvents){
      MockKbd kbd;

      for (const unsigned char c : "water") {
        kbd.release(c);
        Assert::IsFalse(kbd.isPressed(c));

        auto maybeEvent = kbd.nextEvent();
        Assert::IsTrue(maybeEvent.has_value());
        MockKbd::KeyEvent& event = maybeEvent.value();
        Assert::IsTrue(event.type() == MockKbd::KeyEvent::Type::RELEASED);
        Assert::AreEqual(event.keyCode(), c);
      }
    }

    TEST_METHOD(ReceiveOnCharEvents) {
      MockKbd kbd;

      for (const unsigned char c : "chicken nuggets") {
        kbd.onCh(c);
      }

      for (const unsigned char c : "chicken nuggets") {
        auto ch = kbd.nextChar();
        Assert::IsTrue(ch.has_value());
        Assert::AreEqual(c, ch.value());

      }
    }

    TEST_METHOD(ClearCharBuffer) {
      MockKbd kbd;

      for (const unsigned char c : "numbers") {
        kbd.onCh(c);
      }

      // Make sure we definitely put something onto the buffer
      Assert::IsTrue(kbd.nextChar().has_value());
      kbd.clearCharBuffer();
      Assert::IsFalse(kbd.nextChar().has_value());
    }

    TEST_METHOD(ClearEvents) {
      MockKbd kbd;

      for (const unsigned char c : "pingu") {
        kbd.press(c);
      }

      // Make sure we definitely put something onto the Events queue
      Assert::IsTrue(kbd.nextEvent().has_value());
      kbd.clearEvents();
      Assert::IsFalse(kbd.nextEvent().has_value());
    }

    TEST_METHOD(ClearKeyState) {
      MockKbd kbd;

      for (const unsigned char c : "blah") {
        kbd.press(c);
        Assert::IsTrue(kbd.isPressed(c));
      }

      kbd.clearKeyStates();

      for (const unsigned char c : "blah") {
        Assert::IsFalse(kbd.isPressed(c));
      }
    }
  };
}

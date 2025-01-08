#ifndef DXGI_INFO_MANAGER_H
#define DXGI_INFO_MANAGER_H
#include "stdafx.h"

#pragma comment(lib, "dxguid.lib")

#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include <wrl.h>

#include <string>
#include <vector>

#include "engine_exception.h"
#include "exception_macros.h"

// Helper class for DXGI Debug Layer
class dxgi_info_manager {
 public:
  dxgi_info_manager() {
    // Load the debug library
    typedef HRESULT(WINAPI * DXGIGetDebugInterface)(REFIID, void*);
    HMODULE dxgi_debug =
        LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dxgi_debug) {
      throw std::exception("Failed to load dxgidebug.dll");
    }

    auto dxgi_get_debug_interface = reinterpret_cast<DXGIGetDebugInterface>(
        GetProcAddress(dxgi_debug, "DXGIGetDebugInterface"));

    if (!dxgi_get_debug_interface) {
      throw std::exception("Failed to get DXGIGetDebugInterface function");
    }

    // Create the debug interface
    Microsoft::WRL::ComPtr<IDXGIDebug> p_debug;
    HRESULT hr =
        dxgi_get_debug_interface(__uuidof(IDXGIDebug), (void**)&p_debug);
    if (FAILED(hr)) {
      FreeLibrary(dxgi_debug);
      throw std::exception("Failed to create DXGI Debug Interface");
    }

    // Get the info queue
    hr = dxgi_get_debug_interface(__uuidof(IDXGIInfoQueue),
                                  (void**)&p_info_queue_);

    if (FAILED(hr)) {
      p_debug->Release();
      FreeLibrary(dxgi_debug);

      throw std::exception("Failed to create DXGI Info Queue");
    }

    p_debug->Release();
  }

  ~dxgi_info_manager() = default;

  dxgi_info_manager(dxgi_info_manager const&) = delete;
  auto operator=(const dxgi_info_manager&) -> dxgi_info_manager& = delete;
  dxgi_info_manager(dxgi_info_manager const&&) = delete;
  auto operator=(const dxgi_info_manager&&) -> dxgi_info_manager& = delete;

  auto set() -> void {
    // Set index of next message in info queue
    next_ = p_info_queue_->GetNumStoredMessages(DXGI_DEBUG_ALL);
  }

  // Returns all latest messages from DXGI Debug Layer.
  [[nodiscard]] auto messages() const -> std::vector<std::string> {
    std::vector<std::string> messages;
    const auto end = p_info_queue_->GetNumStoredMessages(DXGI_DEBUG_ALL);
    for (auto i = next_; i < end; ++i) {
      HRESULT hr;
      SIZE_T message_len = 0;

      // Get length of message at index i
      HR_THROW_ON_FAIL(
          p_info_queue_->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &message_len));

      // Allocate memory buffer for message
      auto bytes = std::make_unique<byte[]>(message_len);
      const auto p_msg =
          reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

      // Add message description to vector
      HR_THROW_ON_FAIL(
          p_info_queue_->GetMessage(DXGI_DEBUG_ALL, i, p_msg, &message_len));

      messages.emplace_back(p_msg->pDescription);
    }

    return messages;
  }

 private:
  unsigned long long next_ = 0u;
  Microsoft::WRL::ComPtr<IDXGIInfoQueue> p_info_queue_;
};

#endif  // DXGI_INFO_MANAGER_H

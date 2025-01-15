#ifndef SHADER_H
#define SHADER_H

#include "stdafx.h"
#include "bindable.h"
#include "engine_exception.h"
#include "renderer/renderer.h"

#include <filesystem>
#include <d3dcompiler.h>

class shader : public bindable {
public:
 class exception : public engine_exception {
  public:
   exception(const char* file, int line, const std::string& error_text) noexcept
       : engine_exception(file, line), error_text_(error_text) {}

   auto what() const noexcept -> const char* override {
     std::ostringstream oss;
     oss << engine_exception::what() << "\n" << "[Error] " << error_text_;
     what_buffer_ = oss.str();
     return what_buffer_.c_str();
   }

  private:
   std::string error_text_;
 };

 shader(std::filesystem::path const& filename) {
   auto path = std::filesystem::path(SHADER_SRC_DIR) / filename;
   path_ = path.wstring();

   throw exception(__FILE__, __LINE__, file_not_found_);
 }

 virtual auto bind(renderer& rndr) noexcept -> void = 0;

protected:
 std::wstring path_;
 const std::string file_not_found_ = "Error: shader file not found.\n";
 LPCSTR entry_point_;
 LPCSTR target_;

 Microsoft::WRL::ComPtr<ID3DBlob> blob_;
 Microsoft::WRL::ComPtr<ID3DBlob> error_blob_;

 virtual auto create(renderer& rndr) -> HRESULT = 0;

 auto compile() -> HRESULT {
   return D3DCompileFromFile(
       path_.c_str(),                                    // HLSL file
       nullptr,                                          // Macros
       nullptr,                                          // Include handler
       entry_point_,                                     // Entry point
       target_,                                          // Target shader model
       D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,  // Compile flags
       0,                                                // Effect flags
       blob_.GetAddressOf(),       // Compiled shader output
       error_blob_.GetAddressOf()  // Error messages
   );
 }
};

#endif // SHADER_H

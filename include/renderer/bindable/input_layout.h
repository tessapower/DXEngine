#ifndef INPUT_LAYOUT_H
#define INPUT_LAYOUT_H

#include "bindable.h"
#include "exception_macros.h"

class input_layout : public bindable {
 public:
  input_layout(renderer& rndr,
    const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
    ID3DBlob* vertex_shader_bytecode) {
    HRESULT hr;
    auto& dxgi_info_mgr_ = info_manager(rndr);
    RENDER_THROW_HR_MSG(
      device(rndr)->CreateInputLayout(
        layout.data(),
        static_cast<UINT>(layout.size()),
        vertex_shader_bytecode->GetBufferPointer(),
        vertex_shader_bytecode->GetBufferSize(),
        &input_layout_
      )
    );
  }

  virtual ~input_layout() = default;
  
  input_layout(const input_layout&) = delete;
  auto operator=(const input_layout&) -> input_layout& = delete;

  auto bind(renderer& rndr) noexcept -> void override {
    context(rndr)->IASetInputLayout(input_layout_.Get());
  }

 protected:
  Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
};

#endif // INPUT_LAYOUT_H

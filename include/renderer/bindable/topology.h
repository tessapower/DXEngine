#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "bindable.h"

class topology : public bindable {
 public:
  topology(renderer& rndr, D3D11_PRIMITIVE_TOPOLOGY type) : type_(type) {}
  
  auto bind(renderer& rndr) noexcept -> void override {
    context(rndr)->IASetPrimitiveTopology(type_);
  }

 protected:
  D3D11_PRIMITIVE_TOPOLOGY type_;
};

#endif // TOPOLOLGY_H

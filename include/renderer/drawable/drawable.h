#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "stdafx.h"
#include "renderer/bindable/bindable.h"
#include "renderer/bindable/index_buffer.h"

#include <DirectXMath.h>
#include <vector>

class drawable {
 public:
  drawable() = default;
  virtual ~drawable() = default;

  virtual auto transform() const noexcept -> DirectX::XMMATRIX = 0;
  virtual auto update(float dt_ms) noexcept-> void = 0;

  auto draw(renderer& rndr) const noexcept -> void {
    for (auto& b : binds_) {
      b->bind(rndr);
    }

    rndr.draw_indexed(index_buffer_->count());
  }

  auto add_bind(std::unique_ptr<bindable> bind) noexcept -> void {
    // Check if we are adding an index buffer and store it
    if (typeid(bind) == typeid(index_buffer)) {
      // Ensure we haven't already added an index buffer
      if (!index_buffer_) {
        index_buffer_ = dynamic_cast<index_buffer*>(bind.get());
      } else {
        return;
      }
    }

    binds_.push_back(std::move(bind));
  }

  drawable(const drawable&) = delete;

 private:
  const index_buffer* index_buffer_ = nullptr;
  std::vector<std::unique_ptr<bindable>> binds_;
};

#endif // DRAWABLE_H

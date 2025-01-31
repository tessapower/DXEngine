#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "stdafx.h"
#include "renderer/bindable/bindable.h"
#include "renderer/bindable/index_buffer.h"

#include <DirectXMath.h>
#include <vector>


class drawable {
  template <typename T>
  friend class drawable_base;
 public:
  drawable() = default;
  virtual ~drawable() = default;

  virtual auto transform() const noexcept -> DirectX::XMMATRIX = 0;
  virtual auto update(float dt_ms) noexcept-> void = 0;

  auto draw(renderer& rndr) -> void {
    // Bind all bindable objects
    for (auto& b : binds_) {
      b->bind(rndr);
    }

    // Bind all static bindable objects
    for (auto& b : static_binds()) {
      b->bind(rndr);
    }

    rndr.draw_indexed(index_buffer_->count());
  }

 protected:
  auto add_bind(std::unique_ptr<bindable> bind) noexcept -> void {
    // Check if we are adding an index buffer and store it
    if (typeid(*bind) == typeid(index_buffer)) {
      // Ensure we haven't already added an index buffer
      if (!index_buffer_) {
        index_buffer_ = dynamic_cast<index_buffer*>(bind.get());
      } else {
        return;
      }
    }

    binds_.push_back(std::move(bind));
  }

 private:
  virtual auto static_binds() const noexcept
      -> const std::vector<std::unique_ptr<bindable>>& = 0;
  const class index_buffer* index_buffer_ = nullptr;
  std::vector<std::unique_ptr<bindable>> binds_;
};

#endif // DRAWABLE_H

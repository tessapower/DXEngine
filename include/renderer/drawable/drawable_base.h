#ifndef DRAWABLE_BASE_H
#define DRAWABLE_BASE_H

#include "renderer/drawable/drawable.h"
#include "renderer/bindable/bindable.h"

template <typename T>
class drawable_base : public drawable {
 public:
  auto is_static_initialized() const noexcept -> bool { return !static_binds_.empty(); }

  auto add_static_bind(std::unique_ptr<bindable> bind) noexcept {
    // Check if we are adding an index buffer and store it
    if (typeid(*bind) == typeid(index_buffer)) {
      // Ensure we haven't already added an index buffer
      if (index_buffer_) return;
      index_buffer_ = dynamic_cast<index_buffer*>(bind.get());
    }

    static_binds_.push_back(std::move(bind));
  }

  private:
  const std::vector<std::unique_ptr<bindable>>& static_binds() const noexcept {
     return static_binds_;
  }

  static std::vector<std::unique_ptr<bindable>> static_binds_;
};

template <typename T>
std::vector<std::unique_ptr<bindable>> drawable_base<T>::static_binds_;

#endif // DRAWABLE_BASE_H

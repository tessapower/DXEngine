#ifndef GUI_H
#define GUI_H

#include "stdafx.h"
#include <imgui.h>

class gui {
  int width_;
  int height_;
  ImGuiIO* io_;
  ImVec4 clear_color_ = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

 public:
  explicit gui(int viewport_width = 0, int viewport_height = 0);
  gui(const gui&) = delete;
  gui(const gui&&) = delete;
  ~gui();

  auto init() -> void;

  auto set_viewport(int width, int height) noexcept -> void;

  auto update() const noexcept -> void;

  auto render() const noexcept -> void;

};

#endif  // GUI_H

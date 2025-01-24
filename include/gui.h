#ifndef GUI_H
#define GUI_H
#include "stdafx.h"

#include <imgui.h>

class renderer;

class gui {
  float width_;
  float height_;
  ImGuiIO* io_;
  ImVec4 clear_color_ = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

 public:
  explicit gui(float viewport_width = 0, float viewport_height = 0);
  gui(const gui&) = delete;
  gui& operator=(const gui&) = delete;
  gui(const gui&&) = delete;
  gui& operator=(const gui&&) = delete;
  ~gui();

  auto init() -> void;

  auto set_viewport(float width, float height) noexcept -> void;

  auto update() const noexcept -> void;

  auto render(renderer const& r) const noexcept -> void;
};

#endif  // GUI_H

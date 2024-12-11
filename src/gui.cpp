#include "stdafx.h"
#include "gui.h"

#include <imgui_impl_dx11.h>
#include "renderer.h"

gui::gui(const int viewport_width, const int viewport_height) {
  width_ = viewport_width;
  height_ = viewport_height;
  io_ = nullptr;
}

gui::~gui() { ImGui::DestroyContext(); }

auto gui::set_viewport(const int width, const int height) noexcept -> void {
  width_ = width;
  height_ = height;
}

auto gui::init() -> void {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io_ = &ImGui::GetIO();
  (void)io_;
  io_->ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io_->ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
}

auto gui::update() const noexcept -> void {
  // Start a new ImGui frame, which ends when we call ImGui::Render().
  ImGui::NewFrame();

  {
    ImGui::Begin("Shaders");
    ImGui::Text(
        "This window will display a list of shaders that \nare dynamically "
        "loaded and applied to the scene.");

    ImGui::Spacing();

    ImGui::ColorEdit3(
        "clear color",
        const_cast<float *>(reinterpret_cast<const float *>(
            &clear_color_)));  // Edit 3 floats representing a color

    // TODO: Add bullet list of shaders to pick from

    ImGui::Spacing();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / io_->Framerate, io_->Framerate);
    ImGui::End();
  }
}

/**
 * @brief Render the GUI to the window using the given <code>renderer</code>.
 */
auto gui::render(renderer const& r) const noexcept -> void {
  // End the ImGui frame.
  ImGui::Render();

  const float clear_color_with_alpha[4] = {
      clear_color_.x * clear_color_.w, clear_color_.y * clear_color_.w,
      clear_color_.z * clear_color_.w, clear_color_.w};

  r.clear_back_buffer(clear_color_with_alpha);

  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

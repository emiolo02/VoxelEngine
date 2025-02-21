#include "Inspector.hpp"
#include "imgui.h"

void
Inspector::Draw() {
  if (m_ShowViewport) {
    ImGui::Begin("Viewport", nullptr,
                 ImGuiWindowFlags_NoMove
                 | ImGuiWindowFlags_NoResize
                 | ImGuiWindowFlags_NoInputs
                 | ImGuiWindowFlags_NoBackground
    );
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowPadding = {0.0f, 0.0f};
    ImGui::SetWindowPos({0, 0});
    if (m_Textures.find("Viewport") != m_Textures.end()) {
      auto &texture = m_Textures["Viewport"];
      const ImVec2 dims = {(float) texture.width, (float) texture.height};
      ImGui::SetWindowSize(dims);
      const ImVec2 imSize = {dims.x, dims.y - 20};
      ImGui::Image((ImTextureID) texture.id, imSize, {0, 1}, {1, 0});
    }

    ImGui::End();
  }

  ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None);
  ImGui::SetWindowSize({200, 400});
  ImGui::Checkbox("Show viewport", &m_ShowViewport);

  for (auto &[name, value]: m_Ints)
    ImGui::DragInt(name.c_str(), &value);
  for (auto &[name, value]: m_Bools)
    ImGui::Checkbox(name.c_str(), &value);
  for (auto &[name, value]: m_Floats)
    ImGui::DragFloat(name.c_str(), &value.f, value.step);
  for (auto &[name, vec]: m_Vec2s)
    ImGui::DragFloat2(name.c_str(), &vec[0], 0.1f);
  for (auto &[name, vec]: m_Vec3s)
    ImGui::DragFloat3(name.c_str(), &vec[0], 0.1f);
  for (auto &[name, vec]: m_Vec4s)
    ImGui::DragFloat4(name.c_str(), &vec[0], 0.1f);

  ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

  ImGui::End();
}

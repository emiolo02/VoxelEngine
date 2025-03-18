#pragma once

#include "Display/Window.hpp"
#include "UserInterface/Inspector.hpp"
#include "Render/Texture/Texture.hpp"

class Inspector;

class App {
public:
  void SetArgs(const string &modelPath, const uint32 subdivisions) {
    m_ModelPath = modelPath;
    m_Subdivisions = subdivisions;
  }

  bool Open();

  void Run();

  void Close();

private:
  Display::Window m_Window;
  Inspector m_Inspector;

  const string m_Title = "App";

  string m_ModelPath;
  uint32 m_Subdivisions = 0;
};

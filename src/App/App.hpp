#pragma once

#include "Display/Window.hpp"
#include "UserInterface/Inspector.hpp"
#include "Render/Texture/Texture.hpp"

class Inspector;

class App {
public:
  bool Open();

  void Run();

  void Close();

private:
  Display::Window m_Window;
  Inspector m_Inspector;
  const string m_Title = "App";
};

#pragma once
#include <map>
#include <string>

namespace Display
{
	class Window;
}

class World;
class Object;
class Inspector
{
public:
	static Inspector* Instance();

	void Init(Display::Window* window, World* world);

	void RenderUI();

	void SelectObject(int const& id);

	void AddCheckbox(std::string identifier, bool value = false);

	bool GetCheckbox(std::string identifier);

	void AddFloat(std::string identifier, float value = 0);

	float GetFloat(std::string identifier);

private:
	Inspector(){}

	bool show = false;
	int selectedObject = -1;

	World* world = nullptr;
	Display::Window* window = nullptr;

	std::map<std::string, bool> checkboxes;
	std::map<std::string, float> floats;

	static Inspector* instance;
};
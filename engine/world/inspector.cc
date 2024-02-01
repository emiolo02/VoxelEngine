#include "config.h"
#include "inspector.h"
#include "world.h"

#include "object.h"
#include "imgui.h"
#include "render/window.h"

Inspector* 
Inspector::Instance()
{
	if (instance == nullptr)
		instance = new Inspector();

	return instance;
}

void 
Inspector::Init(Display::Window* window, World* world)
{
	this->window = window;

	this->world = world;
}

void
Inspector::RenderUI()
{
	if (this->window->IsOpen())
	{

		// create a new window
		if (!ImGui::Begin("Inspector", &show, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}

		ImGui::StyleColorsDark(&ImGui::GetStyle());

		auto& objects = world->GetObjects();

		ImGui::ListBoxHeader("Objects");
		for (int n = 0; n < objects.size(); n++)
		{
			const bool selected = (selectedObject == n);

			if (ImGui::Selectable(objects[n]->GetName().c_str(), selected))
				SelectObject(n);
			

			//if (selected)
			//	ImGui::SetItemDefaultFocus();
		}
		ImGui::ListBoxFooter();

		if (selectedObject >= 0)
		{
			auto& obj = objects[selectedObject];
			const auto& rb = obj->GetRigidBody();
			
			ImGui::Text(obj->GetName().c_str());
			ImGui::DragFloat3("Position", &rb->position[0], 0.1f);
			ImGui::DragFloat4("orientation", &rb->orientation[0], 1.0f);
			ImGui::DragFloat3("Scale", &obj->scale[0], 0.1f);

			ImGui::DragFloat("Inverse mass", &rb->invMass, 0.5f);
			ImGui::DragFloat3("Velocity", &rb->velocity[0]);
			ImGui::DragFloat3("Acceleration", &rb->acceleration[0]);
			ImGui::DragInt("Collision count", &PhysicsWorld::Instance()->resolvedCollisions);
		}

		/*ImGui::DragFloat3("pLight pos", &world->pointLights[0].pos[0], 0.1f);
		ImGui::ColorEdit3("pLight color", &world->pointLights[0].color[0]);
		ImGui::DragFloat("pLight intensity", &world->pointLights[0].intensity, 0.1f);*/
		if (ImGui::Button("Start gravity"))
			PhysicsWorld::Instance()->StartGravity();

		for (auto& checkbox : checkboxes)
			ImGui::Checkbox(checkbox.first.c_str(), &checkbox.second);

		for (auto& flt : floats)
			ImGui::DragFloat(flt.first.c_str(), &flt.second, 0.1f);

		ImGui::DragFloat3("Camera position", &Camera::GetCamera(CAMERA_MAIN)->position[0]);
		ImGui::DragFloat3("Camera direction", &Camera::GetCamera(CAMERA_MAIN)->direction[0]);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

void Inspector::SelectObject(int const& id)
{
	if (selectedObject >= 0)
		world->GetObject(selectedObject)->SetColor(vec3(1.0f, 1.0f, 1.0f));
	if (id < 0)
	{
		selectedObject = -1;
		return;
	}

	selectedObject = id;
	world->GetObject(id)->SetColor(vec3(1.0f, 0.7f, 0.7f));
}

void
Inspector::AddCheckbox(std::string identifier, bool value)
{
	if (checkboxes.find(identifier) != checkboxes.end())
	{
		std::cout << "\"" << identifier << "\"" << " is already a checkbox!" << std::endl;
		return;
	}

	checkboxes[identifier] = value;
}

bool
Inspector::GetCheckbox(std::string identifier)
{
	if (checkboxes.find(identifier) == checkboxes.end())
	{
		std::cout << "\"" << identifier << "\"" << " is not a checkbox!" << std::endl;
		return false;
	}

	return checkboxes[identifier];
}

void Inspector::AddFloat(std::string identifier, float value)
{
	if (floats.find(identifier) != floats.end())
	{
		std::cout << "\"" << identifier << "\"" << " is already a checkbox!" << std::endl;
		return;
	}

	floats[identifier] = value;
}

float Inspector::GetFloat(std::string identifier)
{
	if (floats.find(identifier) == floats.end())
	{
		std::cout << "\"" << identifier << "\"" << " is not a float!" << std::endl;
		return 0;
	}

	return floats[identifier];
}

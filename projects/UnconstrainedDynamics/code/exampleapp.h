#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2022 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <memory>
#include <chrono>
#include "core/app.h"
#include "input/Input.h"
#include "render/window.h"
#include "render/graphics/FrameBuffer.h"
#include "render/graphics/SkyBox.h"

class World;
class PhysicsWorld;
class Inspector;

namespace Example
{
class ExampleApp : public Core::App
{
public:
	/// constructor
	ExampleApp();
	/// destructor
	~ExampleApp();

	/// open app
	bool Open();
	/// close app
	void Close();
	/// run app
	void Run();

	void Render();

private:
	enum ShaderProgram
	{
		PROG_SKYBOX,
		PROG_GBUFFER,
		PROG_SCREENQUAD,
		PROG_BASIC,
		NUM_PROGRAMS
	};
	enum Buffer
	{
		BUFFER_GBUFFER,
		BUFFER_SHADOWMAP,
		NUM_BUFFERS
	};
	std::vector<GLuint> programs;
	std::vector<FrameBuffer> frameBuffers;

	Quad renderQuad; // Quad that will display the final render
	SkyBox skyBox;

	
	Inspector* inspector;
	Input::InputManager* manager;
	World* world;
	PhysicsWorld* physWorld;

	Display::Window* window = nullptr;
	int windowWidth = 0, windowHeight = 0;
};
} // namespace Example
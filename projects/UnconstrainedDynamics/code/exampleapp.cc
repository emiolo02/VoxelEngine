//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "config.h"
#include "exampleapp.h"
#include "render/graphics/GraphicsNode.h"
#include "render/graphics/Camera.h"
#include "render/graphics/PointLight.h"
#include "render/graphics/Sun.h"
#include "render/Debug.h"
#include "physics/Physics.h"
#include "world/object.h"
#include "world/world.h"

#include "render/graphics/FrameBuffer.h"
#include "world/inspector.h"
#include "imgui.h"

Inspector* Inspector::instance = nullptr;

using namespace Display;
namespace Example
{

//------------------------------------------------------------------------------
/**
*/
ExampleApp::ExampleApp()
{
	this->programs.resize(NUM_PROGRAMS);
	this->frameBuffers.resize(NUM_BUFFERS);
}

//------------------------------------------------------------------------------
/**
*/
ExampleApp::~ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ExampleApp::Open()
{
	App::Open();
	this->window = new Display::Window;
	this->window->SetTitle("Example");
	this->window->SetSize(1920, 1080);
	window->GetSize(this->windowWidth, this->windowHeight);


	manager = manager->Instance();
	world = World::Instance();
	physWorld = PhysicsWorld::Instance();
	inspector = Inspector::Instance();
	inspector->Init(window, world);

	window->SetKeyPressFunction([this](int32 key, int32, int32 action, int32)
	{
        manager->HandleKeyEvent(key, action);
		if (key == GLFW_KEY_ESCAPE)
			this->window->Close();
	});

	window->SetMouseMoveFunction([this](float64 x, float64 y)
	{
			manager->HandleMousePosition(x, y, this->windowWidth, this->windowHeight);
	});

	window->SetMousePressFunction([this](int32 button, int32 action, int32)
	{
			manager->HandleMouseButton(button, action);
	});

	this->window->SetUiRender([this]()
		{
			inspector->RenderUI();
		});

	return this->window->Open();
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Close()
{
	if (this->window->IsOpen())
		this->window->Close();

	Core::App::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Run()
{
	Debug::Init();
	//// Add options to inspector
	inspector->AddCheckbox("Render debug");
	inspector->AddCheckbox("Impulse on click", true);
	inspector->AddFloat("Impulse force", 2);
	
	//// Generate shaders
	programs[PROG_SKYBOX] = ShaderResource::Instance()->CreateProgram("../Shaders/SkyBox.vs", "../Shaders/Skybox.fs");
	programs[PROG_GBUFFER] = ShaderResource::Instance()->CreateProgram("../Shaders/GBuffer.vs", "../Shaders/GBuffer.fs");
	programs[PROG_SCREENQUAD] = ShaderResource::Instance()->CreateProgram("../Shaders/ScreenQuad.vs", "../Shaders/ScreenQuad.fs");
	programs[PROG_BASIC] = ShaderResource::Instance()->CreateProgram("../Shaders/BasicShader.vs", "../Shaders/BasicShader.fs");

	//// Set up G buffer
	frameBuffers[BUFFER_GBUFFER].InitGBuffer(windowWidth, windowHeight);
 
	renderQuad.Init();

	//// Set up shadow map
	frameBuffers[BUFFER_SHADOWMAP].InitShadowBuffer(2048, 2048);
	
	//// Init skybox
	skyBox.Init();
	//// Generate lights

	// Sun
	world->GetSun().SetProperties(vec3(1, 1, 1), vec3(1, -1, 1), 0.5f);
	PointLight pLight(vec3(0, 4, 0), vec3(1, 1, 1), 1);
	world->pointLights.push_back(pLight);

	//// Load models

	//Object sponza("Assets/Sponza/Sponza.gltf", vec3(), vec3(0.05, 0.05, 0.05));

	/*Object cube0("Assets/Cube/Cube.gltf", vec3(0, 0, 0), vec3(100, 0.5, 100), vec3(), 1, true);
	cube0.SetName("Floor");*/

	////Object sphere("Assets/sphere.obj", vec3(1, 2.5, 0));

	//// BarramundiFish
	///*Object fish("Assets/BarramundiFish/BarramundiFish.gltf", vec3(0, 1, 0));
	//fish.scale = vec3(2, 2, 2);*/

	///*Object cube1("Assets/Cube/Cube.gltf", vec3(2, 2.5, 2), vec3(1), vec3(), 1000);
	//cube1.SetName("Cube1");*/
	//Object cube2("Assets/Cube/Cubawsfde.gltf", vec3(-2, 1.5, 0));
	//cube2.SetName("Cube2");
	//Object cube3("Assets/Cube/Cube.gltf", vec3(-2, 3.5, 0));
	//cube3.SetName("Cube3");

	Object toyCar("Assets/ToyCar.glb", vec3(0, 50, 0), vec3(0.01f));


	//Object helmet("Assets/DamagedHelmet.gltf", vec3(5, 2, 0));

	//Object cube2("Assets/cube.obj", vec3(0, 5, 0));

	// Camera
	Camera camera;
	camera.position = vec3(0, 2, 4);
	camera.view = glm::lookAt(camera.position, vec3(0, 0, 0), camera.up);
	camera.Perspective(45, windowWidth, windowHeight);
	Camera::AddCamera(&camera);

	Camera sunCam;
	sunCam.position = vec3(-5, 5, -5);
	sunCam.view = glm::lookAt(sunCam.position, sunCam.position + world->GetSun().direction, vec3(0, 0, 1));
	sunCam.Orthographic(-10, 10, 10, -10, 0.1, 50);
	Camera::AddCamera(&sunCam);

	std::vector<Physics::HitResult> hitResults;

	glEnable(GL_DEPTH_TEST);
	//glfwSwapInterval(0);
	
	int inX = 0, inY = 0, inZ = 0;

	float deltaSeconds = 0;
	long long frameCount = 0;
	while (this->window->IsOpen())
	{
		auto time1 = std::chrono::steady_clock::now();
		frameCount++;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


		manager->BeginFrame();
		manager->mouse.dx = 0;
		manager->mouse.dy = 0;
		this->window->Update();
		// UPDATE

		// Read input
		float right = 0, up = 0, forward = 0;
		if (manager->keyboard.held[Input::Key::W])
			forward = 1;
		if (manager->keyboard.held[Input::Key::S])
			forward = -1;
		if (manager->keyboard.held[Input::Key::D])
			right = 1;
		if (manager->keyboard.held[Input::Key::A])
			right = -1;
		if (manager->keyboard.held[Input::Key::Space])
			up = 1;
		if (manager->keyboard.held[Input::Key::LeftShift])
			up = -1;


		if (manager->mouse.held[Input::MouseButton::right])
			camera.FreeFly(vec3(right, up, forward), manager->mouse.dx, manager->mouse.dy, deltaSeconds);
		
		if (manager->mouse.pressed[Input::MouseButton::left])
		{
			vec3 dir = Physics::CastFromCam(manager->mouse.nx, manager->mouse.ny, camera);
			vec3 origin = camera.position;
			hitResults.clear();
			hitResults = Physics::RayCast(origin, dir);

			if (hitResults.size() > 0)
			{
				auto& closest = hitResults[0];
				for (int i = 1; i < hitResults.size(); i++)
					if (glm::distance(origin, closest.position) > glm::distance(origin, hitResults[i].position))
						closest = hitResults[i];
				inspector->SelectObject(closest.object->GetID());

				if (inspector->GetCheckbox("Impulse on click"))
				{
					float force = inspector->GetFloat("Impulse force");
					closest.object->GetRigidBody()->Impulse(dir, force, closest.position);
				}
			}
			else
			{
				inspector->SelectObject(-1);
			}
		}

		//Debug::DrawLine(ray.origin, ray.origin + ray.dir*10.0f, vec4(0, 1, 0, 1), 2);
		
		for (auto& hit : hitResults)
		{
			if (hit.hit)
			{
				Debug::DrawBox(hit.position, vec3(), vec3(0.1f, 0.1f, 0.1f), vec4(1, 0, 0, 1), 3);
			}
		}

		world->Update(deltaSeconds);
		physWorld->Update(deltaSeconds);

		Render();

		auto time2 = std::chrono::steady_clock::now();
		float lastDT = deltaSeconds;
		deltaSeconds = ImGui::GetIO().DeltaTime;
		//deltaSeconds = (float)std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1).count() / 1000000;


			//std::cout << 1 / deltaSeconds << " fps\n";
		/*if (frameCount % 200 == 0)
		{
			std::cout << deltaSeconds << " s\n";
		}*/
		
#ifdef CI_TEST
		// if we're running CI, we want to return and exit the application after one frame
		// break the loop and hopefully exit gracefully
		break;
#endif
	}
}

void ExampleApp::Render()
{
	// Shadow pass
	frameBuffers[BUFFER_SHADOWMAP].Bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(programs[PROG_BASIC]);

	for (auto& object : world->GetObjects())
	{
		object->SimpleDraw(*Camera::GetCamera(CAMERA_SUN), programs[PROG_BASIC]);
	}

	glViewport(0, 0, windowWidth, windowHeight);

	// Geometry pass
	frameBuffers[BUFFER_GBUFFER].BindWrite();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programs[PROG_GBUFFER]);

	for (auto& object : world->GetObjects())
	{
		UniformMat4("lightView", Camera::GetCamera(CAMERA_SUN)->projView, programs[PROG_GBUFFER]);
		object->Draw(*Camera::GetCamera(CAMERA_MAIN), programs[PROG_GBUFFER]);
	}

	// Deferred pass
	frameBuffers[BUFFER_GBUFFER].BindRead();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	glBlitFramebuffer(
		0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);

	glUseProgram(programs[PROG_SCREENQUAD]);

	// Send light uniforms
	world->GetSun().Update(programs[PROG_SCREENQUAD]);

	UniformInt("NUM_LIGHTS", world->pointLights.size(), programs[PROG_SCREENQUAD]);
	for (auto& pointLight : world->pointLights)
		pointLight.Update(programs[PROG_SCREENQUAD]);


	// Send camera position uniform
	UniformVec3("cameraPos", Camera::GetCamera(CAMERA_MAIN)->position, programs[PROG_SCREENQUAD]);

	// Render screen quad
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, frameBuffers[BUFFER_SHADOWMAP].textures[0]);


	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);

	frameBuffers[BUFFER_GBUFFER].Draw(renderQuad);

	glDepthMask(GL_TRUE);


	// Draw skybox
	glDepthFunc(GL_EQUAL);
	skyBox.Draw(*Camera::GetCamera(CAMERA_MAIN), programs[PROG_SKYBOX]);
	glDepthFunc(GL_LESS);

	// Draw debug
	if (!inspector->GetCheckbox("Render debug"))
		Debug::ClearQueue();
	Debug::RenderDebug();

	this->window->SwapBuffers();
}

} // namespace Example
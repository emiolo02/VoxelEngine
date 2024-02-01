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
#include <array>

#include "PerlinNoise.h"

#include <execution>

struct Sphere
{
	vec3 center = vec3(0);
	float radius = 0.0f;
	vec3 color = vec3(0);

	Sphere(){}

	Sphere(vec3 center, float radius, vec3 color) :
		center(center), radius(radius), color(color)
	{}

	bool RayHit(Physics::Ray const& ray, vec3& outColor, vec3& outPoint, vec3& outNormal)
	{
		vec3 oc = ray.origin - center;
		float b = 2.0f * glm::dot(oc, ray.dir);

		float a = glm::dot(ray.dir, ray.dir);
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;

		if (discriminant < 0)
		{
			outColor = vec3();
			return false;
		}

		float t = (-b - sqrt(discriminant)) / (2 * a);

		outPoint = ray.origin + ray.dir * t;
		outNormal = glm::normalize(center - outPoint);

		outColor = color;	
		return true;
	}
};
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
	this->window->SetTitle("Voxel Engine");
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

	delete grid;

	Core::App::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Run()
{
	Debug::Init();
	inspector->AddCheckbox("RTX ON", true);
	buffer.resize(windowWidth*windowHeight);
	horizontalIterator.resize(windowWidth);
	verticalIterator.resize(windowHeight);

	for (size_t i = 0; i < horizontalIterator.size(); i += 4)
		horizontalIterator[i] = i;

	for (size_t i = 0; i < verticalIterator.size(); i += 4)
		verticalIterator[i] = i;


	glEnable(GL_DEPTH_TEST);

	glGenTextures(1, &colorBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	programs[PROG_SCREENQUAD] = ShaderResource::Instance()->CreateProgram("Shaders/ScreenQuad.vs", "Shaders/ScreenQuadComp.fs");
	programs[PROG_COMPUTE] = ShaderResource::Instance()->CreateComputeShader("Shaders/VoxelTraverse.comp");
	renderQuad.Init();

	Camera cam;
	cam.position = vec3(0, 0, -5);
	cam.direction = vec3(0, 0, 1);
	cam.view = glm::lookAt(cam.position, vec3(0), cam.up);
	cam.Perspective(45, windowWidth, windowHeight);
	Camera::AddCamera(&cam);

	grid = new VoxelGrid(400, 400, 400, 0.1f, vec3(-20, -20, -20));
	grid->GenerateProcedural();

	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glUseProgram(programs[PROG_COMPUTE]);
	grid->Upload(programs[PROG_COMPUTE]);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	//glTexImage3D(GL_TEXTURE_3D,)


	int inX = 0, inY = 0, inZ = 0;

	

	float deltaSeconds = 0;
	long long frameCount = 0;
	//glfwSwapInterval(0);
	while (this->window->IsOpen())
	{
		deltaSeconds = ImGui::GetIO().DeltaTime;
		frameCount++;

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
		{
			//this->window->LockCursor();
			cam.FreeFly(vec3(right, up, forward), manager->mouse.dx, manager->mouse.dy, deltaSeconds);
		}

		if (manager->mouse.pressed[Input::MouseButton::left])
		{
			Physics::Ray r(cam.position, Physics::CastFromCam(manager->mouse.nx, manager->mouse.ny, cam));

			grid->RayTraverse(r);
			//grid->FillPath(grid->GetRayPath(r));
		}

		

		world->GetSun().direction = vec3(sin(glfwGetTime()), -1, cos(glfwGetTime()));

		Render();
		

#ifdef CI_TEST
		// if we're running CI, we want to return and exit the application after one frame
		// break the loop and hopefully exit gracefully
		break;
#endif
	}
}

void ExampleApp::Render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto& cam = *Camera::GetCamera(CAMERA_MAIN);
	mat4 invProj = glm::inverse(cam.projection);
	mat4 invView = glm::inverse(cam.view);

	vec3 sunDir = world->GetSun().direction;

	if (inspector->GetCheckbox("RTX ON"))
	{
		//glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glUseProgram(programs[PROG_COMPUTE]);
		UniformVec3("cam.position", cam.position, programs[PROG_COMPUTE]);
		UniformVec3("cam.direction", cam.direction, programs[PROG_COMPUTE]);
		UniformMat4("cam.invProj", invProj, programs[PROG_COMPUTE]);
		UniformMat4("cam.invView", invView, programs[PROG_COMPUTE]);
		glBindImageTexture(0, colorBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glDispatchCompute(std::ceil((float)windowWidth/8), std::ceil((float)windowHeight/4), 1);
		//glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glUseProgram(programs[PROG_SCREENQUAD]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		renderQuad.Bind();

		glDepthFunc(GL_ALWAYS);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	}
	else
	{
		std::for_each(std::execution::par, verticalIterator.begin(), verticalIterator.end(),
			[this, cam, invProj, invView](size_t y)
			{
				std::for_each(horizontalIterator.begin(), horizontalIterator.end(),
				[this, cam, invProj, invView, y](size_t x)
					{
						auto& pixel = buffer[x + y * windowWidth];
						vec2 coord = vec2((float)x / windowWidth, (float)y / windowHeight) * 2.0f - 1.0f;

						Physics::Ray r(cam.position, Physics::CastFromCam(coord.x, -coord.y, invProj, invView));

						pixel = grid->RayTraverse(r);
					});
			});
		
		this->window->Blit((uint8*)&buffer[0], windowWidth, windowHeight);
	}

	this->window->SwapBuffers();
	}

} // namespace Example
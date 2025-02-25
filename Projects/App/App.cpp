#include "App.hpp"
#include <GL/glew.h>
#include <GL/gl.h>
#include "GLFW/glfw3.h"

#include "Input/InputManager.hpp"
#include "Input/Keyboard.hpp"


#include "Render/GraphicsNode.hpp"
#include "Render/Shader/Shader.hpp"
#include "Render/Renderer.hpp"
#include "Render/Shader/StorageBuffer.hpp"

#include "DataStructures/BrickMap.hpp"
#include "DataStructures/VoxelGrid.hpp"

#include "FirstPersonCamera.hpp"
#include "Terrain.hpp"

#include "Render/Model/ObjLoader.hpp"
#include "Render/Model/Voxelizer.hpp"
#include "Render/Debug.hpp"

#include "Render/Texture/Image.hpp"

bool
App::Open() {
  m_Window = Display::Window();
  m_Window.SetTitle(m_Title);

  if (!m_Window.Open()) {
    std::cout << "Failed to open window!\n";
    return false;
  }

  m_Inspector = Inspector();

  Input::InputManager &inputManager = Input::InputManager::Get();

  m_Window.SetUiRender([this]() {
    m_Inspector.Draw();
  });

  m_Window.SetKeyPressFunction(
    [this, &inputManager](int32 key, int32, int32 action, int32) {
      inputManager.HandleKeyEvent(key, action);
    });

  m_Window.SetMousePressFunction(
    [this, &inputManager](int32 button, int32 action, int32) {
      inputManager.HandleMouseButton(button, action);
    });

  m_Window.SetMouseMoveFunction([this, &inputManager](float64 x, float64 y) {
    int32 w, h;
    m_Window.GetSize(w, h);
    inputManager.HandleMousePosition(x, y, w, h);
  });

  m_Window.SetMouseScrollFunction([this, &inputManager](float32 x, float32 y) {
    inputManager.HandleMouseScroll(x, y);
  });

  m_Window.SetWindowResizeFuncion([this](int32 width, int32 height) {
    m_Window.SetSize(width, height, false);
    //m_RenderTexture = Texture(width, height);
  });


  return true;
}

void
App::Run() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

  Debug::Init();


#define MODEL_PATH "assets/dragon_vrip.ply"

  auto &model = ObjLoader::Get().Load(MODEL_PATH);
  //ImageManager::Get().Save(4, "sponzatest.png");
  OctreeMesh octree(model, 11);

  std::vector<GraphicsNode> graphicsNodes;
  graphicsNodes.reserve(model.meshes.size());
  for (const Mesh &mesh: model.meshes) {
    GraphicsNode &node = graphicsNodes.emplace_back(mesh);
    node.GetTransform().scale = vec3(0.001f);
  }
  //ModelBVH bvh(model, 10);

  ObjLoader::Get().Remove(MODEL_PATH);

  BrickMap brickMap = octree.CreateBrickMap(0.1f);
  brickMap.PrintByteSize();
  //brickMap.PrintByteSize();

  //octree.Clear();


  Renderer renderer;
  renderer.SetBrickMap(&brickMap);

  renderer.GetBrickGridBuffer().Upload(brickMap.GetGrid());
  renderer.GetSolidMaskBuffer().Upload(brickMap.GetBricks());
  renderer.GetBrickTextureBuffer().Upload(brickMap.GetBrickTextures());

  int32 windowWidth, windowHeight;
  m_Window.GetSize(windowWidth, windowHeight);


  Input::InputManager &inputManager = Input::InputManager::Get();

  FirstPersonCamera firstPersonCamera(1.0f, 75.0f, windowWidth, windowHeight);
  Camera::SetMainCamera(firstPersonCamera.GetCamera());

  //m_Inspector.AddBool("Show steps");
  //m_Inspector.AddBool("Show normals");

  m_Inspector.AddBool("FXAA");
  m_Inspector.AddFloat("FXAA threshold min", 0.18f, 0.001f);
  m_Inspector.AddFloat("FXAA threshold max", 0.5f, 0.001f);

  m_Inspector.AddInt("BVH Depth");

  float deltaSeconds = 0.0f;
  glfwSwapInterval(1);

  while (m_Window.IsOpen()) {
    start = std::chrono::high_resolution_clock::now();
    m_Window.GetSize(windowWidth, windowHeight);
    inputManager.BeginFrame();
    m_Window.Update();

    if (inputManager.mouse.GetHeld(Input::MouseButton::right))
      firstPersonCamera.Update(deltaSeconds);

    if (inputManager.keyboard.GetPressed(Input::Key::Escape))
      break;

    m_Inspector.AddVec3("Camera direction", firstPersonCamera.GetForward());

    // Render
    //octree.Draw();
    glViewport(0, 0, windowWidth, windowHeight);
    firstPersonCamera.SetProjection(windowWidth, windowHeight);

    renderer.SetDimensions(windowWidth, windowHeight);
    renderer.Render();


    //-----------------
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    ////bvh.Draw(m_Inspector.GetInt("BVH Depth"));

    for (const auto &node: graphicsNodes) {
      node.Draw();
    }

    //octree.Draw();

    Debug::RenderDebug(firstPersonCamera.GetProjView());
    Debug::ClearQueue();

    m_Window.SwapBuffers();

    end = std::chrono::high_resolution_clock::now();
    auto frameDuration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    float64 frameRate = 1000000.0 / frameDuration.count();
    deltaSeconds = 1.0f / frameRate;

    m_Window.SetTitle(m_Title + " - fps: " + std::to_string(frameRate));
  }
}

void
App::Close() {
  m_Window.Close();
}

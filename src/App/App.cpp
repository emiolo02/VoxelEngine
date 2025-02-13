#include "App.hpp"
#include <GL/glew.h>
#include <GL/gl.h>
#include "GLFW/glfw3.h"

#include "glm/ext/vector_int3.hpp"
#include "Input/InputManager.hpp"
#include "Input/Keyboard.hpp"


#include "Render/Vertex/IndexBuffer.hpp"
#include "Render/Vertex/VertexArray.hpp"
#include "Render/Vertex/VertexBuffer.hpp"
#include "Render/Vertex/VertexBufferLayout.hpp"
#include "Render/Texture/Texture.hpp"

#include "Render/Shader/Shader.hpp"
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
  //Shader blitShader("shaders/fullscreen.vert", "shaders/blit.frag");
  Debug::Init();
  Shader gridShader("shaders/rtGrid.comp");
  Shader brickShader("shaders/rtBrickmap.comp");

  Shader fxaaShader("shaders/fxaa.comp");

#define MODEL_PATH "assets/dragon_vrip.ply"

  auto &model = ObjLoader::Get().Load(MODEL_PATH);
  //ImageManager::Get().Save(1, "sponzatest.png");
  OctreeMesh octree(model);
  octree.Subdivide(8);

  ObjLoader::Get().Remove(MODEL_PATH);

  VoxelGrid grid; //= octree.CreateVoxelGrid(0.1f);

  SSBO gridSSBO(grid.GetVoxels().data(), grid.GetVoxels().size() * sizeof(Color), 1);

  BrickMap brickMap = octree.CreateBrickMap(0.1f);
  brickMap.PrintByteSize();

  octree.Clear();

  SSBO coarseSSBO(brickMap.GetGrid().data(), brickMap.GetGrid().size() * sizeof(uint32), 2);
  SSBO fineSSBO(
    brickMap.GetBricks().data(), brickMap.GetBricks().size() * sizeof(BrickMap::Brick), 3);

  int32 windowWidth, windowHeight;
  m_Window.GetSize(windowWidth, windowHeight);


  Input::InputManager &inputManager = Input::InputManager::Get();

  FirstPersonCamera firstPersonCamera(1.0f, 75.0f, windowWidth, windowHeight);
  m_Inspector.AddBool("Use brickmap");
  m_Inspector.AddBool("Show steps");
  m_Inspector.AddBool("Show normals");

  m_Inspector.AddBool("FXAA");
  m_Inspector.AddFloat("FXAA threshold min", 0.18f, 0.001f);
  m_Inspector.AddFloat("FXAA threshold max", 0.5f, 0.001f);


  glfwSwapInterval(1);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  float deltaSeconds = 0.0f;

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
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    Texture renderTexture(windowWidth, windowHeight);
    m_Inspector.AddTexture("Viewport", {renderTexture.GetId(), windowWidth, windowHeight, 0.1f});

    //-----------------

    renderTexture.BindImageTexture();

    if (!m_Inspector.GetBool("Use brickmap")) {
      gridShader.Bind();

      gridSSBO.Bind();

      gridShader.SetValue("u_CameraPosition", firstPersonCamera.GetPosition());
      gridShader.SetValue("u_InvProjection", firstPersonCamera.GetInvProjection());
      gridShader.SetValue("u_InvView", firstPersonCamera.GetInvView());

      const AABB &boundingBox = grid.GetBoundingBox();

      gridShader.SetValue("u_GridMinBounds", boundingBox.GetMinExtents());
      gridShader.SetValue("u_GridMaxBounds", boundingBox.GetMaxExtents());
      gridShader.SetValue("u_VoxelSize", grid.GetVoxelSize());

      const ivec3 &dimensions = grid.GetDimensions();

      gridShader.SetValue("u_GridXSize", dimensions.x);
      gridShader.SetValue("u_GridYSize", dimensions.y);
      gridShader.SetValue("u_GridZSize", dimensions.z);
      gridShader.SetValue("u_Resolution", vec2(windowWidth, windowHeight));

      gridShader.SetValue("u_ShowSteps", m_Inspector.GetBool("Show steps"));
      gridShader.SetValue("u_ShowNormals", m_Inspector.GetBool("Show normals"));
    } else {
      brickShader.Bind();

      coarseSSBO.Bind();
      fineSSBO.Bind();

      brickShader.SetValue("u_CameraPosition", firstPersonCamera.GetPosition());
      brickShader.SetValue("u_InvProjection", firstPersonCamera.GetInvProjection());
      brickShader.SetValue("u_InvView", firstPersonCamera.GetInvView());

      const AABB &boundingBox = brickMap.GetBoundingBox();

      brickShader.SetValue("u_GridMinBounds", boundingBox.GetMinExtents());
      brickShader.SetValue("u_GridMaxBounds", boundingBox.GetMaxExtents());
      brickShader.SetValue("u_VoxelSize", brickMap.GetVoxelSize());

      const ivec3 &dimensions = brickMap.GetDimensions();

      brickShader.SetValue("u_GridXSize", dimensions.x);
      brickShader.SetValue("u_GridYSize", dimensions.y);
      brickShader.SetValue("u_GridZSize", dimensions.z);
      brickShader.SetValue("u_Resolution", vec2(windowWidth, windowHeight));

      brickShader.SetValue("u_ShowSteps", m_Inspector.GetBool("Show steps"));
      brickShader.SetValue("u_ShowNormals", m_Inspector.GetBool("Show normals"));
    }

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glDispatchCompute(std::ceil(float(windowWidth) / 16.0f), std::ceil(float(windowHeight) / 16.0f), 1);

    //------------------------------------------------

    if (m_Inspector.GetBool("FXAA")) {
      fxaaShader.Bind();
      renderTexture.BindImageTexture();

      fxaaShader.SetValue("u_ThresholdMin", m_Inspector.GetFloat("FXAA threshold min"));
      fxaaShader.SetValue("u_ThresholdMax", m_Inspector.GetFloat("FXAA threshold max"));

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      glDispatchCompute(std::ceil(float(windowWidth) / 16.0f), std::ceil(float(windowHeight) / 16.0f), 1);
    }
    //blitShader.Bind();

    //renderTexture.BindTexture();
    //vertexArray.Bind();
    //indexBuffer.Bind();
    //glDrawElements(
    //  GL_TRIANGLES, indexBuffer.GetSize(), GL_UNSIGNED_INT, nullptr);

    //octreeDragon.Draw();

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

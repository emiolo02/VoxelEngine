#include "App.hpp"

#include <glad/glad.h>
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


#define MODEL_PATH "assets/monkey.obj"

    auto &model = ObjLoader::Get().Load(MODEL_PATH);
    //ImageManager::Get().Save(4, "sponzatest.png");
    OctreeMesh octree(model, 6);

    std::vector<GraphicsNode> graphicsNodes;
    graphicsNodes.reserve(model.meshes.size());
    //for (const Mesh &mesh: model.meshes) {
    //  GraphicsNode &node = graphicsNodes.emplace_back(mesh);
    //  node.GetTransform().scale = vec3(0.001f);
    //}
    //ModelBVH bvh(model, 10);

    ObjLoader::Get().Remove(MODEL_PATH);

    BrickMap brickMap = octree.CreateBrickMap(0.1f);
    brickMap.PrintByteSize();
    //brickMap.PrintByteSize();

    //octree.Clear();


    Renderer renderer;
    renderer.SetBrickMap(&brickMap);

    StorageBuffer<uint32> &gridBuffer = renderer.GetBrickGridBuffer();
    StorageBuffer<BrickMap::Brick> &brickBuffer = renderer.GetSolidMaskBuffer();
    StorageBuffer<BrickMap::BrickTexture> &textureBuffer = renderer.GetBrickTextureBuffer();
    gridBuffer.Upload(brickMap.GetGrid());
    brickBuffer.Upload(brickMap.GetBricks());
    textureBuffer.Upload(brickMap.GetBrickTextures());

    int32 windowWidth, windowHeight;
    m_Window.GetSize(windowWidth, windowHeight);


    Input::InputManager &inputManager = Input::InputManager::Get();

    FirstPersonCamera firstPersonCamera(1.0f, 75.0f, windowWidth, windowHeight);
    Camera::SetMainCamera(firstPersonCamera.GetCamera());


    m_Inspector.AddBool("Show steps");
    m_Inspector.AddBool("Show normals");

    m_Inspector.AddButton("Recompile shader", [&renderer] {
        renderer.GetRaytraceShader() = Shader("shaders/rtBrickmap.comp");
    });


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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, windowWidth, windowHeight);
        firstPersonCamera.SetProjection(windowWidth, windowHeight);

        renderer.SetShowSteps(m_Inspector.GetBool("Show steps"));
        renderer.SetShowNormals(m_Inspector.GetBool("Show normals"));

        renderer.SetDimensions(windowWidth, windowHeight);
        renderer.Render();
        static math::Ray ray;
        if (inputManager.mouse.GetPressed(Input::MouseButton::left)) {
            //std::cout << "Sent ray\n";
            ray = {firstPersonCamera.GetPosition(), firstPersonCamera.GetForward()};
            //std::cout << to_string(ray.origin) << '\n' << to_string(ray.direction) << '\n';
            const auto hitVoxel = brickMap.RayCast(ray);
            if (hitVoxel) {
                //std::cout << "Hit pog\n";
                const ivec3 globalPosition = hitVoxel.value();
                const ivec3 localPosition = globalPosition % 8;
                auto hierarchy = brickMap.GetHierarchy(globalPosition);

                BrickMap::BrickTexture &texture = std::get<2>(hierarchy);
                texture.voxels[Flatten(localPosition, ivec3(8))] = math::Color(1.0f, 0.0f, 0.0f, 1.0f);

                textureBuffer.SetData(std::get<1>(hierarchy).colorPointer, texture);

                m_Inspector.AddVec3("hit", vec3(
                                        std::get<0>(hierarchy),
                                        std::get<1>(hierarchy).colorPointer,
                                        Flatten(localPosition, ivec3(8))
                                    ));
            }
        }

        for (const auto &hitGridCell: brickMap.hitGridCells) {
            Debug::DrawBox(hitGridCell.GetCenter(), vec3(), hitGridCell.GetSize(), {1, 0, 0, 1}, 2);
        }

        Debug::DrawLine(ray.origin, ray.direction * 10.0f, {1, 0, 0, 1}, 2);
        const math::BoundingBox &brickBounds = brickMap.GetBoundingBox();
        Debug::DrawBox(brickBounds.GetCenter(), vec3(), brickBounds.GetSize(), {0, 1, 0, 1}, 2);
        //-----------------
        //glEnable(GL_DEPTH_TEST);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto &node: graphicsNodes) {
            node.Draw();
        }


        //Debug::RenderDebug(firstPersonCamera.GetProjView());
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

#include "App.hpp"

#include "GL/glew.h"
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

    auto &model = ObjLoader::Get().Load(m_ModelPath);
    //ImageManager::Get().Save(4, "sponzatest.png");

    std::vector<GraphicsNode> graphicsNodes;
    //graphicsNodes.reserve(model.meshes.size());
    //for (const Mesh &mesh: model.meshes) {
    //    GraphicsNode &node = graphicsNodes.emplace_back(mesh);
    //    node.GetTransform().scale = vec3(0.001f);
    //}

    BrickMap brickMap = Voxelize(model, m_Subdivisions, 0.1f);
    //, math::Color(0xFFFFFFFF)); //octree.CreateBrickMap(0.1f);
    brickMap.PrintByteSize();
    //brickMap.PrintByteSize();
    ObjLoader::Get().Remove(m_ModelPath);

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
    m_Inspector.AddInt("Radius", 1);

    m_Inspector.AddButton("Recompile shader", [&renderer] {
        renderer.GetRaytraceShader() = ShaderManager::Get().Load("shaders/rtBrickmap.comp");
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
        if (inputManager.mouse.GetPressed(Input::MouseButton::left)) {
            const bool actionDelete = inputManager.keyboard.GetHeld(Input::Key::Space);
            //std::cout << "Sent ray\n";
            const math::Ray ray = {firstPersonCamera.GetPosition(), firstPersonCamera.GetForward()};
            //std::cout << to_string(ray.origin) << '\n' << to_string(ray.direction) << '\n';
            const auto hitVoxel = brickMap.RayCast(ray);
            if (hitVoxel) {
                //std::cout << "Hit pog\n";
                auto hitResult = hitVoxel.value();
                const ivec3 insertPosition = hitResult.position + hitResult.normal;

                if (actionDelete) {
                    std::vector<uint32> removedBricks;
                    std::vector<uint32> modifiedBricks;
                    const int radius = m_Inspector.GetInt("Radius");
                    const int radiusSq = radius * radius;
                    for (int z = hitResult.position.z - radius; z < hitResult.position.z + radius; ++z) {
                        for (int y = hitResult.position.y - radius; y < hitResult.position.y + radius; ++y) {
                            for (int x = hitResult.position.x - radius; x < hitResult.position.x + radius; ++x) {
                                const ivec3 distanceVector = hitResult.position - ivec3(x, y, z);
                                const int distanceSq =
                                        distanceVector.x * distanceVector.x +
                                        distanceVector.y * distanceVector.y +
                                        distanceVector.z * distanceVector.z;

                                if (distanceSq < radiusSq) {
                                    if (auto result = brickMap.Delete({x, y, z})) {
                                        if (result->isEmpty) {
                                            removedBricks.push_back(result->cellIndex);
                                        } else {
                                            modifiedBricks.push_back(result->cellIndex);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    for (auto gridCell: removedBricks) {
                        auto &grid = brickMap.GetGrid();
                        auto &bricks = brickMap.GetBricks();
                        auto &brickTextures = brickMap.GetBrickTextures();

                        const uint32 removedBrickIndex = grid[gridCell];
                        if (removedBrickIndex == EMPTY_BRICK) continue;

                        // Set GPU data.
                        gridBuffer.SetData(bricks.back().parent, removedBrickIndex);
                        gridBuffer.SetData(gridCell, EMPTY_BRICK);

                        brickBuffer.SetData(removedBrickIndex, bricks.back());
                        brickBuffer.PopBack();

                        textureBuffer.SetData(removedBrickIndex, brickTextures.back());
                        textureBuffer.PopBack();

                        // Set CPU data.
                        grid[bricks.back().parent] = removedBrickIndex;
                        grid[gridCell] = EMPTY_BRICK;

                        bricks[removedBrickIndex] = bricks.back();
                        bricks.pop_back();

                        brickTextures[removedBrickIndex] = brickTextures.back();
                        brickTextures.pop_back();
                    }
                    for (auto gridCell: modifiedBricks) {
                        const uint32 brickPointer = brickMap.GetGrid()[gridCell];
                        if (brickPointer == EMPTY_BRICK) continue;

                        const BrickMap::Brick &brick = brickMap.GetBricks()[brickPointer];
                        const BrickMap::BrickTexture &brickTexture = brickMap.GetBrickTextures()[brick.
                            colorPointer];

                        brickBuffer.SetData(brickPointer, brick);
                        textureBuffer.SetData(brick.colorPointer, brickTexture);
                    }
                } else {
                    const math::Color color = brickMap.GetVoxel(hitResult.position).value();
                    std::vector<uint32> newBricks;
                    std::vector<uint32> modifiedBricks;
                    const int radius = m_Inspector.GetInt("Radius");
                    const int radiusSq = radius * radius;
                    for (int z = insertPosition.z - radius; z < insertPosition.z + radius; ++z) {
                        for (int y = insertPosition.y - radius; y < insertPosition.y + radius; ++y) {
                            for (int x = insertPosition.x - radius; x < insertPosition.x + radius; ++x) {
                                const ivec3 distanceVector = insertPosition - ivec3(x, y, z);
                                const int distanceSq =
                                        distanceVector.x * distanceVector.x +
                                        distanceVector.y * distanceVector.y +
                                        distanceVector.z * distanceVector.z;

                                if (distanceSq < radiusSq) {
                                    if (auto result = brickMap.Insert({x, y, z}, color, false)) {
                                        if (result->isNew) {
                                            newBricks.push_back(result->cellIndex);
                                        } else {
                                            modifiedBricks.push_back(result->cellIndex);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    for (const auto gridCell: newBricks) {
                        const uint32 brickPointer = brickMap.GetGrid()[gridCell];
                        const BrickMap::Brick &brick = brickMap.GetBricks()[brickPointer];
                        const BrickMap::BrickTexture &brickTexture = brickMap.GetBrickTextures()[brick.colorPointer];

                        gridBuffer.SetData(gridCell, brickPointer);
                        brickBuffer.PushBack(brick);
                        textureBuffer.PushBack(brickTexture);
                    }

                    for (const auto gridCell: modifiedBricks) {
                        const uint32 brickPointer = brickMap.GetGrid()[gridCell];
                        const BrickMap::Brick &brick = brickMap.GetBricks()[brickPointer];
                        const BrickMap::BrickTexture &brickTexture = brickMap.GetBrickTextures()[brick.colorPointer];

                        brickBuffer.SetData(brickPointer, brick);
                        textureBuffer.SetData(brick.colorPointer, brickTexture);
                    }
                }
            }
        }

        // Cursor
        //Debug::DrawBox(firstPersonCamera.GetPosition() + firstPersonCamera.GetForward(), {}, vec3(0.001f), vec4(1.0f),
        //               2);

        //-----------------
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (const auto &node: graphicsNodes) {
            node.Draw();
        }


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
    ShaderManager::Get().Clear();
    m_Window.Close();
}

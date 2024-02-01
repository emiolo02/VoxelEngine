#pragma once
#include <vector>

enum Cameras
{
	CAMERA_MAIN,
	CAMERA_SUN,
	NUM_CAMERAS
};

class Camera
{
public:
	static Camera* GetCamera(Cameras cam);
	static void AddCamera(Camera* camera);

	Camera();
	void Perspective(float fovY, int windowWidth, int windowHeight);
	void Orthographic(float l, float r, float t, float b, float n, float f);
	void FreeFly(vec3 input, float dMouseX, float dMouseY, float deltaSeconds);
	void Orbit(float speed, float radius, float height, float time);


	vec3 up;
	vec3 right;
	vec3 direction = vec3();

	vec3 position = vec3();
	mat4 projection;
	mat4 view;
	mat4 projView;

private:
	static std::vector<Camera*> cameras;

	float dirx = 0;
	float diry = 0;
};
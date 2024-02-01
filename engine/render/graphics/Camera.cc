#include "config.h"
#include "Camera.h"

std::vector<Camera*> Camera::cameras;

Camera* Camera::GetCamera(Cameras cam)
{
	assert(cam < cameras.size());
	return cameras[cam];
}

void 
Camera::AddCamera(Camera* camera)
{
	cameras.push_back(camera);
}

Camera::Camera()
{
	this->view = mat4();
	this->up = vec3(0, 1, 0);
	this->right = vec3(1, 0, 0);
}

void 
Camera::Perspective(float fovY, int windowWidth, int windowHeight)
{
	float fov = fovY * PI / 180.0f;
	projection = glm::perspective(fov, (float)windowWidth / float(windowHeight), 0.1f, 100.0f);
	
	/*float aspect = (float)windowWidth / windowHeight;
	this->projection = orthographic(-5, 5, 5, -5, 0.1, 10);*/
	projView = projection * view;
}

void 
Camera::Orthographic(float l, float r, float t, float b, float n, float f)
{
	projection = glm::ortho(l, r, b, t, n, f);
	projView = projection * view;
}

void
Camera::FreeFly(vec3 input, float dMouseX, float dMouseY, float deltaSeconds)
{
	//this->up = normalize(cross(this->direction, this->right));
	//this->right = normalize(cross(this->up, this->direction));

	this->dirx -= dMouseX * deltaSeconds;
	this->diry -= dMouseY * deltaSeconds;

	// Lock y direction so it doesnt loop around
	if (this->diry > (PI / 2) - 0.1f)
		this->diry = (PI / 2) - 0.1f;
	if (this->diry < -(PI / 2) + 0.1f)
		this->diry = -(PI / 2) + 0.1f;

	this->direction = normalize(vec3(-cos(dirx) * cos(diry), diry, sin(dirx) * cos(diry)));

	//move
	this->position += 
		//forward
		this->direction * input.z * deltaSeconds * 3.0f
		//right
		+ normalize(cross(this->direction, this->up)) * input.x * deltaSeconds * 3.0f
		//up
		+ this->up * input.y * deltaSeconds * 3.0f;

	this->view = glm::lookAt(this->position, this->position + this->direction, this->up);

	projView = projection * view;
}

void
Camera::Orbit(float speed, float height, float radius, float time)
{
	this->position.x = sin(time * speed) * radius;
	this->position.y = height;
	this->position.z = cos(time * speed) * radius;


	this->view = glm::lookAt(this->position, vec3(0, 0, 0), this->up);
}
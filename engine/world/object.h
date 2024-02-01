#pragma once
#include <string>
#include "GL/glew.h"
#include "render/graphics/Model.h"
#include "render/graphics/GraphicsNode.h"

#include "physics/Physics.h"

class Camera;

class Object
{
public:
	Object(std::string modelPath, vec3 position = vec3(), vec3 scale = vec3(1, 1, 1), vec3 rotation = vec3(), float mass = 5.0f, bool isStatic = false);
	~Object() { delete rb; }

	void Update(float dt);

	Physics::RigidBody* GetRigidBody();

	void SetColor(vec3 diffuse);


	uint32 GetID();

	void Draw(Camera camera, GLuint program);

	void SimpleDraw(Camera camera, GLuint program);

	std::string GetName() const&;
	void SetName(std::string const& n);

	GraphicsNode& GetGNode();
	vec3 position;
	vec3 scale;
	vec3 rotation;
	quat orientation;
private:

	static uint32 nextId;
	uint32 id = 0;

	Physics::RigidBody* rb;

	GraphicsNode gNode;
	std::string name;
};
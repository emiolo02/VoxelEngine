#pragma once
#include "render/graphics/Sun.h"
#include "render/graphics/PointLight.h"
#include "input/Input.h"
#include "physics/Physics.h"


class Object;

class World
{
public:
	static World* Instance();

	std::vector<Object*> const& GetObjects();

	void Update(float dt);

	Object* GetObject(uint32 const& id);

	void AddObject(Object* obj);

	Sun& GetSun();

	std::vector<PointLight> pointLights;
private:
	static World* instance;
	World(){}
	~World();

	std::vector<Object*> objects;
	Sun sun;
};

class PhysicsWorld
{
public:
	static PhysicsWorld* Instance();
	
	void Update(float dt);

	void AddRigidBody(Physics::RigidBody* rb);

	void StartGravity();

	std::vector<Physics::RigidBody*>& GetRigidBodies();

	int resolvedCollisions = 0;
private:
	static PhysicsWorld* instance;
	PhysicsWorld() {}

	std::vector<Physics::RigidBody*> rigidBodies;
	std::queue<Physics::CollisionData> collisions;

	bool gravityActive = false;
	float gravityScale = 9.82f;
};
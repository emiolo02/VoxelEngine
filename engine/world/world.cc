#include "config.h"
#include "world.h"
#include "object.h"
#include "physics/gjk.h"

World* World::instance = nullptr;

World*
World::Instance()
{
	if (instance == nullptr)
		instance = new World();

	return instance;
}

std::vector<Object*> const& 
World::GetObjects()
{
	return objects;
}

void World::Update(float dt)
{
	for (auto& obj : objects)
		obj->Update(dt);
}

Object* World::GetObject(uint32 const& id)
{
	return objects[id];
}

Sun&
World::GetSun()
{
	return sun;
}

World::~World()
{
	for (auto& obj : objects)
	{
		for (auto& mesh : obj->GetGNode().model->meshes)
		{
			for (auto& prim : mesh.primitives)
			{
				delete prim.material;
			}
		}
	}
}

void 
World::AddObject(Object* obj)
{
	objects.push_back(obj);
}

PhysicsWorld* PhysicsWorld::instance = nullptr;

PhysicsWorld* PhysicsWorld::Instance()
{
	if (instance == nullptr)
		instance = new PhysicsWorld();

	return instance;
}

void PhysicsWorld::Update(float dt)
{
	counter++;
	/*if (counter % 10 != 0)
		return;*/
	counter = 0;

	Physics::FindCollisions(rigidBodies, collisions);

	while (!collisions.empty())
	{
		Physics::SolveCollision(collisions.front());
		collisions.pop();
	}

	for (auto& rb : rigidBodies)
	{
		rb->Update(dt);
	}
}

void PhysicsWorld::AddRigidBody(Physics::RigidBody* rb)
{
	rigidBodies.push_back(rb);
}

void PhysicsWorld::StartGravity()
{
	gravityActive = true;
	for (auto& rb : rigidBodies)
		rb->ApplyGravity(gravityScale);
}

std::vector<Physics::RigidBody*>& PhysicsWorld::GetRigidBodies()
{
	return rigidBodies;
}
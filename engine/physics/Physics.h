#pragma once
#include "Rigidbody.h"

namespace Physics
{
	void FindCollisions(std::vector<RigidBody*>& bodies, std::queue<CollisionData>& out);
	void SolveCollision(CollisionData const& collision, float separationThreshold = 0.01f);
	
}
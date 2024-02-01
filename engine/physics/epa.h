#include "physics.h"

namespace gjk
{
	struct Simplex;
}

namespace epa
{
	Physics::CollisionData Expand(gjk::Simplex const& simplex, Physics::RigidBody* a, Physics::RigidBody* b);
}
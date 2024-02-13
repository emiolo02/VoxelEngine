#include "config.h"
#include "Physics.h"
#include "gjk.h"
#include "world/object.h"

namespace Physics
{
	
	struct RigidBodyCompare
	{
		uint32 axis = 0;

		bool operator()(RigidBody* a, RigidBody* b)
		{
			float aMin = a->GetCollider().GetCoarse().GetMinExtent()[axis];
			float bMin = b->GetCollider().GetCoarse().GetMinExtent()[axis];

			return aMin < bMin;
		}
	};

	static RigidBodyCompare rbComp;
	static std::vector<std::pair<RigidBody*, RigidBody*>> possibleCollisions;
	
	void FindCollisions(std::vector<RigidBody*>& bodies, std::queue<CollisionData>& out)
	{
		// Debug draw AABBs
		for (auto& rb : bodies)
		{
			Physics::AABB& aabb = rb->GetCollider().GetCoarse();
			vec3 pos = (aabb.GetMinExtent() + aabb.GetMaxExtent()) * 0.5f;
			vec3 extent = (aabb.GetMaxExtent() - pos) * 2.0f;
			Debug::DrawBox(pos, vec3(), extent, aabb.GetDbgColor());
		}

		// Reset debug color
		for (auto& bodies : possibleCollisions)
		{
			bodies.first->GetCollider().GetCoarse().SetDbgColor(vec4(0, 1, 0, 1));
			bodies.second->GetCollider().GetCoarse().SetDbgColor(vec4(0, 1, 0, 1));


			bodies.first->GetParent()->SetColor(vec3(1, 1, 1));
			bodies.second->GetParent()->SetColor(vec3(1, 1, 1));
		}
		possibleCollisions.resize(0);



		// Find most varied axis
		std::sort(bodies.begin(), bodies.end(), rbComp);

		vec3 centerSum;
		vec3 centerSqSum;
		for (int i = 0; i < bodies.size(); i++)
		{
			Physics::AABB& aabbCurrent = bodies[i]->GetCollider().GetCoarse();

			vec3 center = aabbCurrent.GetMaxExtent() - aabbCurrent.GetMinExtent();
			centerSum += center;
			centerSqSum += center * center;

			for (int j = i + 1; j < bodies.size(); j++)
			{
				Physics::AABB& aabbOther = bodies[j]->GetCollider().GetCoarse();

				if (aabbCurrent.GetMaxExtent()[rbComp.axis] < aabbOther.GetMinExtent()[rbComp.axis])
					break;

				if (aabbCurrent.Intersect(aabbOther))
				{
					aabbCurrent.SetDbgColor(vec4(1, 0, 0, 1));
					aabbOther.SetDbgColor(vec4(1, 0, 0, 1));

					std::pair<Physics::RigidBody*, Physics::RigidBody*> pair(bodies[i], bodies[j]);
					possibleCollisions.push_back(pair);
				}
			}
		}

		vec3 variance = centerSqSum - (centerSum * centerSum);
		rbComp.axis = 0;
		if (variance[1] > variance[0])
			rbComp.axis = 1;
		if (variance[2] > variance[rbComp.axis])
			rbComp.axis = 2;

		for (auto& possibleCollision : possibleCollisions)
		{
			auto colData = gjk::Collision(
				possibleCollision.first,
				possibleCollision.second);

			if (colData.hasCollision)
				out.push(colData);
		}
	}

	void SolveCollision(CollisionData const& collision, float separationThreshold)
	{
		Physics::RigidBody* rbA = collision.bodies.first;
		Physics::RigidBody* rbB = collision.bodies.second;

		const vec3 pointA = collision.collisionPoints.first;

		const vec3 radiusVectorA = pointA - rbA->GetPosition();

		const vec3 pointVelocityA =
			rbA->GetVelocity() +
			glm::cross(rbA->GetAngMomentum(), radiusVectorA);

		const float effectiveMassA =
			rbA->GetInvMass() +
			glm::dot(
				glm::cross(
					rbA->GetInertiaTensor() * glm::cross(
						radiusVectorA,
						collision.normal
					),
					radiusVectorA
				),
				collision.normal);


		const vec3 pointB = collision.collisionPoints.second;

		const vec3 radiusVectorB = pointB - rbB->GetPosition();

		const vec3 pointVelocityB =
			rbB->GetVelocity() +
			glm::cross(rbB->GetAngMomentum(), radiusVectorB);

		const float effectiveMassB =
			rbB->GetInvMass() +
			glm::dot(
				glm::cross(
					rbB->GetInertiaTensor() * glm::cross(
						radiusVectorB,
						collision.normal
					),
					radiusVectorB
				),
				collision.normal);


		const float relativeVelocity =
			glm::dot(collision.normal, pointVelocityA - pointVelocityB);
		if (relativeVelocity < separationThreshold)
		{
			// Objects are at rest
			if (relativeVelocity > 0)
			{
				// @TODO - implement proper resting forces
				return;
			}
			rbA->EnablePhysics();
			rbB->EnablePhysics();
			//Objects are moving away from each other
			return;
		}

		const float restitutionCoefficient = 0.2f;
		const float force =
			-(1 + restitutionCoefficient) * relativeVelocity
			/ (effectiveMassA + effectiveMassB);

		Debug::DrawLine(pointA, pointA + collision.normal * force, vec4(0, 1, 0, 1), 5);
		Debug::DrawLine(pointB, pointB - collision.normal * force, vec4(0, 1, 0, 1), 5);

		rbA->Impulse(collision.normal, force, pointA);
		rbB->Impulse(-collision.normal, force, pointB);

		// Linear projection for resolving intersection

		if (collision.intersectionDepth > 999999)
			return; // for some reason intersection depth is infinite sometimes

		const float percent = 0.9f;
		const float slop = 0.01f;

		vec3 correction = collision.normal * percent
			* fmax(collision.intersectionDepth - slop, 0.0f)
			/ (rbA->GetInvMass() + rbB->GetInvMass());
		float corrLength = glm::length(correction);

		rbA->SetPosition(rbA->GetPosition() - correction * rbA->GetInvMass());
		rbB->SetPosition(rbB->GetPosition() + correction * rbB->GetInvMass());
	}

}

#include "config.h"
#include "Rigidbody.h"
#include "world/world.h"
#include "world/object.h"

namespace Physics
{
	RigidBody::RigidBody(Object* parent)
	{
		this->parent = parent;

		position = parent->position;
		orientation = glm::quat(parent->GetGNode().GetRotation());
		collider = Collider(parent);

		CalcInertiaTensor(5);

		PhysicsWorld::Instance()->AddRigidBody(this);
	}

	RigidBody::RigidBody(Object* parent, float mass, bool isStatic)
	{
		this->parent = parent;

		position = parent->position;
		orientation = glm::quat(parent->GetGNode().GetRotation());
		collider = Collider(parent);

		if (isStatic)
			SetInvMass(0);
		else
			SetMass(mass);

		PhysicsWorld::Instance()->AddRigidBody(this);
	}

	void RigidBody::CalcInertiaTensor(float im)
	{
		if (im == 0)
		{
			boxInertia = mat3(0);
			return;
		}


		// Create inertia tensor from AABB
		float mass = 1 / im;

		float invTwelve = 1.0f / 12.0f;
		vec3 ext = collider.GetCoarse().GetMaxExtent() -
			collider.GetCoarse().GetMinExtent();
		ext *= parent->scale;
		boxInertia = glm::inverse(glm::mat3(
			invTwelve * mass * (ext.y * ext.y + ext.z * ext.z), 0, 0,
			0, invTwelve * mass * (ext.x * ext.x + ext.z * ext.z), 0,
			0, 0, invTwelve * mass * (ext.x * ext.x + ext.y * ext.y)
		));
	}

	void RigidBody::Update(float dt)
	{
		if (!physicsEnabled)
			return;

		velocity += acceleration * dt;
		position += velocity * dt;

		collider.position = position;
		collider.scale = parent->scale;

		Debug::DrawLine(position, position + angularMomentum, vec4(0, 1, 0, 1));

		parent->position = position;

		mat3 objectRot = parent->GetGNode().GetRotation();
		worldInertia = objectRot * boxInertia * glm::transpose(objectRot);

		quat spinQuat(0.0f, angularMomentum * dt);
		orientation += spinQuat * orientation * 0.5f;
		orientation = glm::normalize(orientation);

		parent->orientation = orientation;

		parent->rotation = glm::eulerAngles(orientation);
		forceAccum = vec3(0);
	}

	void RigidBody::Impulse(vec3 dir, float force)
	{
		velocity += dir * force * invMass;
		EnablePhysics();
	}

	void RigidBody::Impulse(vec3 dir, float force, vec3 point)
	{
		velocity += dir * force * invMass;

		vec3 torque = glm::cross(point - position, dir * force);
		angularMomentum += worldInertia * torque;
		EnablePhysics();
	}

	void RigidBody::ApplyGravity(float gravityScale)
	{
		if (invMass == 0)
			return;

		acceleration.y = -gravityScale;
		EnablePhysics();
	}
}
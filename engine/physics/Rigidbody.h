#pragma once
#include "Collider.h"

class Object;
class Inspector;

namespace Physics
{

class RigidBody
{
public:
	RigidBody() {}
	RigidBody(Object* parent);
	RigidBody(Object* parent, float mass, bool isStatic = false);

	void CalcInertiaTensor(float mass);

	void Update(float dt);

	void Impulse(vec3 dir, float force);
	void Impulse(vec3 dir, float force, vec3 point);

	void ApplyGravity(float gravityScale);

	Collider& GetCollider() { return collider; }

	vec3 GetPosition() { return position; }
	void SetPosition(vec3 const& pos) { position = pos; }

	vec3 GetAcceleration() { return acceleration; }

	vec3 GetVelocity() { return velocity; }

	vec3 GetAngMomentum() { return angularMomentum; }
	void SetAngMomentum(vec3 const& ang) { angularMomentum = ang; }

	mat3 GetInertiaTensor() { return worldInertia; }

	quat GetOrientation() { return orientation; }
	void SetOrientation(quat const& q) { orientation = q; }

	float GetInvMass() { return invMass; }
	float GetMass()
	{
		if (invMass != 0)
			return 1.0f / invMass;
		else
			return 0;
	}

	void EnablePhysics() { physicsEnabled = true; }
	void DisablePhysics() { physicsEnabled = false; }


	void SetInvMass(float m)
	{
		invMass = m;
		CalcInertiaTensor(invMass);
	}
	void SetMass(float m)
	{
		invMass = 1.0f / m;
		CalcInertiaTensor(invMass);
	}


	Object* GetParent()
	{
		return parent;
	}

private:
	float invMass = 0.2f;
	Collider collider;

	Object* parent = nullptr;


	bool physicsEnabled = true;
	//vec3 spin = vec3();

	vec3 forceAccum = vec3();

	vec3 acceleration = vec3();
	vec3 velocity = vec3();
	vec3 position = vec3();

	vec3 angularMomentum = vec3();
	quat orientation = quat(vec3(0, 0, 0));

	mat3 boxInertia;
	mat3 worldInertia;

	friend class ::Inspector;
};

}

#pragma once
#include "render/Debug.h"
#include "render/graphics/Model.h"

class Object;

namespace Physics
{
	class RigidBody;

	struct CollisionData
	{
		bool hasCollision = false;

		float intersectionDepth = 0.0f;
		vec3 normal = vec3();

		std::pair<RigidBody*, RigidBody*> bodies;
		std::pair<vec3, vec3> collisionPoints;
	};

	struct Plane
	{
		Plane() {}
		Plane(vec3 center, vec3 normal);
		Plane(vec3 nor, float dist);
		vec3 normal = vec3(0, 1, 0);
		float distance = 0;

		bool operator==(Plane const& other);
	};


	struct HitResult
	{
		bool hit = false;
		vec3 position;
		Object* object = nullptr;

		HitResult() {}
		HitResult(Object* hit, vec3 point) :
			object(hit), position(point), hit(true)
		{}
	};

	class Ray;

	class AABB
	{
	public:
		AABB()
		{}

		AABB(vec3 const& min, vec3 const& max) :
			initialMinExtent(min),	initialMaxExtent(max),
			minExtent(min),			maxExtent(max)
		{}

		AABB(Model const& model);

		void UpdateBounds(mat4 const& transform);

		bool Intersect(AABB const& other);

		bool Intersect(Ray const& ray, float& tNear, float& tFar);

		vec3 GetMinExtent() const
		{
			return minExtent;
		}
		vec3 GetMaxExtent() const
		{
			return maxExtent;
		}

		vec3 GetInitialMinExtent() const
		{
			return initialMinExtent;
		}

		vec3 GetInitialMaxExtent() const
		{
			return initialMaxExtent;
		}

		void SetMinExtent(vec3 const& min)
		{
			minExtent = min;
		}

		void SetMaxExtent(vec3 const& max)
		{
			maxExtent = max;
		}

		vec4 GetDbgColor()
		{
			return dbgColor;
		}

		void SetDbgColor(vec4 const& c)
		{
			dbgColor = c;
		}

	private:
		vec4 dbgColor;

		vec3 minExtent;
		vec3 maxExtent;

		vec3 initialMinExtent;
		vec3 initialMaxExtent;
	};

	class Collider
	{
	public:
		Collider() {}
		Collider(Object* parent);

		struct MeshCollider
		{
			struct Triangle
			{
				vec3 vertices[3];
				vec3 normal;
			};
			std::vector<Triangle> tris;
			std::vector<vec3> vertices;
		};

		AABB& GetCoarse()
		{
			return coarseCollider;
		}

		MeshCollider& GetMeshCollider()
		{
			return meshCollider;
		}

		const MeshCollider& GetMeshCollider() const
		{
			return meshCollider;
		}

		Object* GetParent()
		{
			return parent;
		}

		const mat4 GetRotation() const;
		vec3 position;
		vec3 scale;

	private:
		void GenerateMeshCollider(Model const& model);
		//bool OverlapOnAxis(vec3 const& axis, MeshCollider::Triangle const& tri1, MeshCollider::Triangle const& tri2);
		//std::pair<float, float> ProjectTriangleOntoAxis(vec3 const& axis, MeshCollider::Triangle const& tri);
		MeshCollider meshCollider;
		AABB coarseCollider;

		Object* parent = nullptr;
	};

	class Ray
	{
	public:
		vec3 origin;
		vec3 dir;
		float distance = 0;

		Ray(vec3 origin, vec3 dir) :
			origin(origin), dir(dir)
		{}

		HitResult Intersect(Collider col);
	};

	std::vector<HitResult> RayCast(vec3 origin, vec3 dir);

	/// returns ray direction in world space
	vec3 CastFromCam(float64 nx, float64 ny, Camera cam);
	/// returns ray direction in world space
	vec3 CastFromCam(float64 nx, float64 ny, mat4 const& invProj, mat4 const& invView);
}
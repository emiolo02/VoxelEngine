#include "config.h"
#include "Collider.h"
#include "world/world.h"
#include "world/object.h"
#include "Rigidbody.h"

namespace Physics
{
	Plane::Plane(vec3 center, vec3 normal)
	{
		this->normal = normal;
		this->distance = -(dot(center, normal));
	}

	Plane::Plane(vec3 nor, float dist)
	{
		this->normal = nor;
		this->distance = dist;
	}

	bool Plane::operator==(Plane const& other)
	{
		return (normal == other.normal && distance == other.distance);
	}

	vec3 CastFromCam(float64 nx, float64 ny, Camera cam)
	{
		vec4 rayClip4(nx, -ny, -1, 1);
		vec4 rayEye4 = inverse(cam.projection) * rayClip4;
		rayEye4 = vec4(rayEye4.x, rayEye4.y, -1, 0);

		vec4 rayWorld4 = inverse(cam.view) * rayEye4;

		vec3 rayWorld(rayWorld4.x, rayWorld4.y, rayWorld4.z);
		rayWorld = normalize(rayWorld);

		return rayWorld;
	}

	vec3 CastFromCam(float64 nx, float64 ny, mat4 const& invProj, mat4 const& invView)
	{
		vec4 rayClip4(nx, -ny, -1, 1);
		vec4 rayEye4 = invProj * rayClip4;
		rayEye4 = vec4(rayEye4.x, rayEye4.y, -1, 0);

		vec4 rayWorld4 = invView * rayEye4;

		vec3 rayWorld(rayWorld4.x, rayWorld4.y, rayWorld4.z);
		rayWorld = normalize(rayWorld);

		return rayWorld;
	}

	std::vector<HitResult> RayCast(vec3 origin, vec3 dir)
	{
		Ray ray(origin, dir);

		std::vector<HitResult> hit;
		for (auto& rb : PhysicsWorld::Instance()->GetRigidBodies())
		{
			HitResult hitRes = ray.Intersect(rb->GetCollider());
			if (hitRes.hit)
				hit.push_back(hitRes);
		}

		return hit;
	}

	AABB::AABB(Model const& model)
	{
		float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;

		for (auto& mesh : model.meshes)
		{
			for (auto& prim : mesh.primitives)
			{
				for (auto& vert : prim.vertices)
				{
					minX = fmin(minX, vert.position.x);
					maxX = fmax(maxX, vert.position.x);
					minY = fmin(minY, vert.position.y);
					maxY = fmax(maxY, vert.position.y);
					minZ = fmin(minZ, vert.position.z);
					maxZ = fmax(maxZ, vert.position.z);
				}
			}
		}

		initialMinExtent = vec3(minX, minY, minZ);
		initialMaxExtent = vec3(maxX, maxY, maxZ);

		minExtent = initialMinExtent;
		maxExtent = initialMaxExtent;
	}

	void AABB::UpdateBounds(mat4 const& transform)
	{
		// Algorithm shamelessly stolen from Christer Ericson
		// Real time collision detection

		for (int i = 0; i < 3; i++)
		{
			minExtent[i] = maxExtent[i] = transform[3][i];

			for (int j = 0; j < 3; j++)
			{
				float e = transform[j][i] * initialMinExtent[j];
				float f = transform[j][i] * initialMaxExtent[j];
				if (e < f)
				{
					minExtent[i] += e;
					maxExtent[i] += f;
				}
				else
				{
					minExtent[i] += f;
					maxExtent[i] += e;
				}
			}
		}
	}

	bool AABB::Intersect(AABB const& other)
	{
		vec3 d1 = other.GetMinExtent() - maxExtent;
		vec3 d2 = minExtent - other.GetMaxExtent();
		vec3 d = max(d1, d2);

		float maxDist = d[0];
		for (int i = 1; i < 3; i++)
		{
			if (maxDist < d[i])
				maxDist = d[i];
		}

		return maxDist < 0;
	}

	bool AABB::Intersect(Ray const& ray, float& tNear, float& tFar)
	{
		const vec3 tMin = (minExtent - ray.origin) / ray.dir;
		const vec3 tMax = (maxExtent - ray.origin) / ray.dir;
		const vec3 t1 = min(tMin, tMax);
		const vec3 t2 = max(tMin, tMax);
		tNear = std::max(std::max(t1.x, t1.y), t1.z);
		tFar = std::min(std::min(t2.x, t2.y), t2.z);

		return tNear < tFar && tFar > 0;
	}

	Collider::Collider(Object* parent)
	{
		GenerateMeshCollider(*parent->GetGNode().model);
		coarseCollider = AABB(*parent->GetGNode().model);
		this->parent = parent;
	}

	const mat4 Collider::GetRotation() const
	{
		return parent->GetGNode().GetRotation();
	}

	void
		Collider::GenerateMeshCollider(Model const& model)
	{
		for (auto& mesh : model.meshes)
		{
			for (auto& prim : mesh.primitives)
			{
				for (int i = 0; i < prim.indices.size(); i += 3)
				{
					MeshCollider::Triangle tri;
					tri.vertices[0] = prim.vertices[prim.indices[i]].position;
					tri.vertices[1] = prim.vertices[prim.indices[i + 1]].position;
					tri.vertices[2] = prim.vertices[prim.indices[i + 2]].position;
					tri.normal = prim.vertices[prim.indices[i]].normal;
					meshCollider.tris.push_back(tri);

				}

				for (auto& vert : prim.vertices)
				{
					bool isUnique = true;
					for (auto& meshVert : meshCollider.vertices)
					{
						if (meshVert == vert.position)
						{
							isUnique = false;
							break;
						}
					}

					if (!isUnique)
						continue;

					meshCollider.vertices.push_back(vert.position);
				}
			}
		}
	}

	HitResult
		Ray::Intersect(Collider col)
	{
		// Check against bounding box first
		AABB aabb = col.GetCoarse();
		vec3 tMin = (aabb.GetMinExtent() - origin) / dir;
		vec3 tMax = (aabb.GetMaxExtent() - origin) / dir;
		vec3 t1 = min(tMin, tMax);
		vec3 t2 = max(tMin, tMax);
		float tNear = fmax(fmax(t1.x, t1.y), t1.z);
		float tFar = fmin(fmin(t2.x, t2.y), t2.z);

		// No collision
		if (tNear > tFar)
			return HitResult();

		// Check against mesh
		mat4 invTransform = glm::inverse(col.GetParent()->GetGNode().GetTransform());
		vec3 invOrigin = invTransform * vec4(origin, 1.0f);
		vec3 invDir = invTransform * vec4(dir, 0.0f);

		std::vector<float> hitDistances;
		for (auto& tri : col.GetMeshCollider().tris)
		{
			Plane plane(tri.vertices[0], tri.normal);

			float denom = glm::dot(plane.normal, invDir);
			float dividend = -(glm::dot(plane.normal, invOrigin) + plane.distance);
			float t = dividend / denom;

			// Skip if plane was not hit
			if (t < 0.0f)
				continue;

			vec3 intersection = invOrigin + invDir * t;

			// Check if intersection is inside triangle bounds
			vec3 a = tri.vertices[0] - intersection;
			vec3 b = tri.vertices[1] - intersection;
			vec3 c = tri.vertices[2] - intersection;

			vec3 u = glm::cross(b, c);
			vec3 v = glm::cross(c, a);
			vec3 w = glm::cross(a, b);

			// Not in bounds
			if (glm::dot(u, v) < 0.0f || glm::dot(u, w) < 0.0f)
				continue;

			// In bounds
			hitDistances.push_back(t);
		}

		// No intersection with mesh
		if (hitDistances.empty())
			return HitResult();

		float smallest = hitDistances[0];
		for (int i = 0; i < hitDistances.size(); i++)
		{
			if (hitDistances[i] < smallest)
				smallest = hitDistances[i];
		}

		return HitResult(col.GetParent(), origin + dir * smallest);
	}
}
#include "config.h"
#include "gjk.h"
#include "epa.h"

using namespace Physics;

namespace gjk
{
	vec3
	GetFurthestPoint(Collider const& col, vec3 const& dir)
	{
		float maxDot = -FLT_MAX;
		glm::vec3 furthest = vec3();
		for (const auto& vertex : col.GetMeshCollider().vertices)
		{
			float dotProduct = glm::dot(vertex, dir);

			if (dotProduct > maxDot)
			{
				maxDot = dotProduct;
				furthest = vertex;
			}
		}

		return furthest;
	}

	SupportPoint
	Support(Collider const& colA, Collider const& colB, const vec3& dir)
	{
		const mat3 rotA = colA.GetRotation();
		const mat3 rotB = colB.GetRotation();

		// Compute points in local space
		vec3 pointA = GetFurthestPoint(colA, glm::transpose(rotA) *  dir);
		vec3 pointB = GetFurthestPoint(colB, glm::transpose(rotB) * -dir);
		
		// Return world space points
		return	SupportPoint(
			rotA * pointA * colA.scale + colA.position,
			rotB * pointB * colB.scale + colB.position
		);
	}

	bool 
	Line(Simplex& simplex, vec3& dir)
	{
		SupportPoint a = simplex[0];
		SupportPoint b = simplex[1];

		vec3 ab = b.difference - a.difference;
		vec3 ao = -a.difference;

		if (SameDirection(ab, ao)) 
			dir = cross(cross(ab, ao), ab);

		else 
		{
			simplex = { a };
			dir = ao;
		}

		return false;
	}

	bool 
	Triangle(Simplex& simplex, vec3& dir)
	{
		SupportPoint a = simplex[0];
		SupportPoint b = simplex[1];
		SupportPoint c = simplex[2];

		vec3 ab = b.difference - a.difference;
		vec3 ac = c.difference - a.difference;
		vec3 ao = -a.difference;

		vec3 abc = cross(ab, ac);

		if (SameDirection(cross(abc, ac), ao)) 
		{
			if (SameDirection(ac, ao)) 
			{
				simplex = { a, c };
				dir = cross(cross(ac, ao), ac);
			}

			else
				return Line(simplex = { a, b }, dir);
			
		}

		else 
		{
			if (SameDirection(cross(ab, abc), ao))
				return Line(simplex = { a, b }, dir);

			else 
			{
				if (SameDirection(abc, ao)) 
					dir = abc;

				else 
				{
					simplex = { a, c, b };
					dir = -abc;
				}
			}
		}

		return false;
	}

	bool 
	Tetrahedron(Simplex& simplex, vec3& dir)
	{
		SupportPoint a = simplex[0];
		SupportPoint b = simplex[1];
		SupportPoint c = simplex[2];
		SupportPoint d = simplex[3];

		vec3 ab = b.difference - a.difference;
		vec3 ac = c.difference - a.difference;
		vec3 ad = d.difference - a.difference;
		vec3 ao = -a.difference;

		vec3 abc = cross(ab, ac);
		vec3 acd = cross(ac, ad);
		vec3 adb = cross(ad, ab);

		if (SameDirection(abc, ao))
			return Triangle(simplex = { a, b, c }, dir);

		if (SameDirection(acd, ao))
			return Triangle(simplex = { a, c, d }, dir);

		if (SameDirection(adb, ao))
			return Triangle(simplex = { a, d, b }, dir);

		return true;
	}

	bool
	NextSimplex(Simplex& simplex, vec3& dir)
	{
		switch (simplex.size())
		{
		case 2: return Line(simplex, dir);
		case 3: return Triangle(simplex, dir);
		case 4: return Tetrahedron(simplex, dir);
		}

		// Something went very wrong
		assert(false);
	}

	CollisionData 
	Collision(Physics::RigidBody* a, Physics::RigidBody* b)
	{
		Collider& colA = a->GetCollider();
		Collider& colB = b->GetCollider();

		Simplex simplex;

		// Choose a start direction
		vec3 dir = vec3(1.0f, 0.0f, 0.0f); 
		SupportPoint support = Support(colA, colB, dir);
		simplex.push_front(support); 

		// Next direction is towards center 
		dir = -support.difference;

		while (true)
		{
			support = Support(colA, colB, dir);

			if (!SameDirection(support.difference, dir))
				return CollisionData(); // No collision

			simplex.push_front(support);

			if (NextSimplex(simplex, dir))
				return epa::Expand(simplex, a, b);
		}
	}

}

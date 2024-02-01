#include "config.h"
#include "epa.h"
#include "gjk.h"

using namespace Physics;
using namespace gjk;

namespace epa
{
	struct Face
	{
		union
		{
			struct
			{
				SupportPoint vertices[3];
				SupportPoint a, b, c;
			};
		};

		Face() {}

		// CCW winding order
		Face(SupportPoint const& A, SupportPoint const& B, SupportPoint const& C) :
			a(A), b(B), c(C)
		{}
	};

	std::vector<Face> CreateTetrahedron(Simplex& simplex)
	{
		std::vector<Face> ret(4);
		ret[0] = Face(simplex[0], simplex[1], simplex[2]);
		ret[1] = Face(simplex[0], simplex[3], simplex[1]);
		ret[2] = Face(simplex[1], simplex[3], simplex[2]);
		
		return ret;
	}

	std::pair<std::vector<vec4>, uint32> 
	GetFaceNormals(std::vector<SupportPoint> const& polytope, std::vector<uint32> const& faces)
	{
		std::vector<vec4> normals;
		uint32 minTriangle = 0;
		float  minDistance = FLT_MAX;

		for (int i = 0; i < faces.size(); i += 3) {
			vec3 a = polytope[faces[i]].difference;
			vec3 b = polytope[faces[i + 1]].difference;
			vec3 c = polytope[faces[i + 2]].difference;

			vec3 normal = glm::normalize(cross(b - a, c - a));
			float distance = dot(normal, a);

			if (distance < 0) {
				normal *= -1;
				distance *= -1;
			}

			normals.emplace_back(normal, distance);

			if (distance < minDistance) {
				minTriangle = i / 3;
				minDistance = distance;
			}
		}

		return { normals, minTriangle };
	}

	void 
	AddIfUniqueEdge(
	std::vector<std::pair<uint32, uint32>>& edges,
	const std::vector<uint32>& faces,
	uint32 a,
	uint32 b)
	{
		auto reverse = std::find(                       //      0--<--3
			edges.begin(),                              //     / \ B /   A: 2-0
			edges.end(),                                //    / A \ /    B: 0-2
			std::make_pair(faces[b], faces[a]) //   1-->--2
		);

		if (reverse != edges.end()) {
			edges.erase(reverse);
		}

		else {
			edges.emplace_back(faces[a], faces[b]);
		}
	}

	vec3
	Cartesian(vec3 const& b, vec3 const& p0, vec3 const& p1, vec3 const& p2)
	{
		return p0 * b.x + p1 * b.y + p2 * b.z;
	}

	vec3
	Barycentric(vec3 p, vec3 a, vec3 b, vec3 c)
	{
		vec3 v0 = b - a, v1 = c - a, v2 = p - a;
		float d00 = glm::dot(v0, v0);
		float d01 = glm::dot(v0, v1);
		float d11 = glm::dot(v1, v1);
		float d20 = glm::dot(v2, v0);
		float d21 = glm::dot(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;

		return vec3(u, v, w);
	}

	vec3 
	ProjectToTriangle(vec3 const& p, vec3 const& a, vec3 const& b, vec3 const& c)
	{
		vec3 n, q, r, t;
		q = b - a;
		r = c - a;
		n = glm::cross(q, r);
		
		q = a - p;
		r = b - p;
		const float wc = glm::dot(n, glm::cross(q, r));

		t = c - p;
		const float wa = glm::dot(n, glm::cross(r, t));

		const float wb = glm::dot(n, glm::cross(t, q));

		const float denom = wa + wb + wc;

		return vec3(wa / denom, wb / denom, wc / denom);
	}

	std::pair<vec3, vec3> 
	CalcCollisionPoint(const SupportPoint face[])
	{

		vec3 closestBary = ProjectToTriangle(
			vec3(),
			face[0].difference,
			face[1].difference,
			face[2].difference);

		vec3 colPointA = Cartesian(
			closestBary,
			face[0].supportA,
			face[1].supportA,
			face[2].supportA);

		vec3 colPointB = Cartesian(
			closestBary,
			face[0].supportB,
			face[1].supportB,
			face[2].supportB);
	
		return { colPointA, colPointB };
	}

    CollisionData 
	Expand(Simplex const& simplex, RigidBody* a, RigidBody* b)
	{
		assert(simplex.size() == 4);

		Collider& colA = a->GetCollider();
		Collider& colB = b->GetCollider();

		std::vector<SupportPoint> polytope(simplex.begin(), simplex.end());
		std::vector<uint32> faces = {
			0, 1, 2,
			0, 3, 1,
			0, 2, 3,
			1, 3, 2
		};

		auto faceNormals = GetFaceNormals(polytope, faces);
		std::vector<vec4> normals = faceNormals.first;
		uint32 minFace = faceNormals.second;

		vec3  minNormal;
		float minDistance = FLT_MAX;


		uint32 maxIterations = 50;
		while (minDistance == FLT_MAX && maxIterations--)
		{
			minNormal = vec3(normals[minFace]);
			minDistance = normals[minFace].w;

			SupportPoint support = Support(colA, colB, minNormal);
			float sDistance = dot(minNormal, support.difference);

			if (abs(sDistance - minDistance) > 0.001f) 
			{
				minDistance = FLT_MAX;

				std::vector<std::pair<uint32, uint32>> uniqueEdges;

				for (int i = 0; i < normals.size(); i++)
				{
					if (glm::dot(vec3(normals[i]), support.difference) > 0)
					{
						int f = i * 3;

						AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
						AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
						AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);

						faces[f + 2] = faces.back(); faces.pop_back();
						faces[f + 1] = faces.back(); faces.pop_back();
						faces[f] = faces.back(); faces.pop_back();

						normals[i] = normals.back(); // pop-erase
						normals.pop_back();

						i--;
					}
				}

				if (uniqueEdges.size() == 0)
					return CollisionData();

				std::vector<uint32> newFaces;
				for (auto& edge : uniqueEdges) 
				{
					newFaces.push_back(edge.first);
					newFaces.push_back(edge.second);
					newFaces.push_back(polytope.size());
				}

				polytope.push_back(support);

				auto newFaceNormals = GetFaceNormals(polytope, newFaces);
				std::vector<vec4> newNormals = newFaceNormals.first;
				uint32 newMinFace = newFaceNormals.second;

				float oldMinDistance = FLT_MAX;
				for (int i = 0; i < normals.size(); i++) {
					if (normals[i].w < oldMinDistance) {
						oldMinDistance = normals[i].w;
						minFace = i;
					}
				}

				if (newNormals[newMinFace].w < oldMinDistance) {
					minFace = newMinFace + normals.size();
				}

				faces.insert(faces.end(), newFaces.begin(), newFaces.end());
				normals.insert(normals.end(), newNormals.begin(), newNormals.end());
			}
		}

		const SupportPoint closestFace[] =
		{
			polytope[faces[minFace * 3]],
			polytope[faces[minFace * 3 + 2]],
			polytope[faces[minFace * 3 + 1]]
		};
		
		std::pair<vec3,vec3> colPoints = CalcCollisionPoint(closestFace);

		Debug::DrawBox(colPoints.first, vec3(), vec3(0.1, 0.1, 0.1), vec4(1, 0, 0, 1), 2);
		Debug::DrawBox(colPoints.second, vec3(), vec3(0.1, 0.1, 0.1), vec4(0, 0, 1, 1), 2);
		

		CollisionData ret;
		ret.hasCollision = true;
		ret.bodies = std::pair<RigidBody*, RigidBody*>(a, b);
		ret.collisionPoints = colPoints;
		ret.intersectionDepth = minDistance + 0.001f;
		ret.normal = glm::normalize(minNormal);

		return ret;
	}
}
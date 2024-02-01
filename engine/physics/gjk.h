#pragma once
#include "Physics.h"
#include <array>

namespace gjk
{
	struct SupportPoint
	{
		vec3 supportA = vec3();
		vec3 supportB = vec3();
		vec3 difference = vec3();

		SupportPoint(vec3 A, vec3 B) :
			supportA(A), supportB(B), difference(A - B)
		{}

		SupportPoint() {}
	};

	struct Simplex 
	{
	public:
		Simplex()
			: m_size(0)
		{}

		Simplex& operator=(std::initializer_list<SupportPoint> list)
		{
			m_size = 0;

			for (auto point : list)
				m_points[m_size++] = point;

			return *this;
		}

		void push_front(SupportPoint point)
		{
			m_points = { point, m_points[0], m_points[1], m_points[2] };
			m_size = std::min(m_size + 1, 4);
		}

		SupportPoint& operator[](int i) { return m_points[i]; }

		size_t size() const { return m_size; }

		auto begin() const { return m_points.begin(); }
		auto end() const { return m_points.end() - (4 - m_size); }
	private:
		std::array<SupportPoint, 4> m_points;
		int m_size;
	};

	SupportPoint Support(Physics::Collider const& colA, Physics::Collider const& colB, const vec3& dir);
	Physics::CollisionData Collision(Physics::RigidBody* a, Physics::RigidBody* b);
}
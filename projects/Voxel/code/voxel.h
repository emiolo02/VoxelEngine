#pragma once
#include <vector>
#include "random.h"
#include "physics/Physics.h"


struct Color
{
	union
	{
		struct
		{
			uint8 r, g, b;
		};
		uint8 v[3] = { 0 };
	};
	Color(){}
	Color(uint8 r, uint8 g, uint8 b) :
		r(r), g(g), b(b)
	{}

	Color operator+(Color const& rhs)
	{
		r += rhs.r, g += rhs.g, b += rhs.b;
		return *this;
	}

	vec3 ToVec()
	{
		vec3 ret;
		ret.r = float(r) / float(0xFF);
		ret.g = float(g) / float(0xFF);
		ret.b = float(b) / float(0xFF);

		return ret;
	}

	int ToInt()
	{
		int ret = 0;
		for (int i = 0; i < 3; ++i)
		{
			ret += v[i];
			ret <<= 8;
		}
		
		return ret;
	}
};

struct VoxelData
{
	Color color;
	bool occupied = false;

	VoxelData() 
	{}

	VoxelData(Color c) :
		color(c), occupied(true)
	{}

	VoxelData(bool genVoxel)
	{
		if (genVoxel)
		{
			color.r = FastRandom() >> 24;
			color.g = FastRandom() >> 24;
			color.b = FastRandom() >> 24;
			occupied = true;
		}
	}
};

class VoxelGrid
{
public:
	VoxelGrid(int width, int height, int depth, float voxelSize = 0.1f, vec3 worldLocation = vec3()) :
		m_width(width), m_height(height), m_depth(depth), m_voxelSize(voxelSize)
	{
		m_voxels.resize(m_width * m_height * m_depth);
		boundingBox = Physics::AABB(worldLocation, worldLocation + vec3(m_width, m_height, m_depth) * m_voxelSize);
	}

	void GenerateProcedural();

	~VoxelGrid()
	{
		/*for (auto& voxel : m_voxels)
		{
			if (voxel)
				delete voxel;
		}*/
	}

	void Upload(GLuint program);

	inline VoxelData VoxelAt(int x, int y, int z) const
	{
		assert(x + m_width * (y + m_depth * z) < m_voxels.size());
		return m_voxels[x + m_width * (y + m_depth * z)];
	}
	
	inline VoxelData VoxelAt(vec3i const& v) const
	{
		assert(v.x + m_width * (v.y + m_depth * v.z) < m_voxels.size());
		return m_voxels[v.x + m_width * (v.y + m_depth * v.z)];
	}

	Color RayTraverse(Physics::Ray const& ray);

	std::vector<vec3i> GetRayPath(Physics::Ray const& ray);
	
	void FillPath(std::vector<vec3i> path);

private:

	inline bool SampleOutOfBounds(vec3i const& v)
	{
		return (v.x < 0 || v.y < 0 || v.z < 0 ||
			v.x >= m_width || v.y >= m_height || v.z >= m_depth);
	}

	// Stores voxels as colors in a grid, essentially a 3D bitmap image
	std::vector<VoxelData> m_voxels;

	int m_width = 0, m_height = 0, m_depth = 0;
	float m_voxelSize = 0.0f;

	Physics::AABB boundingBox;

	GLuint computeProgram = 0;
	GLuint ssbo = 0;
};

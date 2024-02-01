#include "config.h"
#include "voxel.h"
#include "PerlinNoise.h"
#include <algorithm>

#define MAX(a,b) ((a > b ? a : b))

inline const int MapRange(float inValue, float minInRange, float maxInRange, int minOutRange, int maxOutRange)
{
	float x = (inValue - minInRange) / (maxInRange - minInRange);
	return minOutRange + (maxOutRange - minOutRange) * x;
}

void 
VoxelGrid::GenerateProcedural()
{
	const siv::PerlinNoise perlin;
	const float threshold = 0.4f;
	for (size_t z = 0; z < m_depth; ++z)
	{
		for (size_t y = 0; y < m_height; ++y)
		{
			for (size_t x = 0; x < m_width; ++x)
			{
				auto& voxel = m_voxels[x + m_width * (y + m_depth * z)];
				if (perlin.noise3D((float)x / m_width * 5, (float)y / m_height * 5, (float)z / m_depth * 5) > threshold)
					voxel = VoxelData(true);
			}
		}
	}
}

void VoxelGrid::Upload(GLuint program)
{
	UniformVec3("Grid.minExtent", boundingBox.GetMinExtent(), program);
	UniformVec3("Grid.maxExtent", boundingBox.GetMaxExtent(), program);

	UniformFloat("Grid.voxelSize", m_voxelSize, program);

	UniformInt("Grid.width", m_width, program);
	UniformInt("Grid.height", m_height, program);
	UniformInt("Grid.depth", m_depth, program);

	//glUniform4fv()
	std::vector<vec4> temp(m_voxels.size());
	for (int i = 0; i < m_voxels.size(); i++)
	{
		if (m_voxels[i].occupied)
			temp[i] = vec4(m_voxels[i].color.ToVec(), 1.0f);
		else
			temp[i] = vec4(0.0f);
	}
	//glUniform4fv(glGetUniformLocation(program, "voxels"), temp.size(), &temp[0][0]);

	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, temp.size() * sizeof(vec4), temp.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	computeProgram = program;
}

Color VoxelGrid::RayTraverse(Physics::Ray const& ray)
{
	float tNear = 0.0f, tFar = 0.0f;

	if (!boundingBox.Intersect(ray, tNear, tFar))
		return Color(); // No collision with AABB

	tNear = std::max(tNear + 0.001f, 0.0f);
	tFar = std::max(tFar - 0.001f, 0.0f);

	const vec3 gridOrigin = boundingBox.GetMinExtent();

	const vec3 rayStart = ray.origin + ray.dir * tNear - gridOrigin;
	const vec3 rayEnd = ray.origin + ray.dir * tFar - gridOrigin;

	const vec3i startVoxel(
		rayStart.x / m_voxelSize,
		rayStart.y / m_voxelSize,
		rayStart.z / m_voxelSize);
	const vec3i endVoxel(
		rayEnd.x / m_voxelSize,
		rayEnd.y / m_voxelSize,
		rayEnd.z / m_voxelSize);

	vec3i currVoxel = startVoxel;

	// Traversal direction
	const int stepX = (ray.dir.x >= 0) ? 1 : -1;
	const int stepY = (ray.dir.y >= 0) ? 1 : -1;
	const int stepZ = (ray.dir.z >= 0) ? 1 : -1;

	// The next voxel's relative location
	const float nextVoxelX = (currVoxel.x + std::max(0, stepX)) * m_voxelSize;
	const float nextVoxelY = (currVoxel.y + std::max(0, stepY)) * m_voxelSize;
	const float nextVoxelZ = (currVoxel.z + std::max(0, stepZ)) * m_voxelSize;	

	// The distance along the ray to intersect the next cell for each axis
	float tMaxX = (ray.dir.x != 0) ? (nextVoxelX - rayStart.x) / ray.dir.x : FLT_MAX;
	float tMaxY = (ray.dir.y != 0) ? (nextVoxelY - rayStart.y) / ray.dir.y : FLT_MAX;
	float tMaxZ = (ray.dir.z != 0) ? (nextVoxelZ - rayStart.z) / ray.dir.z : FLT_MAX;

	// How far along the ray to travel to equal the width of a voxel
	const float tDeltaX = (ray.dir.x != 0) ? m_voxelSize / std::abs(ray.dir.x) : FLT_MAX;
	const float tDeltaY = (ray.dir.y != 0) ? m_voxelSize / std::abs(ray.dir.y) : FLT_MAX;
	const float tDeltaZ = (ray.dir.z != 0) ? m_voxelSize / std::abs(ray.dir.z) : FLT_MAX;

	while (currVoxel != endVoxel) 
	{
		if (VoxelAt(currVoxel).occupied)
			return VoxelAt(currVoxel).color;

		if (tMaxX < tMaxY) 
		{
			if (tMaxX < tMaxZ) 
			{
				currVoxel.x += stepX;
				tMaxX += tDeltaX;
			}
			else 
			{
				currVoxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
		else 
		{
			if (tMaxY < tMaxZ) 
			{
				currVoxel.y += stepY;
				tMaxY += tDeltaY;
			}
			else 
			{
				currVoxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
	}

	return Color();
}

std::vector<vec3i> VoxelGrid::GetRayPath(Physics::Ray const& ray)
{
	float tNear = 0.0f, tFar = 0.0f;

	if (!boundingBox.Intersect(ray, tNear, tFar))
		return std::vector<vec3i>(); // No collision with AABB

	tNear = fmax(tNear, 0);
	tFar = fmax(tFar, 0);

	const vec3 minExtent = boundingBox.GetMinExtent();
	const vec3 maxExtent = boundingBox.GetMaxExtent();

	vec3 rayStart = ray.origin + ray.dir * tNear - minExtent;
	vec3 rayEnd = ray.origin + ray.dir * tFar - minExtent;
	
	vec3i startVoxel(
		rayStart.x / m_voxelSize - minExtent.x,
		rayStart.y / m_voxelSize - minExtent.y,
		rayStart.z / m_voxelSize - minExtent.z);
	vec3i endVoxel(
		rayEnd.x / m_voxelSize - minExtent.x,
		rayEnd.y / m_voxelSize - minExtent.y,
		rayEnd.z / m_voxelSize - minExtent.z);


	vec3i currVoxel = startVoxel;

	float stepX = (ray.dir.x >= 0) ? 1 : -1;
	float stepY = (ray.dir.y >= 0) ? 1 : -1;
	float stepZ = (ray.dir.z >= 0) ? 1 : -1;

	float nextVoxelX = (currVoxel.x + stepX) * m_voxelSize;
	float nextVoxelY = (currVoxel.y + stepY) * m_voxelSize;
	float nextVoxelZ = (currVoxel.z + stepZ) * m_voxelSize;

	float tMaxX = (ray.dir.x != 0) ? (nextVoxelX - rayStart.x) / ray.dir.x : FLT_MAX;
	float tMaxY = (ray.dir.y != 0) ? (nextVoxelY - rayStart.y) / ray.dir.y : FLT_MAX;
	float tMaxZ = (ray.dir.z != 0) ? (nextVoxelZ - rayStart.z) / ray.dir.z : FLT_MAX;

	float tDeltaX = (ray.dir.x != 0) ? m_voxelSize / std::abs(ray.dir.x) : FLT_MAX;
	float tDeltaY = (ray.dir.y != 0) ? m_voxelSize / std::abs(ray.dir.y) : FLT_MAX;
	float tDeltaZ = (ray.dir.z != 0) ? m_voxelSize / std::abs(ray.dir.z) : FLT_MAX;

	std::vector<vec3i> Traversed;

	while (currVoxel != endVoxel && !SampleOutOfBounds(currVoxel))
	{
		Traversed.push_back(currVoxel);

		if (tMaxX < tMaxY)
		{
			if (tMaxX < tMaxZ)
			{
				currVoxel.x += stepX;
				tMaxX += tDeltaX;
			}
			else
			{
				currVoxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
		else
		{
			if (tMaxY < tMaxZ)
			{
				currVoxel.y += stepY;
				tMaxY += tDeltaY;
			}
			else
			{
				currVoxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
	}

	return Traversed;
}

void VoxelGrid::FillPath(std::vector<vec3i> path)
{
	for (auto& tile : path)
	{
		VoxelData voxel = VoxelAt(tile);

		if (voxel.occupied)
			voxel.color = Color(255, 0, 0);
		else
			m_voxels[tile.x + m_width * (tile.y + m_depth * tile.z)] = VoxelData(Color(255, 0, 0));
	}
	glDeleteBuffers(1, &ssbo);
	Upload(computeProgram);
}
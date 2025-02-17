#pragma once

struct AABB
{
  AABB() = default;
  AABB(const AABB&) = default;

  AABB(const vec3& min, const vec3& max)
    : m_initialMinExtents(min)
    , m_initialMaxExtents(max)
    , m_minExtents(min)
    , m_maxExtents(max)
  {
  }

  inline const vec3& GetMinExtents() const { return m_minExtents; }
  inline const vec3& GetMaxExtents() const { return m_maxExtents; }
  inline const vec3 GetExtents() const { return m_maxExtents - m_minExtents; }

  inline const vec3 GetCenter() const
  {
    return (m_minExtents + m_maxExtents) * 0.5f;
  }

  inline const bool Intersects(const AABB& other) const
  {
    vec3 d1 = other.GetMinExtents() - m_maxExtents;
    vec3 d2 = m_minExtents - other.GetMaxExtents();
    vec3 d = max(d1, d2);

    float maxDist = d[0];
    for (int i = 1; i < 3; i++) {
      if (maxDist < d[i])
        maxDist = d[i];
    }

    return maxDist < 0;
  }

  inline void Transform(const mat4& transform)
  {
    for (int i = 0; i < 3; i++) {
      m_minExtents[i] = m_maxExtents[i] = transform[3][i];

      for (int j = 0; j < 3; j++) {
        float e = transform[j][i] * m_initialMinExtents[j];
        float f = transform[j][i] * m_initialMaxExtents[j];
        if (e < f) {
          m_minExtents[i] += e;
          m_maxExtents[i] += f;
        } else {
          m_minExtents[i] += f;
          m_maxExtents[i] += e;
        }
      }
    }
  }

private:
  vec3 m_initialMinExtents = vec3();
  vec3 m_initialMaxExtents = vec3();

  vec3 m_minExtents = vec3();
  vec3 m_maxExtents = vec3();
};

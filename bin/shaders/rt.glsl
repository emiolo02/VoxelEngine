#define CORRECTION 0.0001

struct Ray {
    vec3 origin;
    vec3 direction;
};

uint
EncodeColor(vec4 c)
{
    ivec4 color = ivec4(c * 255.0);
    return color.r << 24 + color.g << 16 + color.b << 8 + color.a;
}

vec4
DecodeColor(uint c) {
    float a = c & 0xFF;
    float b = (c >> 8) & 0xFF;
    float g = (c >> 16) & 0xFF;
    float r = (c >> 24) & 0xFF;
    return vec4(r, g, b, a) / 255.0;
}

vec4
DecodeSteps(uint s, uint maxSteps) {
    return vec4(s) / maxSteps;
}

bool
IntersectAABB(Ray ray, out float tNear, out float tFar, vec3 minBounds, vec3 maxBounds) {
    vec3 tMin = (minBounds - ray.origin) / ray.direction;
    vec3 tMax = (maxBounds - ray.origin) / ray.direction;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    tNear = max(max(t1.x, t1.y), t1.z);
    tFar = min(min(t2.x, t2.y), t2.z);
    return tNear < tFar && tFar > 0.0;
}

vec3
NormalAABB(vec3 point, vec3 minBounds, vec3 maxBounds) {
    bvec3 hitMin = lessThan(abs(point - minBounds), vec3(2 * CORRECTION));
    bvec3 hitMax = lessThan(abs(point - maxBounds), vec3(2 * CORRECTION));
    return - vec3(hitMin) + vec3(hitMax);//normalize(sign(pc) * step(-CORRECTION, abs(pc) - halfSize));
}

Ray
CastRay(vec3 origin, mat4 invView, mat4 invProjection, vec2 uv) {
    vec4 target = invProjection * vec4(uv.x, uv.y, 1.0, 1.0);
    return Ray(origin, vec3(invView * vec4(normalize(target.xyz / target.w), 0.0)));
}

ivec3
GetOutOfBounds(vec3 rayDir, ivec3 gridSize) {
    bvec3 mask = greaterThan(rayDir, vec3(0.0));
    return ivec3(mask) * (gridSize + 1) - 1;
}

bool
InBounds(ivec3 pos, ivec3 bounds) {
    return pos.x != bounds.x && pos.y != bounds.y && pos.z != bounds.z;
}

void
InitDDA(float voxelSize, vec3 rayStart, vec3 rayDir, vec3 invRayDir, out ivec3 currentPos, out vec3 tMax, out vec3 tDelta, out ivec3 gridStep) {
    rayStart /= voxelSize;
    currentPos = ivec3(floor(rayStart));
    vec3 signDir = sign(rayDir);
    gridStep = ivec3(sign(signDir));
    tDelta = invRayDir * gridStep;
    tMax = (signDir * (currentPos - rayStart) + (signDir * 0.5) + 0.5) * tDelta;
}

void
StepDDA(vec3 tDelta, ivec3 gridStep, inout vec3 normal, inout vec3 tMax, inout ivec3 currentPos) {
    bvec3 mask = lessThanEqual(tMax.xyz, min(tMax.yzx, tMax.zxy));
    normal = ivec3(mask) * -gridStep;
    tMax += ivec3(mask) * tDelta;
    currentPos += ivec3(mask) * gridStep;
}

uint
GetIndex(ivec3 pos, ivec3 gridSize) {
    return pos.x + gridSize.x * (pos.y + gridSize.y * pos.z);
}

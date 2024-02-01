#include "config.h"
#include "GraphicsNode.h"
#include "GL/glew.h"
#include "ShaderResource.h"
#include "Camera.h"
#include "Model.h"

GraphicsNode::GraphicsNode()
{
	
}

void 
GraphicsNode::Draw(Camera cam, uint32 program)
{
	this->transform = translation * rotation * scale;

	UniformMat4("model"     , transform     , program);
	UniformMat4("projView"  , cam.projView  , program);
	UniformVec3("cameraPos" , cam.position  , program);
	
	model->Draw(program);
}

void 
GraphicsNode::SimpleDraw(mat4 lightView, uint32 program)
{
	UniformMat4("projView", lightView, program);
	UniformMat4("model", transform, program);

	model->SimpleDraw();
}
void
GraphicsNode::SetRotation(vec3 const& v)
{
	
	rotation = rotationaxis(v);
}

void GraphicsNode::SetRotation(mat4 const& m)
{
	rotation = m;
}

void GraphicsNode::RotateAxis(vec3 const& axis, float const& rad)
{
	rotation = glm::rotate(rotation, rad, axis);
}

void
GraphicsNode::Translate(const vec3& v)
{
	translation = mat4(
		vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(v.x, v.y, v.z, 1)
	);
}

mat4& GraphicsNode::GetTransform()
{
	return transform;
}

mat4& GraphicsNode::GetScale()
{
	return scale;
}

mat4& GraphicsNode::GetRotation()
{
	return rotation;
}

mat4& GraphicsNode::GetTranslation()
{
	return translation;
}

void GraphicsNode::Scale(const vec3& v)
{
	this->scale = mat4(
		vec4(v.x, 0, 0, 0),
		vec4(0, v.y, 0, 0),
		vec4(0, 0, v.z, 0),
		vec4(0,   0, 0, 1)
	);
}
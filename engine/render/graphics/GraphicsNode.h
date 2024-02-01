#pragma once
#include <memory>
//#include "core/app.h"

class Model;
class Camera;

class GraphicsNode
{
public:
	GraphicsNode();
	std::shared_ptr<Model> model;

	void Draw(Camera cam, uint32 program);
	void SimpleDraw(mat4 lightView, uint32 program);

	void SetRotation(vec3 const& v);
	void SetRotation(mat4 const& m);
	void RotateAxis(vec3 const& axis, float const& rad);
	void Translate(const vec3& v);

	mat4& GetTransform();
	mat4& GetScale();
	mat4& GetRotation();
	mat4& GetTranslation();

	void Scale(const vec3& v);
private:
	mat4 transform;

	mat4 scale = mat4(1.0f);
	mat4 rotation = mat4(1.0f);
	mat4 translation = mat4(1.0f);
};
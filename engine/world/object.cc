#include "config.h"
#include "object.h"
#include "render/graphics/MeshResource.h"
#include "render/graphics/Material.h"
#include "world.h"

uint32 Object::nextId = 0;

Object::Object(std::string modelPath, vec3 position, vec3 scale, vec3 rotation, float mass, bool isStatic)
{
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;

	gNode.model = MeshResource::Instance()->LoadModel(modelPath);
	gNode.Translate(position);
	gNode.Scale(scale);

	gNode.SetRotation(rotation);

	std::vector<std::string> pathSplit;
	split(modelPath, '/', pathSplit);
	std::vector<std::string> fileSplit;
	split(pathSplit[pathSplit.size() - 1], '.', fileSplit);

	name = fileSplit[0];

	id = nextId;
	nextId++;

	rb = new Physics::RigidBody(this, mass, isStatic);
	World::Instance()->AddObject(this);
}

void
Object::Update(float dt)
{
	gNode.Translate(position);
	gNode.SetRotation(mat4(orientation));


	gNode.Scale(scale);

	rb->GetCollider().GetCoarse().UpdateBounds(gNode.GetTransform());
}

Physics::RigidBody*
Object::GetRigidBody()
{
	return rb;
}

void 
Object::SetColor(vec3 diffuse)
{
	for (auto& mesh : gNode.model->meshes)
	{
		for (auto& primitive : mesh.primitives)
		{
			BlinnPhongMaterial* bpMat = (BlinnPhongMaterial*)(primitive.material);
			bpMat->SetProperties(vec3(1, 1, 1), diffuse, vec3(1, 1, 1));
		}
	}
}

uint32 Object::GetID()
{
	return id;
}

void
Object::Draw(Camera camera, GLuint program)
{
	gNode.Draw(camera, program);
}

void 
Object::SimpleDraw(Camera camera, GLuint program)
{
	gNode.SimpleDraw(camera.projView, program);
}

std::string 
Object::GetName() const&
{
	return name;
}

void Object::SetName(std::string const& n)
{
	name = n;
}

GraphicsNode&
Object::GetGNode()
{
	return gNode;
}
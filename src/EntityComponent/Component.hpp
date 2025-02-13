#pragma once
#include <limits>

namespace ecs {
// The reason for all the templates is to support runtime dynamic types

struct Entity;
class Scene;

typedef Entity* EntityHandle;
// constexpr EntityHandle NULL_ENTITY = nullptr;

struct BaseComponent;
typedef size_t ComponentId;
typedef size_t (*ComponentCreateFunction)(std::vector<uint8>& memoryPool,
                                          EntityHandle entity,
                                          BaseComponent* comp);
typedef void (*ComponentFreeFunction)(BaseComponent* comp);

class Entity
{
  template<typename T>
  T& GetComponent();

  template<typename T>
  bool AddComponent();

  const std::string& GetName() const { return m_name; }
  void Rename(const std::string& newName);

private:
  std::string m_name = "Entity";

  struct ComponentData
  {
    ComponentId id;
    size_t index = SIZE_MAX;
  };

  std::vector<ComponentData> m_ComponentData;
  size_t index = SIZE_MAX;

  Scene* m_Scene;
  friend class Scene;
};

// Don't inherit from this, use "Component" instead.
struct BaseComponent
{
public:
  static ComponentId RegisterComponentType(ComponentCreateFunction createFn,
                                           ComponentFreeFunction freeFn,
                                           size_t size);
  EntityHandle parent;

  inline static const ComponentCreateFunction GetCreateFunction(ComponentId id)
  {
    return std::get<0>((*s_componentTypes)[id]);
  }

  inline static const ComponentFreeFunction GetFreeFunction(ComponentId id)
  {
    return std::get<1>((*s_componentTypes)[id]);
  }

  inline static const size_t GetSize(ComponentId id)
  {
    return std::get<2>((*s_componentTypes)[id]);
  }

  inline static bool IsValidType(ComponentId id)
  {
    return id < s_componentTypes->size();
  }

  virtual void ImGuiDraw() {}

private:
  static std::vector<
    std::tuple<ComponentCreateFunction, ComponentFreeFunction, size_t>>*
    s_componentTypes;
};

// When inheriting, pass the new struct as the template parameter,
// this is to ensure each type of component gets assigned a unique id.
// example usage: struct Transform : public Component<Transform>
template<typename T>
struct Component : public BaseComponent
{
  static const ComponentCreateFunction s_createFunc;
  static const ComponentFreeFunction s_freeFunc;
  static const ComponentId s_id;
  static const size_t s_size;
};

// Creates a component and allocates it in the component memory pool.
// returns the index in the memory.
template<typename TComponent>
size_t
ComponentCreate(std::vector<uint8>& memoryPool,
                EntityHandle entity,
                BaseComponent* comp)
{
  size_t index = memoryPool.size();
  memoryPool.resize(index + TComponent::s_size);
  TComponent* component =
    new (&memoryPool[index]) TComponent(*(TComponent*)comp);
  component->parent = entity;
  return index;
}

template<typename TComponent>
void
ComponentFree(BaseComponent* comp)
{
  TComponent* component = (TComponent*)comp;
  component->~TComponent();
}

template<typename T>
const ComponentCreateFunction Component<T>::s_createFunc(ComponentCreate<T>);

template<typename T>
const ComponentFreeFunction Component<T>::s_freeFunc(ComponentFree<T>);

// Gives each type of component a unique id.
template<typename T>
const ComponentId Component<T>::s_id(
  BaseComponent::RegisterComponentType(ComponentCreate<T>,
                                       ComponentFree<T>,
                                       sizeof(T)));

// Set component size.
template<typename T>
const size_t Component<T>::s_size(sizeof(T));

}

#include "Component.hpp"
#include "EntityComponentSystem.hpp"

namespace ecs {
  template<typename T>
  T &
  Entity::GetComponent() {
    return *m_Scene->GetComponent<T>(this);
  }

  template<typename T>
  bool
  Entity::AddComponent() {
    return m_Scene->AddComponent<T>(this);
  }

  void
  Entity::Rename(const std::string &newName) {
    m_Scene->RenameEntity(this, newName);
  }

  std::vector<std::tuple<ComponentCreateFunction, ComponentFreeFunction, size_t> > *
  BaseComponent::s_componentTypes;

  ComponentId
  BaseComponent::RegisterComponentType(ComponentCreateFunction createFn,
                                       ComponentFreeFunction freeFn,
                                       size_t size) {
    if (s_componentTypes == nullptr)
      s_componentTypes = new std::vector<
        std::tuple<ComponentCreateFunction, ComponentFreeFunction, size_t> >();

    const ComponentId id = s_componentTypes->size();

    s_componentTypes->push_back({createFn, freeFn, size});

    return id;
  }
}

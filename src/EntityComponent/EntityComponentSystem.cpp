#include "EntityComponentSystem.hpp"
#include <iostream>

namespace ecs {
  Scene::~Scene() {
    for (auto &comp: m_components) {
      size_t compSize = BaseComponent::GetSize(comp.first);
      auto freeFn = BaseComponent::GetFreeFunction(comp.first);

      for (int i = 0; i < comp.second.size(); i += compSize) {
        freeFn((BaseComponent *) &comp.second[i]);
      }
    }

    for (auto &entity: m_entities)
      delete entity;

    m_entities.resize(0);
  }

  EntityHandle
  Scene::CreateEntityImpl(const std::string &name,
                          BaseComponent **components,
                          const ComponentId *componentIds,
                          size_t numComponents) {
    // Create entity and add to scene
    EntityHandle entity = new Entity();

    std::string entityName = name;
    uint32 append = 0;
    while (m_entityNames.find(entityName) != m_entityNames.end()) {
      entityName = name + std::to_string(append);
      append++;
    }

    entity->m_name = entityName;
    m_entityNames[entityName] = entity;

    for (int i = 0; i < numComponents; ++i) {
      const ComponentId compId = componentIds[i];

      if (!BaseComponent::IsValidType(compId)) {
        std::cout << "Component with id \'" << compId << "\' is invalid!\n";
        return EntityHandle();
      }

      AddComponentImpl(entity, compId, components[i]);
    }

    entity->index = m_entities.size();
    entity->m_Scene = this;

    m_entities.push_back(entity);

    // Add entity to listeners
    for (auto listener: m_sceneListeners) {
      bool isValid = true;
      const auto &listenerComponents = listener->GetComponents();
      const auto &listenerComponentFlags = listener->GetComponentFlags();
      for (int i = 0; i < listenerComponents.size(); ++i) {
        bool hasComponent = false;
        bool isRequired = (listenerComponentFlags[i] & FLAG_OPTIONAL) != 0;
        for (auto &entityComponent: entity->m_ComponentData) {
          if (listenerComponents[i] == entityComponent.id) {
            hasComponent = true;
            break;
          }
        }
        if (!hasComponent && isRequired) {
          isValid = false;
          break;
        }
      }
      if (isValid)
        listener->OnCreateEntity(entity);
    }

    return entity;
  }

  void
  Scene::DestroyEntity(EntityHandle entity) {
    // Remove entity from listeners
    for (auto listener: m_sceneListeners) {
      bool isValid = true;
      const auto &listenerComponents = listener->GetComponents();
      for (const auto &listenerComponent: listenerComponents) {
        bool hasComponent = false;
        for (auto &entityComponent: entity->m_ComponentData) {
          if (listenerComponent == entityComponent.id) {
            hasComponent = true;
            break;
          }
        }
        if (!hasComponent) {
          isValid = false;
          break;
        }
      }
      if (isValid)
        listener->OnDestroyEntity(entity);
    }

    // Remove entity from scene
    for (auto &compData: entity->m_ComponentData)
      DeleteComponent(compData.id, compData.index);

    // Swap remove
    size_t swappedIndex = entity->index;
    std::swap(entity, m_entities.back());
    entity->index = swappedIndex;
    m_entities.pop_back();
  }

  EntityHandle
  Scene::GetEntityByName(const std::string &name) {
    if (m_entityNames.find(name) == m_entityNames.end()) {
      std::cout << "Could not find entity with name \"" << name << "\"!\n";
      return nullptr;
    }
    return m_entityNames[name];
  }

  bool
  Scene::RenameEntity(EntityHandle entity, const std::string &newName) {
    if (m_entityNames.find(newName) == m_entityNames.end())
      return false;

    m_entityNames.erase(entity->m_name);

    entity->m_name = newName;
    m_entityNames[newName] = entity;

    return true;
  }

  EntityHandle
  Scene::GetMainCamera() {
    if (!m_mainCamera)
      std::cout << "Main camera has not been set!\n";
    return m_mainCamera;
  }

  bool
  Scene::AddComponentImpl(EntityHandle entityHandle,
                          ComponentId compId,
                          BaseComponent *component) {
    // Create component and add to entity
    auto &compMemory = m_components[compId];
    auto createFn = BaseComponent::GetCreateFunction(compId);

    entityHandle->m_ComponentData.push_back(
      {compId, createFn(compMemory, entityHandle, component)});

    for (auto &entityComp: entityHandle->m_ComponentData) {
      if (entityComp.id == compId)
        return false;
    }

    // Add to listener
    for (auto listener: m_sceneListeners) {
      for (auto componentType: listener->GetComponents()) {
        if (componentType == compId) {
          listener->OnAddComponent(entityHandle, compId);
          break;
        }
      }
    }

    return true;
  }

  BaseComponent *
  Scene::GetComponentImpl(EntityHandle entity,
                          ComponentId compId,
                          std::vector<uint8> &compMemory) {
    for (auto &entityComp: entity->m_ComponentData) {
      if (compId == entityComp.id)
        return (BaseComponent *) &compMemory[entityComp.index];
    }

    return nullptr;
  }

  bool
  Scene::RemoveComponent(EntityHandle entity, ComponentId compId) {
    for (auto listener: m_sceneListeners) {
      for (auto componentType: listener->GetComponents()) {
        if (componentType == compId) {
          listener->OnDestroyComponent(entity, compId);
          break;
        }
      }
    }

    for (int i = 0; i < entity->m_ComponentData.size(); ++i) {
      auto &entityComp = entity->m_ComponentData[i];

      if (compId == entityComp.id) {
        DeleteComponent(compId, entityComp.index);

        // Swap remove
        const size_t srcIndex = entity->m_ComponentData.size();
        entityComp = entity->m_ComponentData[srcIndex];
        entity->m_ComponentData.pop_back();
        return true;
      }
    }

    return false;
  }

  void
  Scene::DeleteComponent(ComponentId compId, size_t index) {
    auto &compMemory = m_components[compId];
    auto freeFn = BaseComponent::GetFreeFunction(compId);

    const size_t compSize = BaseComponent::GetSize(compId);

    const size_t srcIndex = compMemory.size() - compSize;

    BaseComponent *srcComponent = (BaseComponent *) &compMemory[srcIndex];
    BaseComponent *destComponent = (BaseComponent *) &compMemory[index];

    freeFn(destComponent);

    if (index == srcIndex) {
      // is the final component of type in m_components
      compMemory.resize(index);
      return;
    }

    std::memcpy((void *) destComponent, (void *) srcComponent, compSize);

    // Update component index in the affected entity
    auto entity = srcComponent->parent;
    for (auto &entityComp: entity->m_ComponentData) {
      if (compId == entityComp.id && srcIndex == entityComp.index) {
        entityComp.index = index;
        break;
      }
    }

    compMemory.resize(srcIndex);
  }

  void
  Scene::UpdateSystems(float deltaSeconds, SystemList &systemList) {
    std::vector<BaseComponent *> compParams;
    std::vector<std::vector<uint8> *> compMemories;

    for (uint32 i = 0; i < systemList.size(); ++i) {
      const auto &system = systemList[i];
      const auto &compTypes = system->GetComponentTypes();

      system->scene = this;

      if (compTypes.size() == 1) {
        const auto &compMemory = m_components[compTypes[0]];
        const size_t compSize = BaseComponent::GetSize(compTypes[0]);

        for (size_t index = 0; index < compMemory.size(); index += compSize) {
          BaseComponent *comp = (BaseComponent *) &compMemory[index];
          system->UpdateComponents(deltaSeconds, &comp);
        }
      } else {
        compParams.resize(std::max(compParams.size(), compTypes.size()));
        compMemories.resize(std::max(compMemories.size(), compTypes.size()));
        UpdateSystemMultipleComponents(
          system, deltaSeconds, compTypes, compParams, compMemories);
      }
    }
  }

  void
  Scene::UpdateSystemMultipleComponents(
    BaseSystem *system,
    float deltaSeconds,
    const std::vector<ComponentId> &compTypes,
    std::vector<BaseComponent *> &compParams,
    std::vector<std::vector<uint8> *> &compMemories) {
    auto &compFlags = system->GetComponentFlags();

    // Pre fetch the memory for each component
    for (int i = 0; i < compTypes.size(); ++i) {
      compMemories[i] = &m_components[compTypes[i]];
    }

    size_t minSizeIndex = FindLeastCommonComponent(compTypes, compFlags);

    auto &compMemory = *compMemories[minSizeIndex];
    const size_t compSize = BaseComponent::GetSize(compTypes[minSizeIndex]);

    for (size_t index = 0; index < compMemory.size(); index += compSize) {
      compParams[minSizeIndex] = (BaseComponent *) &compMemory[index];
      EntityHandle entityHandle = compParams[minSizeIndex]->parent;

      bool isValid = true;
      for (size_t i = 0; i < compTypes.size(); ++i) {
        if (i == minSizeIndex)
          continue;

        compParams[i] =
            GetComponentImpl(entityHandle, compTypes[i], *compMemories[i]);

        if (compParams[i] == nullptr && (compFlags[i] & FLAG_OPTIONAL) != 0) {
          isValid = false;

          std::cout << "Component with Id \'" << compTypes[i]
              << "\' doesn't exist in entity with index \'"
              << entityHandle->index
              << "\' \nThis update will be ignored.\n\n";

          break;
        }
      }

      if (isValid)
        system->UpdateComponents(deltaSeconds, &compParams[0]);
    }
  }

  size_t
  Scene::FindLeastCommonComponent(const std::vector<ComponentId> &compTypes,
                                  const std::vector<ComponentFlag> &compFlags) {
    size_t minSize = (size_t) -1;
    size_t minIndex = (size_t) -1;

    for (size_t i = 0; i < compTypes.size(); ++i) {
      if ((compFlags[i] & FLAG_OPTIONAL) != 0)
        continue;

      size_t typeSize = BaseComponent::GetSize(compTypes[i]);
      size_t count = m_components[compTypes[i]].size() / typeSize;

      if (count <= minSize) {
        minSize = count;
        minIndex = i;
      }
    }

    return minIndex;
  }
}

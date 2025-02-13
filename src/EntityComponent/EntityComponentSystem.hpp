#pragma once

#include "Component.hpp"
#include "System.hpp"
#include <map>

namespace ecs {
  class SceneListener {
  public:
    virtual void OnCreateEntity(EntityHandle entity) {
    }

    virtual void OnDestroyEntity(EntityHandle entity) {
    }

    virtual void OnAddComponent(EntityHandle entity, ComponentId componentId) {
    }

    virtual void OnDestroyComponent(EntityHandle entity, ComponentId componentId) {
    }

    inline const std::vector<ComponentId> &GetComponents() const {
      return m_componentTypes;
    }

    inline const std::vector<ComponentFlag> &GetComponentFlags() const {
      return m_componentFlags;
    }

  protected:
    inline void AddComponent(ComponentId componentId,
                             ComponentFlag flag = ComponentFlag::FLAG_NONE) {
      m_componentTypes.push_back(componentId);
      m_componentFlags.push_back(flag);
    }

  private:
    std::vector<ComponentId> m_componentTypes;
    std::vector<ComponentFlag> m_componentFlags;
  };

  class Scene {
  public:
    NON_COPYABLE(Scene);

    Scene() {
    }

    ~Scene();

    // Listener methods
    inline void AddListener(SceneListener *listener) {
      m_sceneListeners.push_back(listener);
    }

    // Entity methods
    template<class... TComponents>
    inline EntityHandle CreateEntity(const std::string &name,
                                     TComponents &... compArgs) {
      // variadic template shenanigans
      BaseComponent *comps[] = {&compArgs...};
      ComponentId ids[] = {TComponents::s_id...};
      return CreateEntityImpl(
        name, comps, ids, sizeof(ids) / sizeof(ComponentId));
    }

    void DestroyEntity(EntityHandle entityHandle);

    inline const std::vector<EntityHandle> &GetEntities() { return m_entities; }

    EntityHandle GetEntityByName(const std::string &name);

    bool RenameEntity(EntityHandle entity, const std::string &newName);

    inline void SetMainCamera(EntityHandle camera) { m_mainCamera = camera; }

    EntityHandle GetMainCamera();

    inline const std::vector<BaseComponent *> GetEntityComponents(
      EntityHandle entity) {
      std::vector<BaseComponent *> entityComponents(
        entity->m_ComponentData.size());

      for (uint32 i = 0; i < entityComponents.size(); ++i) {
        Entity::ComponentData &compData = entity->m_ComponentData[i];
        entityComponents[i] =
            (BaseComponent *) &m_components[compData.id][compData.index];
      }

      return entityComponents;
    }

    // Component methods
    template<typename TComponent>
    inline bool AddComponent(EntityHandle entityHandle, TComponent *component) {
      return AddComponentImpl(entityHandle, TComponent::s_id, component);
    }

    template<typename TComponent>
    inline TComponent *GetComponent(EntityHandle entityHandle) {
      BaseComponent *component = GetComponentImpl(
        entityHandle, TComponent::s_id, m_components[TComponent::s_id]);

      // @TODO - handle this properly
      if (component == nullptr) {
        exit(-1);
      }
      return (TComponent *) component;
    }

    template<typename TComponent>
    inline bool DestroyComponent(EntityHandle entity) {
      return RemoveComponent(entity, TComponent::s_id);
    }

    void UpdateSystems(float deltaSeconds, SystemList &systemList);

  private:
    std::map<ComponentId, std::vector<uint8> > m_components;
    std::vector<EntityHandle> m_entities;
    std::map<std::string, EntityHandle> m_entityNames;

    std::vector<SceneListener *> m_sceneListeners;

    EntityHandle m_mainCamera = nullptr;

    EntityHandle CreateEntityImpl(const std::string &name,
                                  BaseComponent **components,
                                  const ComponentId *componentIds,
                                  size_t numComponents);

    bool AddComponentImpl(EntityHandle entityHandle,
                          ComponentId compId,
                          BaseComponent *component);

    BaseComponent *GetComponentImpl(EntityHandle entityHandle,
                                    ComponentId compId,
                                    std::vector<uint8> &compMemory);

    bool RemoveComponent(EntityHandle entityHandle, ComponentId compId);

    void DeleteComponent(ComponentId compId, size_t index);

    void UpdateSystemMultipleComponents(
      BaseSystem *system,
      float deltaSeconds,
      const std::vector<ComponentId> &compTypes,
      std::vector<BaseComponent *> &compParams,
      std::vector<std::vector<uint8> *> &compMemories);

    size_t FindLeastCommonComponent(const std::vector<ComponentId> &compTypes,
                                    const std::vector<ComponentFlag> &compFlags);
  };
}

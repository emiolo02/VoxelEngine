#pragma once

#include "Component.hpp"

namespace ecs {
	enum ComponentFlag {
		FLAG_NONE = 0,
		FLAG_OPTIONAL = 1
	};

	class BaseSystem {
	public:
		BaseSystem() {
		}

		virtual void UpdateComponents(float deltaSeconds, BaseComponent **component) {
		}

		const std::vector<ComponentId> &GetComponentTypes() { return m_componentTypes; }
		const std::vector<ComponentFlag> &GetComponentFlags() { return m_componentFlags; }

		bool IsValid();

		Scene *scene = nullptr;

	protected:
		void AddComponent(ComponentId componentType, ComponentFlag flag = FLAG_NONE) {
			m_componentTypes.push_back(componentType);
			m_componentFlags.push_back(flag);
		}

	private:
		std::vector<ComponentId> m_componentTypes;
		std::vector<ComponentFlag> m_componentFlags;
	};

	class SystemList {
	public:
		inline bool AddSystem(BaseSystem &system) {
			if (!system.IsValid())
				return false;

			m_systems.push_back(&system);
			return true;
		}

		bool DestroySystem(BaseSystem &system);

		inline size_t size() { return m_systems.size(); }
		inline BaseSystem *operator[](const size_t i) { return m_systems[i]; }

	private:
		std::vector<BaseSystem *> m_systems;
	};
}

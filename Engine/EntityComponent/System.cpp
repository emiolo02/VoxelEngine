#include "System.hpp"

namespace ecs {
	// A system is considered valid if it contains at least one required component.
	bool BaseSystem::IsValid() {
		// At least one component must be required.
		for (auto flag: m_componentFlags)
			if ((flag & FLAG_OPTIONAL) == 0)
				return true;

		return false;
	}

	// Returns true if a system was deleted.
	bool SystemList::DestroySystem(BaseSystem &system) {
		for (int i = 0; i < m_systems.size(); ++i) {
			if (&system == m_systems[i]) {
				m_systems.erase(m_systems.begin() + i);
				return true;
			}
		}

		return false;
	}
}

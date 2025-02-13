#include <Geode/modify/HardStreak.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyHardStreak, HardStreak) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("HardStreak::updateStroke", 3999);
	}
	void updateStroke(float delta) {
		const auto manager = Manager::getSharedInstance();

		if (Utils::modEnabled() && manager->noWavePulse && PlayLayer::get()) {
			double minimum = manager->addMinumumWidth ? .5 : .0;
			m_pulseSize = static_cast<float>(minimum + manager->wavePulseSize);
		}

		HardStreak::updateStroke(delta);
	}
};
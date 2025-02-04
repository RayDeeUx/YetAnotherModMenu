#include <Geode/modify/CCMotionStreak.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyCCMotionStreak, CCMotionStreak) {
	virtual void update(float delta) {
		if (!Utils::modEnabled() || !this->getUserObject("qualified-for-elongation"_spr) || !PlayLayer::get()) return CCMotionStreak::update(delta);
		CCMotionStreak::update(delta / static_cast<float>(Manager::getSharedInstance()->trailLengthModifier));
	}
};
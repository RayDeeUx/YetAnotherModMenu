#include <Geode/modify/CCMotionStreak.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyCCMotionStreak, CCMotionStreak) {
	virtual void update(float delta) {
		const auto manager = Manager::getSharedInstance();
		if (!Utils::modEnabled() || !this->getUserObject("qualified-for-elongation"_spr) || !GJBaseGameLayer::get() || !manager->trailLength) return CCMotionStreak::update(delta);
		CCMotionStreak::update(delta / static_cast<float>(manager->trailLengthModifier));
	}
};
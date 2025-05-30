#include <Geode/modify/FMODAudioEngine.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify (MyFMODAudioEngine, FMODAudioEngine) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("FMODAudioEngine::playEffect", -1999);
	}
	int playEffect(gd::string p0, float p1, float p2, float p3) {
		const auto manager = Manager::getSharedInstance();
		if (!Utils::modEnabled() || !manager->filth) return FMODAudioEngine::playEffect(p0, p1, p2, p3);
		std::string desiredPath = manager->filthyPath.string();
		if (!manager->filthyGameplay && GJBaseGameLayer::get()) desiredPath = p0;
		else if (!Utils::isSupportedFMODExtension(manager->filthyPath.string())) desiredPath = p0;
		return FMODAudioEngine::playEffect(desiredPath, p1, p2, p3);
	}
};
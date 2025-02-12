#include <Geode/modify/FMODAudioEngine.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify (MyFMODAudioEngine, FMODAudioEngine) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("FMODAudioEngine::playEffect", -1999);
	}
	void playEffect(gd::string p0, float p1, float p2, float p3) {
		Manager* manager = Manager::getSharedInstance();
		if (!manager->filthyGameplay && GJBaseGameLayer::get()) return FMODAudioEngine::playEffect(p0, p1, p2, p3);
		if (!std::filesystem::exists(manager->filthyPath)) return FMODAudioEngine::playEffect(p0, p1, p2, p3);
		if (manager->filthyPath.string().empty()) return FMODAudioEngine::playEffect(p0, p1, p2, p3);
		if (!Utils::modEnabled() || !manager->filth) return FMODAudioEngine::playEffect(p0, p1, p2, p3);
		return FMODAudioEngine::playEffect(manager->filthyPath.string(), p1, p2, p3);
	}
};
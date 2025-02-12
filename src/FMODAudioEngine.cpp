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
		if (!std::filesystem::exists(manager->filthyPath)) return FMODAudioEngine::playEffect(p0, p1, p2, p3);
		if (manager->filthyPath.string().empty()) return FMODAudioEngine::playEffect(p0, p1, p2, p3);
		if (!Utils::modEnabled() || !manager->filth) return FMODAudioEngine::playEffect(p0, p1, p2, p3);
		return FMODAudioEngine::playEffect(manager->filthyPath.string(), p1, p2, p3);
	}
};

class $modify (MyMusicDownloadManager, MusicDownloadManager) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("MusicDownloadManager::pathForSFX", -1999);
	}
	gd::string pathForSFX(int p0) {
		Manager* manager = Manager::getSharedInstance();
		if (!std::filesystem::exists(manager->filthyPath)) return MusicDownloadManager::pathForSFX(p0);
		if (manager->filthyPath.string().empty()) return MusicDownloadManager::pathForSFX(p0);
		if (!Utils::modEnabled() || !manager->filth) return MusicDownloadManager::pathForSFX(p0);
		return manager->filthyPath.string();
		// return "sfx/s4451.ogg";
	}
};
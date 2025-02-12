#include <Geode/modify/MusicDownloadManager.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify (MyMusicDownloadManager, MusicDownloadManager) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("MusicDownloadManager::pathForSFX", -1999);
	}
	gd::string pathForSFX(int p0) {
		Manager* manager = Manager::getSharedInstance();
		log::info("p0: {}", p0);
		log::info("manager->filthyPath: {}", manager->filthyPath);
		log::info("std::filesystem::exists(manager->filthyPath): {}", std::filesystem::exists(manager->filthyPath));
		if (!manager->filthyGameplay && GJBaseGameLayer::get()) return MusicDownloadManager::pathForSFX(p0);
		if (!std::filesystem::exists(manager->filthyPath)) return MusicDownloadManager::pathForSFX(p0);
		if (manager->filthyPath.string().empty()) return MusicDownloadManager::pathForSFX(p0);
		if (!Utils::modEnabled() || !manager->filth) return MusicDownloadManager::pathForSFX(p0);
		return manager->filthyPath.string();
		// return "sfx/s4451.ogg";
	}
};
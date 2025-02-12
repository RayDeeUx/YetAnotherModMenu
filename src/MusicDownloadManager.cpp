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
		const std::string& originalPath = MusicDownloadManager::pathForSFX(p0);
		if (!manager->filthyGameplay && GJBaseGameLayer::get()) return originalPath;
		if (!std::filesystem::exists(manager->filthyPath)) return originalPath;
		if (manager->filthyPath.string().empty()) return originalPath;
		if (!Utils::isSupportedFMODExtension(manager->filthyPath.string())) return originalPath;
		if (!Utils::modEnabled() || !manager->filth) return originalPath;
		return manager->filthyPath.string();
		// return "sfx/s4451.ogg";
	}
};
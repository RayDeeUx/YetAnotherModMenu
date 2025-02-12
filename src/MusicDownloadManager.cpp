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
		if (!manager->filthyGameplay && GJBaseGameLayer::get()) {
			if (PlayLayer::get() && !CCScene::get()->getChildByType<PauseLayer>(0)) return originalPath;
			if (LevelEditorLayer::get() && !CCScene::get()->getChildByType<EditorPauseLayer>(0)) return originalPath;
		}

		const std::string& desiredPath = manager->filthyPath.string();
		if (!desiredPath.empty()) {
			if (!std::filesystem::exists(manager->filthyPath)) return originalPath;
			if (!Utils::isSupportedFMODExtension(desiredPath)) return originalPath;
		}
		if (!Utils::modEnabled() || !manager->filth) return originalPath;
		if (desiredPath.empty()) {
			#ifdef GEODE_IS_ANDROID
			return "sfx/s4451.ogg";
			#else
			return originalPath;
			#endif
		}
		return desiredPath;
		// return "sfx/s4451.ogg";
	}
};
#include <Geode/ui/GeodeUI.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::cocos;

namespace Utils {
	template<class T> T getSetting(const std::string& setting) { return Mod::get()->getSettingValue<T>(setting); }

	bool getBool(const std::string& setting) { return getSetting<bool>(setting); }
	
	int64_t getInt(const std::string& setting) { return getSetting<int64_t>(setting); }
	
	double getDouble(const std::string& setting) { return getSetting<double>(setting); }

	std::string getString(const std::string& setting, bool isPath) {
		if (!isPath) return getSetting<std::string>(setting);
		return getSetting<std::filesystem::path>(setting).string();
	}

	ccColor3B getColor(const std::string& setting) { return getSetting<ccColor3B>(setting); }

	ccColor4B getColorAlpha(const std::string& setting) { return getSetting<ccColor4B>(setting); }

	bool modEnabled() { return getBool("enabled"); }
	
	bool isModLoaded(const std::string& modID) { return Loader::get()->isModLoaded(modID); }

	Mod* getMod(const std::string& modID) { return Loader::get()->getLoadedMod(modID); }

	std::string getModVersion(Mod* mod) { return mod->getVersion().toNonVString(); }

	PlayerObject* getSelectedPlayerObjectToModfy(const PlayLayer *pl) {
		if (!pl) return nullptr;
		if (Manager::getSharedInstance()->hasLoadedSDI && Utils::getMod(SDI)->getSavedValue<bool>("2pselected")) return pl->m_player2;
		return pl->m_player1;
	}

	std::string getNodeName(CCObject* node) {
		#ifdef GEODE_IS_WINDOWS
		return typeid(*node).name() + 6;
		#else
		std::string ret;

		int status = 0;
		auto demangle = abi::__cxa_demangle(typeid(*node).name(), 0, 0, &status);
		if (status == 0) {
			ret = demangle;
		}
		free(demangle);

		return ret;
		#endif
	}

	#ifdef GEODE_IS_ANDROID
	void openSettings(CCObject*) {
		openSettingsPopup(Mod::get());
	}

	void addButtonToNode(CCNode* node, CCLayer* layer, cocos2d::SEL_MenuHandler callback) {
		if (!node) return;
		CircleButtonSprite* buttonSprite = CircleButtonSprite::createWithSprite("logoForShortcutButton.png"_spr, 1, CircleBaseColor::DarkAqua, CircleBaseSize::SmallAlt);
		buttonSprite->setID("settings-shortcut-sprite"_spr);
		CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(buttonSprite, layer, callback);
		button->setID("settings-shortcut"_spr);
		node->addChild(button);
		node->updateLayout();
	}
	#endif

	bool isSupportedFMODExtension(const std::string &path) {
		return std::filesystem::exists(path) && !path.empty() && (path.ends_with(".mp3") || path.ends_with(".wav") || path.ends_with(".ogg") || path.ends_with(".oga") || path.ends_with(".flac"));
	}


}
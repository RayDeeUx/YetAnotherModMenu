#pragma once

#define SDI "weebify.separate_dual_icons"

using namespace geode::prelude;

namespace Utils {
	template<class T> T getSetting(const std::string& setting);
	bool getBool(const std::string& setting);
	int64_t getInt(const std::string& setting);
	double getDouble(const std::string& setting);
	std::string getString(const std::string& setting);
	ccColor3B getColor(const std::string& setting);
	ccColor4B getColorAlpha(const std::string& setting);
	bool modEnabled();
	
	bool isModLoaded(const std::string& modID);
	Mod* getMod(const std::string& modID);
	std::string getModVersion(Mod* mod);

	PlayerObject* getSelectedPlayerObjectToModfy(const PlayLayer* pl = PlayLayer::get());

	std::string getNodeName(CCObject* node);
	#ifdef GEODE_IS_ANDROID
	void openSettings(CCObject* sender = nullptr);
	void addButtonToNode(CCNode* node, CCLayer* layer, cocos2d::SEL_MenuHandler callback);
	#endif
}
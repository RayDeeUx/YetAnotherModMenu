#pragma once
#include "Utils.hpp"

// Manager.hpp structure by acaruso
// reused with explicit permission and strong encouragement

using namespace geode::prelude;

class Manager {

protected:
	static Manager* instance;
public:

	bool coinTracingOpacityModifiers = false;
	bool previouslyCollectedModifier = false;
	bool hasQOLModCommunityEdition = false;
	bool coinTracingDisabledCoin = false;
	bool garageInPauseMenu = false;
	bool showBestPercent = false;
	bool addMinumumWidth = false;
	bool filthyGameplay = false;
	bool pulseMenuTitle = false;
	bool calledAlready = false;
	bool hasLoadedSDI = false;
	bool noWavePulse = false;
	bool isPauseShop = false;
	bool trailLength = false;
	bool traceCoins = false;
	bool hasQOLMod = false;
	bool enabled = false;
	bool filth = false;

	double coinTracingThickness = 0.0;
	double coinOpacityModifier = 0.0;
	double trailLengthModifier = 0.0;
	double pulseScaleFactor = 0.0;
	double wavePulseSize = 0.0;

	char customSeparator = '/';

	std::string colorMode = "";

	std::filesystem::path filthyPath = "";

	ccColor4B colorFromSettings;

	GLubyte coinTraceOpacity;

	geode::Mod* qolMod = nullptr;
	geode::Mod* qolModCommunity = nullptr;

	static Manager* getSharedInstance() {
		if (!instance) instance = new Manager();
		return instance;
	}

	static void load() {
		if (!instance) instance = new Manager();
		instance->coinTracingOpacityModifiers = Utils::getBool("coinTracingOpacityModifiers");
		instance->previouslyCollectedModifier = Utils::getBool("previouslyCollectedModifier");
		instance->coinTracingDisabledCoin = Utils::getBool("coinTracingDisabledCoin");
		instance->coinTracingThickness = Utils::getDouble("coinTracingThickness");
		instance->coinOpacityModifier = Utils::getDouble("coinOpacityModifier");
		instance->trailLengthModifier = Utils::getDouble("trailLengthModifier");
		instance->colorFromSettings = Utils::getColorAlpha("colorFromSettings");
		instance->garageInPauseMenu = Utils::getBool("garageInPauseMenu");
		instance->pulseScaleFactor = Utils::getDouble("pulseScaleFactor");
		instance->coinTraceOpacity = Utils::getInt("coinTraceOpacity");
		instance->addMinumumWidth = Utils::getBool("addMinumumWidth");
		instance->showBestPercent = Utils::getBool("showBestPercent");
		instance->customSeparator = Utils::getString("customSeparator").at(0);
		instance->pulseMenuTitle = Utils::getBool("pulseMenuTitle");
		instance->filthyGameplay = Utils::getBool("filthyGameplay");
		instance->wavePulseSize = Utils::getDouble("wavePulseSize");
		instance->noWavePulse = Utils::getBool("noWavePulse");
		instance->trailLength = Utils::getBool("trailLength");
		instance->filthyPath = Utils::getString("filthyPath", true);
		instance->traceCoins = Utils::getBool("traceCoins");
		instance->colorMode = Utils::getString("colorMode");
		instance->enabled = Utils::getBool("enabled");
		instance->filth = Utils::getBool("filth");
	}

};
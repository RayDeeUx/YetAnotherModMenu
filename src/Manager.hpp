#pragma once

// Manager.hpp structure by acaruso
// reused with explicit permission and strong encouragement

using namespace geode::prelude;

class Manager {

protected:
	static Manager* instance;
public:

	bool coinTracingOpacityModifiers = false;
	bool previouslyCollectedModifier = false;
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

	static Manager* getSharedInstance() {
		if (!instance) instance = new Manager();
		return instance;
	}

};
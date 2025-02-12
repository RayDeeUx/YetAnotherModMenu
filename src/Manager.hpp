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
	bool addMinumumWidth = false;
	bool pulseUseSTDLerp = false;
	bool filthyGameplay = false;
	bool calledAlready = false;
	bool noWavePulse = false;
	bool hasLoadedSDI = false;
	bool isPauseShop = false;
	bool trailLength = false;
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
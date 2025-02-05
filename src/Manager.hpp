#pragma once

// Manager.hpp structure by acaruso
// reused with explicit permission and strong encouragement

using namespace geode::prelude;

class Manager {

protected:
	static Manager* instance;
public:

	bool calledAlready = false;
	bool hasLoadedSDI = false;
	bool isPauseShop = false;

	double trailLengthModifier = 0.0;
	double pulseScaleFactor = 0.0;

	char customSeparator = '/';

	static Manager* getSharedInstance() {
		if (!instance) instance = new Manager();
		return instance;
	}

};
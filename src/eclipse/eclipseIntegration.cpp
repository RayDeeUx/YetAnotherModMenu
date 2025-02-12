#include "eclipse.hpp"
#include "../Manager.hpp"

using namespace eclipse;
using namespace geode::prelude;

void createToggleSettingInTab(const char* settingID, MenuTab& tab, std::string description = "") {
	auto setting = Mod::get()->getSetting(settingID);
	if (description == "USE_MOD_JSON") description = setting->getDescription().value_or("");

	tab.addToggle(Mod::get()->expandSpriteName(settingID).data(), setting->getDisplayName(), [settingID](bool booleanSettingValue) {
		Mod::get()->setSettingValue<bool>(settingID, booleanSettingValue);
		eclipse::config::set<bool>(Mod::get()->expandSpriteName(settingID).data(), booleanSettingValue);
	}).setDescription(description);

	eclipse::config::set<bool>(Mod::get()->expandSpriteName(settingID).data(), Mod::get()->getSettingValue<bool>(settingID));

	listenForSettingChanges(settingID, [settingID](bool boolValue) {
		eclipse::config::set<bool>(Mod::get()->expandSpriteName(settingID).data(), Mod::get()->getSettingValue<bool>(settingID));
	});
}

$on_mod(Loaded) {
	Loader::get()->queueInMainThread([] {
		auto tab = MenuTab::find("YetAnotherQOLMod");

		createToggleSettingInTab("enabled", tab);
		createToggleSettingInTab("showBestPercent", tab);

		createToggleSettingInTab("noWavePulse", tab);
		tab.addInputFloat(
			"wavePulseSize"_spr, "Wave Trail Width",
			[](float wavePulseSize) {
				Mod::get()->setSettingValue<double>("wavePulseSize", wavePulseSize);
				Manager::getSharedInstance()->wavePulseSize = wavePulseSize;
			}
		).setMinValue(0.f).setMaxValue(6.f).setFormat("%.2f");

		eclipse::config::setInternal<float>("wavePulseSize"_spr, Mod::get()->getSettingValue<double>("wavePulseSize"));

		listenForSettingChanges("wavePulseSize", [](double wavePulseSize) {
			Manager::getSharedInstance()->wavePulseSize = wavePulseSize;
			eclipse::config::setInternal<float>("wavePulseSize"_spr, wavePulseSize);
		});

		createToggleSettingInTab("trailLength", tab);
		tab.addInputFloat(
			"trailLengthModifier"_spr, "Trail Length Modifier",
			[](float trailLengthModifier) {
				Mod::get()->setSettingValue<double>("trailLengthModifier", trailLengthModifier);
				Manager::getSharedInstance()->trailLengthModifier = trailLengthModifier;
			}
		).setMinValue(0.f).setMaxValue(6.f).setFormat("%.2f").setDescription("The higher the number, the longer the trail. Set to 0.0 to disable, or to 1.0 to mimic vanilla behavior when inside a level.");

		eclipse::config::setInternal<float>("trailLengthModifier"_spr, Mod::get()->getSettingValue<double>("trailLengthModifier"));

		listenForSettingChanges("trailLengthModifier", [](double trailLengthModifier) {
			Manager::getSharedInstance()->trailLengthModifier = trailLengthModifier;
			eclipse::config::setInternal<float>("trailLengthModifier"_spr, trailLengthModifier);
		});

		createToggleSettingInTab("garageInPauseMenu", tab, "Made with <3 and massive help from km7.");

		createToggleSettingInTab("traceCoins", tab);
		tab.addInputFloat(
			"coinTracingThickness"_spr, "Coin Path Thickness",
			[](float coinTracingThickness) {
				Mod::get()->setSettingValue<double>("coinTracingThickness", coinTracingThickness);
				Manager::getSharedInstance()->coinTracingThickness = coinTracingThickness;
			}
		).setMinValue(0.f).setMaxValue(3.f).setFormat("%.2f");

		eclipse::config::setInternal<float>("coinTracingThickness"_spr, Mod::get()->getSettingValue<double>("coinTracingThickness"));

		listenForSettingChanges("coinTracingThickness", [](double coinTracingThickness) {
			Manager::getSharedInstance()->coinTracingThickness = coinTracingThickness;
			eclipse::config::setInternal<float>("coinTracingThickness"_spr, coinTracingThickness);
		});

		createToggleSettingInTab("pulseMenuTitle", tab, "USE_MOD_JSON");
		tab.addInputFloat(
			"pulseScaleFactor"_spr, "Pulsing Menu Node Scale Factor",
			[](float pulseScaleFactor) {
				Mod::get()->setSettingValue<double>("pulseScaleFactor", pulseScaleFactor);
				Manager::getSharedInstance()->pulseScaleFactor = pulseScaleFactor;
			}
		).setMinValue(0.f).setMaxValue(1.f).setFormat("%.2f").setDescription("The higher the number, the larger the pulsing. Adjust as needed depending on the Node ID you chose.");

		eclipse::config::setInternal<float>("pulseScaleFactor"_spr, Mod::get()->getSettingValue<double>("pulseScaleFactor"));

		listenForSettingChanges("pulseScaleFactor", [](double pulseScaleFactor) {
			Manager::getSharedInstance()->pulseScaleFactor = pulseScaleFactor;
			eclipse::config::setInternal<float>("pulseScaleFactor"_spr, pulseScaleFactor);
		});

		createToggleSettingInTab("filth", tab);
	});
}

#ifndef GEODE_IS_IOS
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;
#ifndef GEODE_IS_IOS
using namespace keybinds;
#endif

$on_mod(Loaded) {
	Manager::load();
	listenForSettingChanges("pulseScaleFactor", [](double pulseScaleFactor) {
		Manager::getSharedInstance()->pulseScaleFactor = pulseScaleFactor;
	});
	listenForSettingChanges("coinTracingThickness", [](double coinTracingThickness) {
		Manager::getSharedInstance()->coinTracingThickness = coinTracingThickness;
	});
	listenForSettingChanges("trailLengthModifier", [](double trailLengthModifier) {
		Manager::getSharedInstance()->trailLengthModifier = trailLengthModifier;
	});
	listenForSettingChanges("coinOpacityModifier", [](double coinOpacityModifier) {
		Manager::getSharedInstance()->coinOpacityModifier = coinOpacityModifier;
	});
	listenForSettingChanges("wavePulseSize", [](double wavePulseSize) {
		Manager::getSharedInstance()->wavePulseSize = wavePulseSize;
	});
	listenForSettingChanges("customSeparator", [](std::string customSeparator) {
		Manager::getSharedInstance()->customSeparator = std::move(customSeparator).at(0);
	});
	listenForSettingChanges("colorMode", [](std::string colorMode) {
		Manager::getSharedInstance()->colorMode = std::move(colorMode);
	});
	listenForSettingChanges("colorFromSettings", [](ccColor4B colorFromSettings) {
		Manager::getSharedInstance()->colorFromSettings = colorFromSettings;
	});
	listenForSettingChanges("coinTraceOpacity", [](int64_t coinTraceOpacity) {
		Manager::getSharedInstance()->coinTraceOpacity = coinTraceOpacity;
	});
	listenForSettingChanges("coinTracingOpacityModifiers", [](bool coinTracingOpacityModifiers) {
		Manager::getSharedInstance()->coinTracingOpacityModifiers = coinTracingOpacityModifiers;
	});
	listenForSettingChanges("coinTracingDisabledCoin", [](bool coinTracingDisabledCoin) {
		Manager::getSharedInstance()->coinTracingDisabledCoin = coinTracingDisabledCoin;
	});
	listenForSettingChanges("previouslyCollectedModifier", [](bool previouslyCollectedModifier) {
		Manager::getSharedInstance()->previouslyCollectedModifier = previouslyCollectedModifier;
	});
	listenForSettingChanges("garageInPauseMenu", [](bool garageInPauseMenu) {
		Manager::getSharedInstance()->garageInPauseMenu = garageInPauseMenu;
	});
	listenForSettingChanges("pulseMenuTitle", [](bool pulseMenuTitle) {
		Manager::getSharedInstance()->pulseMenuTitle = pulseMenuTitle;
	});
	listenForSettingChanges("showBestPercent", [](bool showBestPercent) {
		Manager::getSharedInstance()->showBestPercent = showBestPercent;
	});
	listenForSettingChanges("filthyGameplay", [](bool filthyGameplay) {
		Manager::getSharedInstance()->filthyGameplay = filthyGameplay;
	});
	listenForSettingChanges("addMinumumWidth", [](bool addMinumumWidth) {
		Manager::getSharedInstance()->addMinumumWidth = addMinumumWidth;
	});
	listenForSettingChanges("trailLength", [](bool trailLength) {
		Manager::getSharedInstance()->trailLength = trailLength;
	});
	listenForSettingChanges("noWavePulse", [](bool noWavePulse) {
		Manager::getSharedInstance()->noWavePulse = noWavePulse;
	});
	listenForSettingChanges("traceCoins", [](bool traceCoins) {
		Manager::getSharedInstance()->traceCoins = traceCoins;
	});
	listenForSettingChanges("enabled", [](bool enabled) {
		Manager::getSharedInstance()->enabled = enabled;
	});
	listenForSettingChanges("filth", [](bool filth) {
		Manager::getSharedInstance()->filth = filth;
	});
	listenForSettingChanges("filthyPath", [](std::filesystem::path filthyPath) {
		Manager::getSharedInstance()->filthyPath = std::move(filthyPath);
	});
	#ifndef GEODE_IS_IOS
	BindManager::get()->registerBindable({
		"open-settings"_spr,
		"Open Settings",
		"Opens the YetAnotherModMenu settings.",
		{ Keybind::create(KEY_Tab, Modifier::Shift), Keybind::create(KEY_F9, Modifier::None) },
		"Global/YetAnotherModMenu", false
	});
	new EventListener([=](InvokeBindEvent* event) {
		if (!event->isDown()) return ListenerResult::Propagate;
		// permission granted by dankmeme01 to run the superscary code:
		// https://discord.com/channels/911701438269386882/911702535373475870/1337522714205753426
		for (CCNode* node : CCArrayExt<CCNode*>(CCScene::get()->getChildren())) if (Utils::getNodeName(node) == "ModSettingsPopup") return ListenerResult::Propagate;
		const auto pl = PlayLayer::get();
		if (pl && pl->getParent() && (!pl->getParent()->getChildByType<PauseLayer>(0) || !pl->m_isPaused) && (!pl->getChildByType<EndLevelLayer>(0) && !pl->getChildByType<RetryLevelLayer>(0))) return ListenerResult::Propagate;
		openSettingsPopup(Mod::get());
		return ListenerResult::Propagate;
	}, InvokeBindFilter(nullptr, "open-settings"_spr));
	#endif
}

class PulsingNode final : public CCNode {
public:
	CCNode* nodeToModify = nullptr;
	double originalScale = 1.0;
	static PulsingNode* create() {
		auto ret = new PulsingNode();
		if (ret && ret->init()) {
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
protected:
	FMODAudioEngine* engine = nullptr;
	double forSTDLerp = 1;
	bool init() {
		if (!CCNode::init()) return false;
		this->engine = FMODAudioEngine::sharedEngine();
		this->setID("pulsing-node-for-tracking-fmod-metering"_spr);
		this->scheduleUpdate();
		return true;
	}
	void update(float dt) {
		if (!this->nodeToModify) return;
		if (!this->engine->m_metering) this->engine->enableMetering();
		const auto manager = Manager::getSharedInstance();
		if (GameManager::sharedState()->getGameVariable("0122") || !Utils::modEnabled() || !manager->pulseMenuTitle) return this->nodeToModify->setScale(this->originalScale);
		this->engine->update(dt);
		this->forSTDLerp = std::lerp(this->forSTDLerp, this->engine->m_pulse1, dt);
		const float clamped = std::clamp<float>(static_cast<float>(this->forSTDLerp), 0.f, 1.f);
		const float lerpCalculation = .85f + clamped;
		const auto finalScale = static_cast<float>(manager->pulseScaleFactor * lerpCalculation);
		this->nodeToModify->setScale(finalScale);
	}
};

class $modify(MyMenuLayer, MenuLayer) {
	static void onModify(auto& self) {
		if (Utils::isModLoaded("ninxout.redash")) (void) self.setHookPriorityAfterPost("MenuLayer::init", "ninxout.redash");
		else (void) self.setHookPriority("MenuLayer::init", -3998);
	}
	bool init() {
		if (!MenuLayer::init()) return false;

		#ifdef GEODE_IS_MOBILE
		if (CCNode* bottomMenu = this->getChildByID("bottom-menu")) Utils::addButtonToNode(bottomMenu, this, menu_selector(MyMenuLayer::onYAMMSettings));
		#endif

		FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
		std::string nodeID = Utils::getString("pulseNodeID");
		const std::string& modID = Utils::getString("pulseModID");
		if (!modID.empty() && Utils::isModLoaded(modID)) nodeID = fmt::format("{}/{}", modID, nodeID);
		CCNode* toModify = this->getChildByIDRecursive(nodeID);
		if (PulsingNode* pulsingNode = PulsingNode::create(); engine && pulsingNode && toModify) {
			if (!engine->m_metering) engine->enableMetering();
			pulsingNode->nodeToModify = toModify;
			pulsingNode->originalScale = toModify->getScale();
			this->addChild(pulsingNode);
		}

		const auto manager = Manager::getSharedInstance();
		if (manager->calledAlready) return true;
		manager->calledAlready = true;

		manager->hasLoadedSDI = Utils::isModLoaded(SDI);

		return true;
	}
	#ifdef GEODE_IS_MOBILE
	void onYAMMSettings(CCObject*) { Utils::openSettings(); }
	#endif
};

#ifdef GEODE_IS_MOBILE
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
class $modify(MyEditLevelLayer, EditLevelLayer) {
	bool init(GJGameLevel* p0) {
		if (!EditLevelLayer::init(p0)) return false;
		if (CCNode* node = getChildByIDRecursive("level-actions-menu")) Utils::addButtonToNode(node, this, menu_selector(MyEditLevelLayer::onYAMMSettings));
		return true;
	}
	void onYAMMSettings(CCObject*) { Utils::openSettings(); }
};

class $modify(MyCreatorLayer, CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init()) return false;
		if (CCNode* node = getChildByIDRecursive("bottom-left-menu")) Utils::addButtonToNode(node, this, menu_selector(MyCreatorLayer::onYAMMSettings));
		return true;
	}
	void onYAMMSettings(CCObject*) { Utils::openSettings(); }
};

class $modify(MyEditorPauseLayer, EditorPauseLayer) {
	bool init(LevelEditorLayer* lel) {
		if (!EditorPauseLayer::init(lel)) return false;
		if (CCNode* node = getChildByIDRecursive("settings-menu")) Utils::addButtonToNode(node, this, menu_selector(MyEditorPauseLayer::onYAMMSettings));
		return true;
	}
	void onYAMMSettings(CCObject*) { Utils::openSettings(); }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
	bool init(GJGameLevel* p0, bool p1) {
		if (!LevelInfoLayer::init(p0, p1)) return false;
		if (CCNode* node = getChildByIDRecursive("left-side-menu")) Utils::addButtonToNode(node, this, menu_selector(MyLevelInfoLayer::onYAMMSettings));
		return true;
	}
	void onYAMMSettings(CCObject*) { Utils::openSettings(); }
};
#endif
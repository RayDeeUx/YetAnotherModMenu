#include <Geode/modify/MenuLayer.hpp>
#include <utility>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
	listenForSettingChanges("pulseScaleFactor", [](double pulseScaleFactor) {
		Manager::getSharedInstance()->pulseScaleFactor = pulseScaleFactor;
	});
	listenForSettingChanges("trailLengthModifier", [](double trailLengthModifier) {
		Manager::getSharedInstance()->trailLengthModifier = trailLengthModifier;
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
		if (GameManager::sharedState()->getGameVariable("0122")) return this->nodeToModify->setScale(this->originalScale);
		this->engine->update(dt);
		this->forSTDLerp = lerpingAround(this->forSTDLerp, this->engine->m_pulse1, dt);
		const float clamped = std::clamp<float>(static_cast<float>(this->forSTDLerp), 0.f, 1.f);
		const float lerpCalculation = .85f + clamped;
		const auto finalScale = static_cast<float>(Manager::getSharedInstance()->pulseScaleFactor * lerpCalculation);
		this->nodeToModify->setScale(finalScale);
	}
	static double lerpingAround(const double originalValue, const double currentPulse, const double dt) {
		return currentPulse + (currentPulse - originalValue) * dt;
	}
};

class $modify(MyMenuLayer, MenuLayer) {
	static void onModify(auto& self) {
		if (Utils::isModLoaded("ninxout.redash")) (void) self.setHookPriorityAfterPost("MenuLayer::init", "ninxout.redash");
		else (void) self.setHookPriority("MenuLayer::init", -3998);
	}
	bool init() {
		if (!MenuLayer::init()) return false;

		FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
		std::string nodeID = Utils::getString("pulseNodeID");
		const std::string& modID = Utils::getString("pulseModID");
		if (!modID.empty() && Utils::isModLoaded(modID)) nodeID = fmt::format("{}/{}", modID, nodeID);
		CCNode* toModify = this->getChildByIDRecursive(nodeID);
		if (PulsingNode* pulsingNode = PulsingNode::create(); engine && pulsingNode && toModify && Utils::modEnabled() && Utils::getBool("pulseMenuTitle")) {
			if (!engine->m_metering) engine->enableMetering();
			pulsingNode->nodeToModify = toModify;
			pulsingNode->originalScale = toModify->getScale();
			this->addChild(pulsingNode);
		}

		Manager* manager = Manager::getSharedInstance();
		if (manager->calledAlready) return true;
		manager->calledAlready = true;

		manager->trailLengthModifier = Utils::getDouble("trailLengthModifier");
		manager->colorFromSettings = Utils::getColorAlpha("colorFromSettings");
		manager->pulseScaleFactor = Utils::getDouble("pulseScaleFactor");
		manager->coinTraceOpacity = Utils::getInt("coinTraceOpacity");
		manager->customSeparator = Utils::getString("customSeparator").at(0);
		manager->hasLoadedSDI = Utils::isModLoaded("weebify.separate_dual_icons");
		manager->colorMode = Utils::getString("colorMode");

		return true;
	}
};
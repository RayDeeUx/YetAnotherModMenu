#include <Geode/modify/PlayLayer.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

enum class ColorMode {
	MatchCoin,
	Custom,
	Unknown
};

enum class CoinsStatus {
	RobTop,
	Editor,
	CustomVerified,
	CustomUnverified,
	Unknown
};

class $modify(MyPlayLayer, PlayLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("PlayLayer::updateProgressbar", -3998); // DO NOT CHANGE TO -3999 PRIO!!!!!!!! decimal percentages
		(void) self.setHookPriority("PlayLayer::resetLevelFromStart", -3999);
		(void) self.setHookPriority("PlayLayer::setupHasCompleted", -3999);
		(void) self.setHookPriority("PlayLayer::resetLevel", -3999);
		(void) self.setHookPriority("PlayLayer::fullReset", -3999);
	}
	struct Fields {
		std::vector<GameObject*> coins {};
		std::vector<bool> coinCollected = {};
		std::vector<bool> coinActivatedDuringAttempt = {false, false, false};
		cocos2d::CCDrawNode* coinLines = nullptr;
		const std::unordered_map<std::string, ColorMode> colorModeSettingToEnum = {
			{"Match Coin Status", ColorMode::MatchCoin},
			{"Custom", ColorMode::Custom}
		};
		const std::unordered_map<CoinsStatus, ccColor3B> coinStatusToCocosColor = {
			{CoinsStatus::RobTop, {255, 215, 0}},
			{CoinsStatus::Editor, {255, 255, 255}},
			{CoinsStatus::CustomVerified, {255, 255, 255}},
			{CoinsStatus::CustomUnverified, {165, 110, 50}},
			{CoinsStatus::Unknown, {255, 255, 255}}
		};
		ColorMode currentColorMode = ColorMode::Unknown;
		CoinsStatus coinStatus = CoinsStatus::Unknown;
		ccColor3B coinColorToUse = {255, 0, 0};
		bool playerCanProbablyRecoverCoin = false;
	};
	ccColor4F determineSegmentColor(const bool collected, const bool passedCoin, const bool coinIsDisabled) {
		const auto manager = Manager::getSharedInstance();
		if (!Utils::modEnabled() || !manager->traceCoins) return {0, 0, 0, 255};
		const auto fields = m_fields.self();
		if (!fields) return {0, 0, 0, 255};
		const ColorMode currentMode = fields->currentColorMode;
		GLubyte opacity = manager->coinTraceOpacity;
		if (currentMode == ColorMode::Custom) opacity = manager->colorFromSettings.a;
		ccColor4B destinationColor = {255, 0, 0, opacity};
		if (manager->coinTracingOpacityModifiers) {
			if (collected && manager->previouslyCollectedModifier) destinationColor.a *= manager->coinOpacityModifier;
			if (passedCoin) destinationColor.a *= manager->coinOpacityModifier;
		}
		if (currentMode == ColorMode::Unknown || m_fields->coinStatus == CoinsStatus::Unknown || (coinIsDisabled && manager->coinTracingDisabledCoin))
			return ccc4FFromccc4B(destinationColor);
		if (currentMode == ColorMode::MatchCoin) {
			destinationColor.r = m_fields->coinColorToUse.r;
			destinationColor.g = m_fields->coinColorToUse.g;
			destinationColor.b = m_fields->coinColorToUse.b;
		} else {
			destinationColor.r = manager->colorFromSettings.r;
			destinationColor.g = manager->colorFromSettings.g;
			destinationColor.b = manager->colorFromSettings.b;
		}
		return ccc4FFromccc4B(destinationColor);
	}
	void setupHasCompleted() {
		PlayLayer::setupHasCompleted();
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->traceCoins || !m_objectLayer || !m_level) return;
		const auto fields = m_fields.self();
		if (!fields) return;
		fields->coinLines = CCDrawNode::create();
		fields->coinLines->setID("coin-tracing-node"_spr);
		fields->coinLines->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
		m_objectLayer->addChild(fields->coinLines);

		const std::unordered_map<std::string, ColorMode>& mapToFind = fields->colorModeSettingToEnum;
		const std::string& colorModeFromSettings = Manager::getSharedInstance()->colorMode;
		if (!mapToFind.contains(colorModeFromSettings)) fields->currentColorMode = ColorMode::Unknown;
		else fields->currentColorMode = mapToFind.find(colorModeFromSettings)->second;

		if (m_level->m_levelType == GJLevelType::Main) fields->coinStatus = CoinsStatus::RobTop;
		else if (m_level->m_levelID.value() == 0) fields->coinStatus = CoinsStatus::Editor;
		else if (m_level->m_coinsVerified.value() == 0) fields->coinStatus = CoinsStatus::CustomUnverified;
		else fields->coinStatus = CoinsStatus::CustomVerified;

		fields->coinColorToUse = fields->coinStatusToCocosColor.find(fields->coinStatus)->second;
	}
	void addObject(GameObject* object) {
		PlayLayer::addObject(object);
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->traceCoins) return;
		const auto fields = m_fields.self();
		if (!fields) return;
		if (!fields->playerCanProbablyRecoverCoin && m_level->isPlatformer()) fields->playerCanProbablyRecoverCoin = true;
		if (object->m_objectType != GameObjectType::UserCoin && object->m_objectType != GameObjectType::SecretCoin) {
			if (fields->playerCanProbablyRecoverCoin) return;
			if (object->m_classType == GameObjectClassType::Effect) { // T0D0: MIGHT BECOME AN ENUM AFTER FEBRUARY 18, 2025. EDIT ACCORDINGLY!!!
				const EffectGameObject* ego = static_cast<EffectGameObject*>(object);
				fields->playerCanProbablyRecoverCoin = ego->m_isReverse && !ego->m_isNoTouch && ego->m_objectType != GameObjectType::Decoration;
			}
			if (!fields->playerCanProbablyRecoverCoin) fields->playerCanProbablyRecoverCoin = object->m_objectID == 2900 || object->m_objectID == 1917;
			return;
		}
		fields->coins.push_back(object);
		bool collected = false;
		if (m_level->m_levelID.value() != 0) {
			const auto gsm = GameStatsManager::get();
			const char* coinKey = m_level->getCoinKey(static_cast<int>(fields->coins.size()));
			if (object->m_objectType == GameObjectType::UserCoin && m_level->m_coinsVerified.value() == 0) collected = gsm->hasUserCoin(coinKey);
			else if (object->m_objectType == GameObjectType::SecretCoin) collected = gsm->hasSecretCoin(coinKey);
		}
		fields->coinCollected.push_back(collected);
	}
	void postUpdate(float dt) {
		PlayLayer::postUpdate(dt);
		const auto fields = m_fields.self();
		if (!fields) return;
		if (fields->coinLines) fields->coinLines->clear();
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->traceCoins || fields->coins.empty() || fields->coinCollected.empty() || fields->coinActivatedDuringAttempt.empty()) return;
		const CCPoint positionPlayer = m_player1->getPosition();
		const CCRect playerRect = m_player1->getObjectRect();
		int i = -1;
		for (GameObject* coin : fields->coins) {
			i++;
			if (i > fields->coinCollected.size()) break;
			if (fields->coinActivatedDuringAttempt.at(i)) continue;
			const CCRect coinRect = coin->getObjectRect();
			if (playerRect.intersectsRect(coinRect) && !coin->m_isGroupDisabled) fields->coinActivatedDuringAttempt.at(i) = true;
			bool passedCoin = false;
			CCPoint positionCoin = coin->getPosition();
			if (positionCoin.x < positionPlayer.x) {
				if (fields->playerCanProbablyRecoverCoin) passedCoin = true;
				else continue; // cant get the coin anymore lol
			}
			fields->coinLines->drawSegment(positionPlayer, positionCoin, Manager::getSharedInstance()->coinTracingThickness, determineSegmentColor(fields->coinCollected.at(i), passedCoin, coin->m_isGroupDisabled));
		}
	}
	void resetLevel() {
		if (m_fields->coinActivatedDuringAttempt.empty()) {
			PlayLayer::resetLevel();
			return;
		}
		m_fields->coinActivatedDuringAttempt = {false, false, false};
		PlayLayer::resetLevel();
	}
	void resetLevelFromStart() {
		if (m_fields->coinActivatedDuringAttempt.empty()) {
			PlayLayer::resetLevelFromStart();
			return;
		}
		m_fields->coinActivatedDuringAttempt = {false, false, false};
		PlayLayer::resetLevelFromStart();
	}
	void fullReset() {
		if (m_fields->coinActivatedDuringAttempt.empty()) {
			PlayLayer::fullReset();
			return;
		}
		m_fields->coinActivatedDuringAttempt = {false, false, false};
		PlayLayer::fullReset();
	}
	void updateProgressbar() {
		PlayLayer::updateProgressbar();
		const auto manager = Manager::getSharedInstance();
		if (!Utils::modEnabled() || !manager->showBestPercent || !m_level || m_level->isPlatformer() || !m_percentageLabel) return;
		m_percentageLabel->setString(fmt::format("{} {} {}%", m_percentageLabel->getString(), Manager::getSharedInstance()->customSeparator, m_isPracticeMode ? m_level->m_practicePercent : m_level->m_normalPercent.value()).c_str());
	}
};

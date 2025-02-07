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
		(void) self.setHookPriority("PlayLayer::setupHasCompleted", -3999);
		(void) self.setHookPriority("PlayLayer::updateProgressbar", -3999);
		(void) self.setHookPriority("PlayLayer::resetLevel", -3999);
		(void) self.setHookPriority("PlayLayer::resetLevelFromStart", -3999);
	}
	struct Fields {
		std::vector<GameObject*> coins;
		std::vector<bool> coinCollected;
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
			{CoinsStatus::CustomUnverified, {255, 175, 75}},
			{CoinsStatus::Unknown, {255, 255, 255}}
		};
		ColorMode currentColorMode = ColorMode::Unknown;
		CoinsStatus coinStatus = CoinsStatus::Unknown;
		ccColor3B coinColorToUse = {255, 0, 0};
		bool playerCanProbablyRecoverCoin = false;
	};
	ccColor4F determineSegmentColor(const bool collected, const bool passedCoin) {
		if (!Utils::modEnabled() || !Utils::getBool("traceCoins")) return {0, 0, 0, 255};
		const auto fields = m_fields.self();
		Manager* manager = Manager::getSharedInstance();
		ccColor4B destinationColor = {255, 0, 0, manager->coinTraceOpacity};
		if (collected) destinationColor.a /= 2;
		if (passedCoin) destinationColor.a /= 2;
		const ColorMode currentMode = fields->currentColorMode;
		if (currentMode == ColorMode::Unknown || m_fields->coinStatus == CoinsStatus::Unknown)
			return ccc4FFromccc4B(destinationColor);
		if (currentMode == ColorMode::MatchCoin) {
			destinationColor.r = m_fields->coinColorToUse.r;
			destinationColor.g = m_fields->coinColorToUse.g;
			destinationColor.b = m_fields->coinColorToUse.b;
			return ccc4FFromccc4B(destinationColor);
		}
		return ccc4FFromccc4B(manager->colorFromSettings);
	}
	void setupHasCompleted() {
		PlayLayer::setupHasCompleted();
		if (!Utils::modEnabled() || !Utils::getBool("traceCoins") || !m_objectLayer || !m_level) return;
		m_fields->coinLines = CCDrawNode::create();
		m_fields->coinLines->setID("coin-tracing-node"_spr);
		m_fields->coinLines->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
		m_objectLayer->addChild(m_fields->coinLines);

		const std::unordered_map<std::string, ColorMode>& mapToFind = m_fields->colorModeSettingToEnum;
		const std::string& colorModeFromSettings = Manager::getSharedInstance()->colorMode;
		if (!mapToFind.contains(colorModeFromSettings)) m_fields->currentColorMode = ColorMode::Unknown;
		else m_fields->currentColorMode = mapToFind.find(colorModeFromSettings)->second;

		if (m_level->m_levelType == GJLevelType::Local) m_fields->coinStatus = CoinsStatus::RobTop;
		else if (m_level->m_levelID.value() == 0) m_fields->coinStatus = CoinsStatus::Editor;
		else if (m_level->m_coinsVerified.value() == 0) m_fields->coinStatus = CoinsStatus::CustomUnverified;
		else m_fields->coinStatus = CoinsStatus::CustomVerified;

		m_fields->coinColorToUse = m_fields->coinStatusToCocosColor.find(m_fields->coinStatus)->second;
	}
	void addObject(GameObject* object) {
		PlayLayer::addObject(object);
		if (!Utils::modEnabled() || !Utils::getBool("traceCoins")) return;
		if (!m_fields->playerCanProbablyRecoverCoin && m_level->isPlatformer()) m_fields->playerCanProbablyRecoverCoin = true;
		if (object->m_objectType != GameObjectType::UserCoin && object->m_objectType != GameObjectType::SecretCoin) {
			if (m_fields->playerCanProbablyRecoverCoin) return;
			if (const auto ego = typeinfo_cast<EffectGameObject*>(object)) m_fields->playerCanProbablyRecoverCoin = ego->m_isReverse;
			if (!m_fields->playerCanProbablyRecoverCoin) m_fields->playerCanProbablyRecoverCoin = object->m_objectID == 2900 || object->m_objectID == 1917;
			return;
		}
		m_fields->coins.push_back(object);
		const auto gsm = GameStatsManager::get();
		const char* coinKey = m_level->getCoinKey(static_cast<int>(m_fields->coins.size()));
		bool collected = true;
		if (m_level->m_levelID.value() == 0) collected = false;
		else if (object->m_objectType == GameObjectType::UserCoin) collected = gsm->hasUserCoin(coinKey);
		else if (m_level->m_coinsVerified.value() != 0) collected = gsm->hasSecretCoin(coinKey);
		m_fields->coinCollected.push_back(collected);
	}
	void postUpdate(float dt) {
		PlayLayer::postUpdate(dt);
		if (!Utils::modEnabled() || !Utils::getBool("traceCoins") || !m_fields->coinLines || m_fields->coins.empty() || m_fields->coinCollected.empty() || m_fields->coinActivatedDuringAttempt.empty()) return;
		m_fields->coinLines->clear();
		const CCPoint positionPlayer = m_player1->getPosition();
		const CCRect playerRect = m_player1->getObjectRect();
		int i = -1;
		for (GameObject* coin : m_fields->coins) {
			i++;
			if (!m_isPracticeMode) {
				const CCRect coinRect = coin->getObjectRect();
				if (const bool touchedCoin = playerRect.intersectsRect(coinRect)) m_fields->coinActivatedDuringAttempt.at(i) = true;
				if (m_fields->coinActivatedDuringAttempt.at(i)) continue;
			}
			bool passedCoin = false;
			CCPoint positionCoin = coin->getPosition();
			if (positionCoin.x < positionPlayer.x) {
				if (m_fields->playerCanProbablyRecoverCoin) passedCoin = true;
				else continue; // cant get the coin anymore lol
			}
			m_fields->coinLines->drawSegment(positionPlayer, positionCoin, 1, determineSegmentColor(m_fields->coinCollected.at(i), passedCoin));
		}
	}
	void resetLevel() {
		m_fields->coinActivatedDuringAttempt = {false, false, false};
		PlayLayer::resetLevel();
	}
	void resetLevelFromStart() {
		m_fields->coinActivatedDuringAttempt = {false, false, false};
		PlayLayer::resetLevelFromStart();
	}
	void updateProgressbar() {
		PlayLayer::updateProgressbar();
		if (!Utils::modEnabled() || !Utils::getBool("showBestPercent") || !m_level || m_level->isPlatformer() || !m_percentageLabel) return;
		m_percentageLabel->setString(fmt::format("{} {} {}%", m_percentageLabel->getString(), Manager::getSharedInstance()->customSeparator, m_isPracticeMode ? m_level->m_practicePercent : m_level->m_normalPercent.value()).c_str());
	}
};
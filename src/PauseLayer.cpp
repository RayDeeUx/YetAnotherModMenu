#include <Geode/modify/CharacterColorPage.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();
		if (!Utils::modEnabled() || !Utils::getBool("garageInPauseMenu")) return;
		CCNode* leftButtonMenu = this->getChildByID("left-button-menu");
		if (!leftButtonMenu) return;
		CircleButtonSprite* buttonSprite = CircleButtonSprite::createWithSprite("iconKitBase.png"_spr, 1, CircleBaseColor::Cyan, CircleBaseSize::SmallAlt);
		buttonSprite->setID("garage-button-sprite"_spr);
		#ifndef GEODE_IS_ANDROID64
		CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(MyPauseLayer::onYAQOLMODGarage));
		#else
		CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(LevelInfoLayer::onGarage));
		#endif
		button->setID("garage-button"_spr);
		leftButtonMenu->addChild(button);
		leftButtonMenu->updateLayout();
	}
	#ifndef GEODE_IS_ANDROID64
	void onYAQOLMODGarage(CCObject*) {
		GJGarageLayer* garage = GJGarageLayer::node();
		garage->setUserObject("from-pauselayer"_spr, CCBool::create(true));
		CCScene* currScene = CCScene::get();
		currScene->addChild(garage);
		garage->setZOrder(currScene->getHighestChildZ() + 2);
	}
	#endif
};

class $modify(MyGJGarageLayer, GJGarageLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("GJGarageLayer::onBack", -3999);
		(void) self.setHookPriority("GJGarageLayer::onSelect", -3999);
	}
	void onBack(CCObject* sender) {
		const auto pl = PlayLayer::get();
		if (!Utils::modEnabled() || !Utils::getBool("garageInPauseMenu") || !pl || !this->getUserObject("from-pauselayer"_spr)) return GJGarageLayer::onBack(nullptr);
		if (pl->getParent() && this->getParent() == pl->getParent()) this->removeMeAndCleanup();
	}
	void onSelect(CCObject* sender) {
		GJGarageLayer::onSelect(sender);
		const auto pl = PlayLayer::get();
		if (!Utils::modEnabled() || !Utils::getBool("garageInPauseMenu") || !pl) return;
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModfy();
		if (!playerToModify) return;
		const int iconID = sender->getTag();
		switch (m_selectedIconType) {
			default: return;
			case IconType::Ship:
				if (playerToModify->m_isShip && !pl->m_level->isPlatformer()) playerToModify->updatePlayerShipFrame(iconID);
				return;
			case IconType::Ball:
				if (playerToModify->m_isBall) playerToModify->updatePlayerRollFrame(iconID);
				return;
			case IconType::Ufo:
				if (playerToModify->m_isBird) playerToModify->updatePlayerBirdFrame(iconID);
				return;
			case IconType::Wave:
				if (playerToModify->m_isDart) playerToModify->updatePlayerDartFrame(iconID);
				return;
			case IconType::Robot:
				if (playerToModify->m_isRobot) playerToModify->updatePlayerRobotFrame(iconID);
				return;
			case IconType::Spider:
				if (playerToModify->m_isSpider) playerToModify->updatePlayerSpiderFrame(iconID);
				return;
			case IconType::Swing:
				if (playerToModify->m_isSwing) playerToModify->updatePlayerSwingFrame(iconID);
				return;
			case IconType::Jetpack:
				if (playerToModify->m_isShip && pl->m_level->isPlatformer()) playerToModify->updatePlayerJetpackFrame(iconID);
				return;
			case IconType::Cube:
				return playerToModify->updatePlayerFrame(iconID);
		}
	}
};

class $modify(MyCharacterColorPage, CharacterColorPage) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("CharacterColorPage::onPlayerColor", -3999);
		(void) self.setHookPriority("CharacterColorPage::toggleGlow", -3999);
	}
	void onPlayerColor(CCObject* sender) {
		CharacterColorPage::onPlayerColor(sender);
		if (!Utils::modEnabled() || !Utils::getBool("garageInPauseMenu") || !PlayLayer::get()) return;
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModfy();
		if (!playerToModify) return;
		const auto color = GameManager::get()->colorForIdx(sender->getTag());
		switch (m_colorMode) {
			default: return;
			case 0: playerToModify->setColor(color);
			case 1: playerToModify->setSecondColor(color);
			case 2: playerToModify->setGlowColor(color);
		}
	}
	void toggleGlow(CCObject* sender) {
		CharacterColorPage::toggleGlow(sender);
		if (!Utils::modEnabled() || !Utils::getBool("garageInPauseMenu") || !PlayLayer::get()) return;
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModfy();
		if (!playerToModify) return;
		playerToModify->m_hasGlow = static_cast<CCMenuItemToggler*>(sender)->isToggled();
		playerToModify->updatePlayerGlow();
	}
};

#ifdef GEODE_IS_ANDROID64
/*
TL;DR: PlayLayer::onEnter() does not exist.
GJGarageLayer::node() also does not exist on Android 64-bit.
This was the most appropriate solution.
See https://discord.com/channels/911701438269386882/911702535373475870/1336201672740831333
-- Erymanthus | RayDeeUx
*/
#include <Geode/modify/CCLayer.hpp>
class $modify(MyCCLayer, CCLayer) {
	virtual void onEnter() {
		PlayLayer* pl = PlayLayer::get();
		if (!pl) return CCLayer::onEnter();
		if (this != static_cast<CCLayer*>(pl)) return CCLayer::onEnter();
		if (!pl->m_isPaused) return CCLayer::onEnter();
		if (!Utils::modEnabled() || !Utils::getBool("garageInPauseMenu")) return CCLayer::onEnter();
	}
};
#endif
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
		CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(MyPauseLayer::onYAQOLMODGarage));
		button->setID("garage-button"_spr);
		leftButtonMenu->addChild(button);
		leftButtonMenu->updateLayout();
	}
	#ifdef GEODE_IS_ANDROID64
	static GJGarageLayer* makeGJGarageLayerNode() {
		auto ret = new GJGarageLayer();
		if (ret && ret->init()) {
			ret->autorelease();
			return ret;
		}
		delete ret;
		return nullptr;
	}
	#endif
	void onYAQOLMODGarage(CCObject*) {
		#ifdef GEODE_IS_ANDROID64
		GJGarageLayer* garage = MyPauseLayer::makeGJGarageLayerNode();
		#else
		GJGarageLayer* garage = GJGarageLayer::node();
		#endif
		if (!garage) return FLAlertLayer::create("Oh no!", "You're unable to access the Icon Kit!", "Close")->show();
		garage->setUserObject("from-pauselayer"_spr, CCBool::create(true));
		CCScene* currScene = CCScene::get();
		currScene->addChild(garage);
		garage->setZOrder(currScene->getHighestChildZ() + 2);
	}
};

class $modify(MyGJGarageLayer, GJGarageLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("GJGarageLayer::onBack", -3999);
		(void) self.setHookPriority("GJGarageLayer::onSelect", -3999);
	}
	void onBack(CCObject* sender) {
		const auto pl = PlayLayer::get();
		if (!Utils::modEnabled() || !Utils::getBool("garageInPauseMenu") || !pl || !this->getUserObject("from-pauselayer"_spr)) return GJGarageLayer::onBack(nullptr);
		#ifndef GEODE_IS_ANDROID64
		if (pl->getParent() && this->getParent() == pl->getParent()) this->removeMeAndCleanup();
		#else
		CCDirector::get()->popScene();
		#endif
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
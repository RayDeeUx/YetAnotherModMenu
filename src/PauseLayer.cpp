#include <Geode/modify/CharacterColorPage.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/GJShopLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("PauseLayer::onResume", -3999);
	}
	void customSetup() {
		PauseLayer::customSetup();
		CCNode* leftButtonMenu = this->getChildByID("left-button-menu");
		if (!leftButtonMenu) return;
		#ifdef GEODE_IS_ANDROID
		Utils::addButtonToNode(leftButtonMenu, this, menu_selector(MyPauseLayer::onYAQOLMODSettings));
		#endif
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu) return;
		CircleButtonSprite* buttonSprite = CircleButtonSprite::createWithSprite("iconKitBase.png"_spr, 1, CircleBaseColor::Cyan, CircleBaseSize::SmallAlt);
		buttonSprite->setID("garage-button-sprite"_spr);
		CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(MyPauseLayer::onYAQOLMODGarage));
		button->setID("garage-button"_spr);
		leftButtonMenu->addChild(button);
		leftButtonMenu->updateLayout();
	}
	#ifdef GEODE_IS_ANDROID
	void onYAQOLMODSettings(CCObject*) { Utils::openSettings(); }
	#endif
	void onYAQOLMODGarage(CCObject*) {
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu) return;
		#ifdef GEODE_IS_ANDROID64
		const auto dummyScene = GJGarageLayer::scene();
		GJGarageLayer* garage = typeinfo_cast<GJGarageLayer*>(dummyScene->getChildByID("GJGarageLayer"));
		if (!garage) return FLAlertLayer::create("Oh no!", "You're unable to access the Icon Kit!", "Close")->show();
		dummyScene->removeChild(garage);
		dummyScene->release();
		#else
		GJGarageLayer* garage = GJGarageLayer::node();
		if (!garage) return FLAlertLayer::create("Oh no!", "You're unable to access the Icon Kit!", "Close")->show();
		#endif
		
		// fake bounce in transition
		garage->setPosition({0, CCDirector::get()->getWinSize().height});
		garage->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.5f, {0, 0})));

		garage->setUserObject("from-pauselayer"_spr, CCBool::create(true));
		CCScene* currScene = CCScene::get();
		currScene->addChild(garage);

		this->setUserObject("inside-backrooms"_spr, CCBool::create(true));
		this->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.5f, {0, 1000}))); // move it to the backrooms for hacky touch prio fix
		// this->setPosition({0, 1000}); // original function call from km7

		garage->setZOrder(currScene->getHighestChildZ() + 2);
	}
	// could not figure out disabling the specific keybind for the life of me
	void onResume(CCObject* sender) {
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || (!CCScene::get()->getChildByType<GJGarageLayer>(0) && !CCScene::get()->getChildByType<GJShopLayer>(0))) return PauseLayer::onResume(sender);
	}
};

class $modify(MyGJGarageLayer, GJGarageLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("GJGarageLayer::onBack", -3999);
		(void) self.setHookPriority("GJGarageLayer::onShop", -3999);
		(void) self.setHookPriority("GJGarageLayer::onSelect", -3999);
	}
	void onBack(CCObject* sender) {
		const auto pl = PlayLayer::get();
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !pl || !this->getUserObject("from-pauselayer"_spr)) return GJGarageLayer::onBack(sender);
		if (pl->getParent() && this->getParent() != pl->getParent()) return GJGarageLayer::onBack(sender);
		// fake move up transition
		if (auto* pause = typeinfo_cast<PauseLayer*>(this->getParent()->getChildByID("PauseLayer")); pause && pause->getUserObject("inside-backrooms"_spr))
			pause->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.25f, {0, 0})));
		this->runAction(CCSequence::createWithTwoActions(
			CCMoveTo::create(0.25f, {0, CCDirector::get()->getWinSize().height}),
			CCCallFunc::create(this, callfunc_selector(GJGarageLayer::removeFromParent)))
		);
	}
	void onShop(CCObject *sender) {
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !PlayLayer::get() || !this->getUserObject("from-pauselayer"_spr)) return GJGarageLayer::onShop(sender);
		Manager* manager = Manager::getSharedInstance();
		manager->isPauseShop = true;
		GJShopLayer *shop = GJShopLayer::create(ShopType::Normal);
		manager->isPauseShop = false;
		
		if (!shop) return FLAlertLayer::create("Oh no!", "You're unable to access the Shop!", "Close")->show();
		
		// fake bounce in transition
		shop->setPosition({0, CCDirector::get()->getWinSize().height});
		shop->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.5f, {0, 0})));

		shop->setUserObject("from-pauselayer"_spr, CCBool::create(true));
		CCScene* currScene = CCScene::get();
		currScene->addChild(shop);
		shop->setZOrder(currScene->getHighestChildZ() + 2);
	}
	void onSelect(CCObject* sender) {
		GJGarageLayer::onSelect(sender);
		const auto pl = PlayLayer::get();
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !pl || !this->getUserObject("from-pauselayer"_spr)) return;
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModfy();
		if (!playerToModify) return;
		playerToModify->updateGlowColor();
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
				if (playerToModify->m_isBall || playerToModify->m_isDart || playerToModify->m_isRobot || playerToModify->m_isSpider || playerToModify->m_isSwing) return;
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
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !PlayLayer::get()) return;
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModfy();
		if (!playerToModify) return;
		const auto color = GameManager::get()->colorForIdx(sender->getTag());
		switch (m_colorMode) {
			default: return;
			case 0: return playerToModify->setColor(color);
			case 1: return playerToModify->setSecondColor(color);
			case 2:
				playerToModify->enableCustomGlowColor(color);
				return playerToModify->updateGlowColor();
		}
	}
	void toggleGlow(CCObject* sender) {
		CharacterColorPage::toggleGlow(sender);
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !PlayLayer::get()) return;
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModfy();
		if (!playerToModify) return;
		playerToModify->m_hasGlow = static_cast<CCMenuItemToggler*>(sender)->isToggled();
		playerToModify->updatePlayerGlow();
	}
};

class $modify(MyGJShopLayer, GJShopLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("GJShopLayer::onBack", -3999);
	}
	void onBack(CCObject* sender) {
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !PlayLayer::get() || !this->getUserObject("from-pauselayer"_spr)) return GJShopLayer::onBack(sender);
		// fake move up transition
		this->runAction(CCSequence::createWithTwoActions(
			CCMoveTo::create(0.25f, {0, CCDirector::get()->getWinSize().height}),
			CCCallFunc::create(this, callfunc_selector(GJShopLayer::removeFromParent)))
		);
	}
};

class $modify(MyFMODAudioEngine, FMODAudioEngine) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("FMODAudioEngine::playMusic", -3999);
	}
	void playMusic(gd::string path, bool shouldLoop, float fadeInTime, int channel) {
		if (Utils::modEnabled() && Manager::getSharedInstance()->garageInPauseMenu && Manager::getSharedInstance()->isPauseShop) return;
		FMODAudioEngine::playMusic(path, shouldLoop, fadeInTime, channel);
	}
};
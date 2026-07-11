#include <Geode/modify/CharacterColorPage.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/GJShopLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class GaragePopup final : public Popup {
	protected:
		bool init() override {
			const CCSize ws = CCDirector::get()->getWinSize();
			if (!geode::Popup::init(ws.width, ws.height)) return false;

			m_garageLayer = GJGarageLayer::node();
			if (!m_garageLayer) return false;
			m_garageLayer->setUserObject("from-pauselayer"_spr, CCBool::create(true));
			m_garageLayer->setUserFlag("from-pauselayer"_spr, true);

			this->addChild(m_garageLayer);
			this->setID("GaragePopup"_spr);
			this->setOpacity(0);
			this->m_mainLayer->setVisible(false);

			auto exitMenu = m_garageLayer->getChildByID("back-menu");
			if (!exitMenu) return false;
			if (auto closeBtn = static_cast<CCMenuItemSpriteExtra*>(exitMenu->getChildByID("back-button"))) {
				closeBtn->m_pfnSelector = menu_selector(GaragePopup::onClose);
				closeBtn->m_pListener = this;
			} else {
				return false;
			}

			this->setUserFlag("undefined0.draggable-popups/undraggable-popup", true);
			this->setUserFlag("chs000.customizepopupanimation/dont-animate", true);

			return true;
		}
		void transitionFinished() {
			this->removeMeAndCleanup();
			if (PauseLayer* pause = CCScene::get()->getChildByType<PauseLayer>(0)) {
				pause->setKeyboardEnabled(true);
				pause->setKeypadEnabled(true);
			}
		}
	public:
		static GaragePopup* create() {
			auto ret = new GaragePopup();
			if (ret && ret->init()) {
				ret->autorelease();
				return ret;
			}
			delete ret;
			return nullptr;
		}
		void onClose(CCObject* sender) override {
			this->setKeyboardEnabled(false);
			this->setKeypadEnabled(false);
			m_garageLayer->stopAllActions();
			m_garageLayer->runAction(CCSequence::createWithTwoActions(
				CCMoveTo::create(0.25, {0, CCDirector::sharedDirector()->getWinSize().height + 5}),
				CCCallFunc::create(this, callfunc_selector(GaragePopup::transitionFinished))
			));
		}
		void show() override {
			if (m_noElasticity) return Popup::show();
			m_noElasticity = true;
			Popup::show();
			m_mainLayer->setVisible(false);
			m_noElasticity = false;
			m_garageLayer->setPosition({0, CCDirector::sharedDirector()->getWinSize().height + 5});
			m_garageLayer->stopAllActions();
			m_garageLayer->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.5, {0, 0})));
		}

		GJGarageLayer* m_garageLayer {};
};

class $modify(MyPauseLayer, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();
		Manager::getSharedInstance()->originalSceneEnumValue = GameManager::get()->m_sceneEnum;
		CCNode* leftButtonMenu = this->getChildByID("left-button-menu");
		if (!leftButtonMenu) return;
		#ifdef GEODE_IS_MOBILE
		Utils::addButtonToNode(leftButtonMenu, this, menu_selector(MyPauseLayer::onYAMMSettings));
		#endif
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu) return;
		CircleButtonSprite* buttonSprite = CircleButtonSprite::createWithSprite("iconKitBase.png"_spr, 1, CircleBaseColor::Cyan, CircleBaseSize::SmallAlt);
		buttonSprite->setID("garage-button-sprite"_spr);
		CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(MyPauseLayer::onYAMMGarage));
		button->setID("garage-button"_spr);
		leftButtonMenu->addChild(button);
		leftButtonMenu->updateLayout();
	}
	#ifdef GEODE_IS_MOBILE
	void onYAMMSettings(CCObject*) { Utils::openSettings(); }
	#endif
	void onYAMMGarage(CCObject*) {
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu) return;
		if (GaragePopup* garage = GaragePopup::create()) {
			garage->show();
			this->setKeyboardEnabled(false);
			this->setKeypadEnabled(false);
			GameManager::get()->m_sceneEnum = Manager::getSharedInstance()->originalSceneEnumValue;
		} else {
			FLAlertLayer::create("Oh no!", "You're unable to access the Icon Kit!", "Close")->show();
		}
	}
};

class $modify(MyGJGarageLayer, GJGarageLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("GJGarageLayer::onShop", -3999);
		(void) self.setHookPriority("GJGarageLayer::onSelect", -3999);
	}
	void onShop(CCObject *sender) {
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !PlayLayer::get() || !this->getUserObject("from-pauselayer"_spr)) return GJGarageLayer::onShop(sender);
		const auto manager = Manager::getSharedInstance();
		manager->isPauseShop = true;
		GJShopLayer *shop = GJShopLayer::create(ShopType::Normal);
		manager->isPauseShop = false;

		if (!shop) return FLAlertLayer::create("Oh no!", "You're unable to access the Shop!", "Close")->show();

		// fake bounce in transition
		shop->stopAllActions();
		shop->setPosition({0, CCDirector::get()->getWinSize().height});
		shop->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.5f, {0, 0})));

		shop->setUserObject("from-pauselayer"_spr, CCBool::create(true));
		shop->setUserFlag("from-pauselayer"_spr, true);
		CCScene* currScene = CCScene::get();
		currScene->addChild(shop);
		shop->setZOrder(currScene->getHighestChildZ() + 2);
	}
	void onSelect(CCObject* sender) {
		GJGarageLayer::onSelect(sender);
		const auto pl = PlayLayer::get();
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !pl || !this->getUserObject("from-pauselayer"_spr)) return;
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModify();
		if (!playerToModify) return;
		// sure, playerToModify->m_iconSprite->getScale() could be .6f in other scenarios,
		// but the only time it's used is if playerToModify->m_isShip is true at all, so this is fine
		// thank you alphalaneous for the decomp insight
		const bool isJetpack = playerToModify->m_isPlatformer || (playerToModify->m_iconSprite && playerToModify->m_iconSprite->getScale() == .6f);
		playerToModify->updateGlowColor();
		GameManager* gm = GameManager::get();
		const int iconID = sender->getTag();
		switch (m_selectedIconType) {
			default: return;
			case IconType::Ship:
				if (playerToModify->m_isShip && !isJetpack) playerToModify->updatePlayerShipFrame(iconID);
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
				if (playerToModify->m_isShip && isJetpack) playerToModify->updatePlayerJetpackFrame(iconID);
				return;
			case IconType::Cube:
				if (playerToModify->m_isBall || playerToModify->m_isDart || playerToModify->m_isRobot || playerToModify->m_isSpider || playerToModify->m_isSwing) return;
				return playerToModify->updatePlayerFrame(iconID);
			case IconType::DeathEffect:
				return gm->loadDeathEffect(iconID);
			case IconType::ShipFire:
				if (playerToModify->m_isShip && !isJetpack) playerToModify->m_shipStreakType = static_cast<ShipStreak>(iconID);
				return;
			// can i be real with yall for a second here? i lowk dont know what the fuck i was (and/or am) doing here, but it seems to work i guess?
			// [pretty sure part of this was me re-reading how jasmine did it for more icons]
			case IconType::Special:
				const bool isAlwaysShow = iconID == 5 || iconID == 6;
				const bool isImage = iconID == 2 || iconID == 7;
				playerToModify->resetStreak();
				playerToModify->m_playerStreak = iconID;
				playerToModify->setupStreak();
				playerToModify->m_alwaysShowStreak = isAlwaysShow;
				if (!isImage) {
					playerToModify->m_regularTrail->setColor(gm->colorForIdx(gm->m_playerColor2.value()));
				}
				if (isAlwaysShow || playerToModify->isFlying()) {
					playerToModify->m_regularTrail->resumeStroke();
				}
				return;
		}
	}
	// it just dawned on me i should've been hooking this all along ;-; im such a fraud
	void onToggleItem(CCObject* sender) {
		GJGarageLayer::onToggleItem(sender);
		const auto pl = PlayLayer::get();
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !pl || !this->getUserObject("from-pauselayer"_spr)) return;
		// i have no idea if separate dual icons ever tracks animations separately for robot/spider animations and i'm not taking the risk of finding out the hard way
		PlayerObject* playerOne = pl->m_player1;
		PlayerObject* playerTwo = pl->m_player2;
		if (!playerOne || !playerTwo) return;
		const int animationID = sender->getTag();
		if (animationID < 18 || animationID > 20) return; // focus on the three anims for now
		const bool wasAnimationIDEnabled = GameStatsManager::get()->isItemEnabled(UnlockType::Item, animationID);
		switch (animationID) {
			default: return;
			case 18: {
				playerOne->m_robotAnimation1Enabled = wasAnimationIDEnabled;
				playerTwo->m_robotAnimation1Enabled = wasAnimationIDEnabled;
				return;
			}
			case 19: {
				playerOne->m_robotAnimation2Enabled = wasAnimationIDEnabled;
				playerTwo->m_robotAnimation2Enabled = wasAnimationIDEnabled;
				return;
			}
			case 20: {
				playerOne->m_spiderAnimationEnabled = wasAnimationIDEnabled;
				playerTwo->m_spiderAnimationEnabled = wasAnimationIDEnabled;
				return;
			}
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
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModify();
		if (!playerToModify) return;
		const auto color = GameManager::get()->colorForIdx(sender->getTag());
		switch (m_colorMode) {
			default: return;
			case 0: return playerToModify->setColor(color);
			case 1:
				if (playerToModify->m_playerStreak == 5 || playerToModify->m_playerStreak == 6) playerToModify->m_regularTrail->setColor(color);
				return playerToModify->setSecondColor(color);
			case 2:
				playerToModify->enableCustomGlowColor(color);
				return playerToModify->updateGlowColor();
		}
	}
	void toggleGlow(CCObject* sender) {
		CharacterColorPage::toggleGlow(sender);
		if (!Utils::modEnabled() || !Manager::getSharedInstance()->garageInPauseMenu || !PlayLayer::get()) return;
		PlayerObject* playerToModify = Utils::getSelectedPlayerObjectToModify();
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
		this->stopAllActions();
		this->runAction(CCSequence::createWithTwoActions(
			CCMoveTo::create(0.25f, {0, CCDirector::get()->getWinSize().height + 5}),
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
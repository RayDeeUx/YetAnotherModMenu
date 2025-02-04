#include <Geode/modify/PlayLayer.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("PlayLayer::updateProgressbar", -3999);
	}
	void updateProgressbar() {
		PlayLayer::updateProgressbar();
		if (!Utils::modEnabled() || !Utils::getBool("showBestPercent") || !m_level || m_level->isPlatformer() || !m_percentageLabel) return;
		m_percentageLabel->setString(fmt::format("{} {} {}%", m_percentageLabel->getString(), Manager::getSharedInstance()->customSeparator, m_isPracticeMode ? m_level->m_practicePercent : m_level->m_normalPercent.value()).c_str());
	}
};
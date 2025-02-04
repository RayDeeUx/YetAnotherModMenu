#include <Geode/modify/PlayerObject.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

enum class BlendMode {
	Disabled,
	MoreIcons,
	SolidQOLMod,
	Solid,
	Invert,
	Additive,
};

class $modify(MyPlayerObject, PlayerObject) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("PlayerObject::setupStreak", -3999);
	}
	struct Fields {
		const std::unordered_map<std::string, BlendMode> blendFuncsSettingToEnum = {
			{"Disabled", BlendMode::Disabled},
			{"hiimmoreicons", BlendMode::MoreIcons},
			{"TheSolidQOLMod", BlendMode::SolidQOLMod},
			{"Solidmanthus", BlendMode::Solid},
			{"Invertmanthus", BlendMode::Invert},
			{"Additive", BlendMode::Additive},
		};
	};
	static ccBlendFunc getBlendFunction(const BlendMode blendMode) {
		switch (blendMode) {
			default: [[fallthrough]];
			case BlendMode::MoreIcons:
				return ccBlendFunc { GL_SRC_ALPHA, GL_ONE };
			case BlendMode::Solid:
				return ccBlendFunc { GL_ONE, GL_SRC_ALPHA };
			case BlendMode::Invert:
				return ccBlendFunc { GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA };
			case BlendMode::Additive:
				return ccBlendFunc { GL_ONE, GL_ONE };
		}
	}
	BlendMode getBlendMode(const std::string& blendingTrailMode) {
		const std::unordered_map<std::string, BlendMode> mapToUse = m_fields->blendFuncsSettingToEnum;
		if (!mapToUse.contains(blendingTrailMode)) return BlendMode::Disabled;
		return mapToUse.find(blendingTrailMode)->second;
	}
	void setupStreak() {
		PlayerObject::setupStreak();

		if (Manager::getSharedInstance()->trailLengthModifier > 0.0f)
			m_regularTrail->setUserObject("qualified-for-elongation"_spr, CCBool::create(true));

		const BlendMode blendMode = MyPlayerObject::getBlendMode(Utils::getString("blendingTrailMode"));
		if (blendMode == BlendMode::Disabled) return;

		if (blendMode == BlendMode::SolidQOLMod)
			return this->m_regularTrail->setBlendFunc(this->getBlendFunc());

		const ccBlendFunc blendFunc = MyPlayerObject::getBlendFunction(blendMode);
		this->m_regularTrail->setBlendFunc(blendFunc);
	}
};
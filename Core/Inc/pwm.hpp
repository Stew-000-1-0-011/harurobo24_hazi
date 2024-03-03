#pragma once

#include "tim.h"
#include <CRSLibtmp/std_type.hpp>

namespace stew::harurobo24::hazi::pwm::impl {
	using namespace CRSLib::IntegerTypes;

	struct PwmHtim {
		TIM_HandleTypeDef * htim;

		static auto make(TIM_HandleTypeDef * const htim) noexcept -> PwmHtim {
			HAL_TIM_PWM_Init(htim);
			return PwmHtim{htim};
		}
	};
	
	struct Pwm {
		PwmHtim htim;
		u32 channel;

		static auto make(const PwmHtim& htim, const u32 channel) noexcept {
			
			return Pwm{htim, channel};
		}

		void start() const noexcept {
			HAL_TIM_PWM_Start(htim.htim, channel);
		}

		void stop() const noexcept {
			HAL_TIM_PWM_Stop(htim.htim, channel);
		}

		void set_ccr(const u16 ccr) const noexcept {
			__HAL_TIM_SET_COMPARE(htim.htim, channel, ccr);
		}
	};
}

namespace stew::harurobo24::hazi::pwm {
	using impl::Pwm;
	using impl::PwmHtim;
}

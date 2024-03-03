#pragma once

#include <CRSLibtmp/std_type.hpp>
#include "pwm.hpp"

namespace stew::harurobo24::hazi::servo::impl {
	using namespace CRSLib::IntegerTypes;
	using pwm::Pwm;
	
	struct Servo {
		Pwm pwm;
		u16 close_ccr;
		u16 open_ccr;
		bool is_open;

		static auto make(Pwm&& pwm, const u16 close_ccr, const u16 open_ccr) noexcept -> Servo {
			pwm.start();
			return Servo{pwm, close_ccr, open_ccr, true};
		}

		void close() noexcept {
			is_open = false;
			pwm.set_ccr(close_ccr);
		}

		void open() noexcept {
			is_open = true;
			pwm.set_ccr(open_ccr);
		}

		void toggle() noexcept {
			if(is_open) {
				close();
			} else {
				open();
			}
		}
	};
}

namespace stew::harurobo24::hazi::servo {
	using impl::Servo;
}

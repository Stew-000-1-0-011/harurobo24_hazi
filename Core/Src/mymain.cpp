#include <optional>
#include <array>
#include <type_traits>
#include <can.h>
#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/Can/Stm32/RM0008/register_map.hpp>
#include <CRSLibtmp/Can/Stm32/RM0008/can_bus.hpp>
#include <CRSLibtmp/Can/Stm32/RM0008/filter_manager.hpp>
#include <pwm.hpp>
#include <servo.hpp>

namespace stew::harurobo24::hazi::mymain::impl {
	using namespace CRSLib::IntegerTypes;
	using pwm::Pwm;
	using pwm::PwmHtim;
	using servo::Servo;
	namespace RM0008 = CRSLib::Can::Stm32::RM0008;

	std::optional<std::array<Servo, 3>> hazis{std::nullopt};

	void mymain_inner() noexcept {
		/////////////////////////////// サーボの初期化
		hazis = std::array<Servo, 3> {
			Servo::make(Pwm::make(PwmHtim::make(&htim2), TIM_CHANNEL_1), 28500, 37000)
			, Servo::make(Pwm::make(PwmHtim::make(&htim2), TIM_CHANNEL_2), 40000, 50000)
			, Servo::make(Pwm::make(PwmHtim::make(&htim2), TIM_CHANNEL_3), 15000, 55000)
		};

		/////////////////////////////// CANの初期化
		HAL_CAN_DeInit(&hcan);
		HAL_CAN_MspInit(&hcan);

		RM0008::CanBus can_bus{RM0008::can1};

		RM0008::FilterManager::initialize_you_dont_have_to_think_much (
			{
				{0x20F, RM0008::Fifo::Fifo0}  // Enable/Disable
				, {0x211, RM0008::Fifo::Fifo0}  // A0
				, {0x212, RM0008::Fifo::Fifo0}  // A1
				, {0x213, RM0008::Fifo::Fifo0}  // A2
			}
		);
		bool is_enable = false;

		can_bus.start();

		/////////////////////////////// メインループ
		u32 last_blink = HAL_GetTick();

		while(true) {
			const auto now = HAL_GetTick();
			if(const auto message = can_bus.receive(RM0008::Fifo::Fifo0); message) {
				if(now - last_blink > 20u) {
					HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
					last_blink = now;
				}

				switch(message->id) {
					case 0x20F:
						if(message->data.buffer[0] == (byte)1) {
							is_enable = true;
						}
						else {
							is_enable = false;
						}
						break;

					case 0x211:
					case 0x212:
					case 0x213:
						if(is_enable) {
							const auto servo_index = message->id - 0x211;
							if(message->data.buffer[0] == (byte)1) {
								(*hazis)[servo_index].open();
							}
							else {
								(*hazis)[servo_index].close();
							}
						}
						break;
					default:;
				}
			}

			if(now - last_blink > 20u) {
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
			}
		}

//		volatile unsigned int dummy = 0;
//		while(true) {
//			dummy = dummy + 1;
//			(*hazis)[1].toggle();
//
//		}
	}
}

volatile auto p = &stew::harurobo24::hazi::mymain::impl::hazis;

extern "C" void mymain() noexcept {
	stew::harurobo24::hazi::mymain::impl::mymain_inner();
}

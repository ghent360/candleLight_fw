/*

The MIT License (MIT)

Copyright (c) 2023 Pengutronix,
              Marc Kleine-Budde <kernel@pengutronix.de>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "board.h"
#include "config.h"
#include "device.h"
#include "gpio.h"
#include "led.h"
#include "usbd_gs_can.h"

static void mks_utc_setup(USBD_GS_CAN_HandleTypeDef *hcan)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	UNUSED(hcan);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/* LEDs */

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_INIT_STATE(1));
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* FDCAN */
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {
		.PeriphClockSelection = RCC_PERIPHCLK_FDCAN,
		.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL,
	};

	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
	__HAL_RCC_FDCAN_CLK_ENABLE();

	/* FDCAN1_RX, FDCAN1_TX */
	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF3_FDCAN1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void
mks_utc_phy_power_set(can_data_t *channel, bool enable)
{
    UNUSED(channel);
    UNUSED(enable);
}

static void
mks_utc_termination_set(can_data_t *channel,
					    enum gs_can_termination_state enable)
{
    UNUSED(channel);
    UNUSED(enable);
}

static void
mks_utc_delay_config(can_data_t *channel)
{
	HAL_FDCAN_DisableTxDelayCompensation(&channel->channel);
}

const struct BoardConfig config = {
	.setup = mks_utc_setup,
	.phy_power_set = mks_utc_phy_power_set,
	.termination_set = mks_utc_termination_set,
	.delay_config = mks_utc_delay_config,
	.channels[0] = {
		.interface = FDCAN1,
		.leds = {
			[LED_RX] = {
				.port = GPIOD,
				.pin = GPIO_PIN_0,
				.active_high = 1,
			},
			[LED_TX] = {
				.port = GPIOD,
				.pin = GPIO_PIN_0,
				.active_high = 1,
			}
		},
	},
};

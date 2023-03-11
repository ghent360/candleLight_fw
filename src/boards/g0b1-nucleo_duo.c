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

static void nucleo_duo_setup(USBD_GS_CAN_HandleTypeDef *hcan)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	UNUSED(hcan);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/* LEDs & nCANSTBY pins*/

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_INIT_STATE(0));
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* FDCAN */

	RCC_PeriphCLKInitTypeDef PeriphClkInit = {
		.PeriphClockSelection = RCC_PERIPHCLK_FDCAN,
		.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL,
	};

	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
	__HAL_RCC_FDCAN_CLK_ENABLE();

	/* FDCAN1_RX, FDCAN1_TX */
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF3_FDCAN1;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* FDCAN2_RX, FDCAN2_TX */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_FDCAN2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void
nucleo_duo_phy_power_set(can_data_t *channel, bool enable)
{
    UNUSED(channel);
    UNUSED(enable);
}

static void
nucleo_duo_termination_set(can_data_t *channel,
					    enum gs_can_termination_state enable)
{
    UNUSED(channel);
    UNUSED(enable);
}

static void
nucleo_duo_delay_config(can_data_t *channel, bool fd_mode)
{
    if (fd_mode)
    {
        // SIT1051TK/T3 requires delay compensation above 4Mbps
        uint16_t sampling_point = channel->channel.Init.DataTimeSeg1 + 1;
        HAL_FDCAN_ConfigTxDelayCompensation(&channel->channel, sampling_point, 2);
        HAL_FDCAN_EnableTxDelayCompensation(&channel->channel);
    } else {
        HAL_FDCAN_DisableTxDelayCompensation(&channel->channel);
    }
}

const struct BoardConfig config = {
	.setup = nucleo_duo_setup,
	.phy_power_set = nucleo_duo_phy_power_set,
	.termination_set = nucleo_duo_termination_set,
	.delay_config = nucleo_duo_delay_config,
	.channels[0] = {
		.interface = FDCAN1,
		.leds = {
			[LED_RX] = {
				.port = GPIOA,
				.pin = GPIO_PIN_5,
				.active_high = 0,
			},
			[LED_TX] = {
				.port = GPIOA,
				.pin = GPIO_PIN_5,
				.active_high = 0,
			}
		},
	},
	.channels[1] = {
		.interface = FDCAN2,
		.leds = {
			[LED_RX] = {
				.port = GPIOA,
				.pin = GPIO_PIN_5,
				.active_high = 0,
			},
			[LED_TX] = {
				.port = GPIOA,
				.pin = GPIO_PIN_5,
				.active_high = 0,
			}
		},
	},
};

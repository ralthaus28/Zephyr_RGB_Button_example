/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
static const struct pwm_dt_spec pwm_led1 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led1));
static const struct pwm_dt_spec pwm_led2 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led2));
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
							      {0});
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios,
						     {0});

#define MIN_PERIOD 	PWM_SEC(1U) / 128U
#define PWM_step	0.1

void main(void)
{
	float dcl = 0;
	float stp = PWM_step;
	uint8_t col = 0;

	printk("PWM-based blinky\n");

	if (!device_is_ready(pwm_led0.dev) || !device_is_ready(pwm_led1.dev) || !device_is_ready(pwm_led2.dev)) {
		printk("Error: PWM device is not ready\n");
		return;
	}

	gpio_pin_configure_dt(&button, GPIO_INPUT);
	gpio_pin_configure_dt(&led, GPIO_OUTPUT);

	while (1) {
		if (col == 2){pwm_set_dt(&pwm_led2, MIN_PERIOD, MIN_PERIOD * dcl);}
		else{pwm_set_dt(col ? &pwm_led1 : &pwm_led0, MIN_PERIOD, MIN_PERIOD * dcl);}

		if(dcl >= 1)
		{
			dcl = 1;
			stp = -PWM_step;
		}
		if(dcl <= 0)
		{
			dcl = 0;
			col++;
			if(col > 2){col = 0;}
			stp = PWM_step;
		}

		gpio_pin_set_dt(&led, gpio_pin_get_dt(&button));
		dcl += stp;
		k_sleep(K_MSEC(50u));
	}
}

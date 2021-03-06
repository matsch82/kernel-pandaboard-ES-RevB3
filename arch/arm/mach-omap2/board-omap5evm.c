/*
 * Board support file for OMAP5430 based EVM.
 *
 * Copyright (C) 2010-2011 Texas Instruments
 * Author: Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * Based on mach-omap2/board-4430sdp.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/hwspinlock.h>
#include <linux/i2c/tsl2771.h>
#include <linux/i2c/pca953x.h>
#include <linux/input/mpu6050.h>
#include <linux/input/matrix_keypad.h>
#include <linux/mfd/twl6040.h>

#include <linux/platform_data/omap4-keypad.h>
#include <linux/of_fdt.h>
#include <linux/i2c-gpio.h>
#include <linux/clk.h>
#include <linux/platform_data/omap-abe-twl6040.h>

#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#ifdef CONFIG_OMAP5_SEVM_PALMAS
#include <linux/mfd/palmas.h>
#endif
#include <linux/i2c/smsc.h>
#include <linux/wl12xx.h>

#include <drm/drm_edid.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/hardware/gic.h>

#include <mach/hardware.h>
#include "common.h"
#include <plat/common.h>
#include <plat/usb.h>
#include <plat/mmc.h>
#include <plat/omap4-keypad.h>
#include <plat/omap_hwmod.h>
#include "hsmmc.h"
#include <plat/remoteproc.h>
#include <plat/rpmsg_resmgr.h>
#include "common-board-devices.h"
#include "mux.h"
#include <linux/qtouch_obp_ts.h>

#include "common-board-devices.h"
#include "board-omap5evm.h"

#define OMAP5_TOUCH_IRQ_1              179
#define OMAP5_TOUCH_RESET              230

#define GPIO_WIFI_PMENA			140

#define HDMI_OE_GPIO                   256
#define HDMI_HPD_EN_GPIO               257

static int gpio_wlan_irq = 9; /* correct for sEVM */

static struct gpio_led gpio_leds[] = {
	{
		.name			= "red",
		.default_trigger	= "heartbeat",
		.gpio			= 273,
	},
	{
		.name			= "usr1",
		.default_trigger	= "default-off",
		.gpio			= 258,
	},
	{
		.name			= "usr2",
		.default_trigger	= "default-off",
		.gpio			= 259,
	},
	{
		.name			= "usr3",
		.default_trigger	= "default-off",
		.gpio			= 260,
	},
	{
		.name			= "usr4",
		.default_trigger	= "default-off",
		.gpio			= 261,
	},
	{
		.name			= "usr5",
		.default_trigger	= "default-off",
		.gpio			= 262,
	},
};

static struct gpio_led_platform_data gpio_led_info = {
	.leds		= gpio_leds,
	.num_leds	= ARRAY_SIZE(gpio_leds),
};

static struct platform_device leds_gpio = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &gpio_led_info,
	},
};

static const int evm5430_keymap[] = {
	KEY(0, 0, KEY_RESERVED),
	KEY(0, 1, KEY_RESERVED),
	KEY(0, 2, KEY_RESERVED),
	KEY(0, 3, KEY_RESERVED),
	KEY(0, 4, KEY_RESERVED),
	KEY(0, 5, KEY_RESERVED),
	KEY(0, 6, KEY_RESERVED),
	KEY(0, 7, KEY_RESERVED),

	KEY(1, 0, KEY_RESERVED),
	KEY(1, 1, KEY_RESERVED),
	KEY(1, 2, KEY_RESERVED),
	KEY(1, 3, KEY_RESERVED),
	KEY(1, 4, KEY_RESERVED),
	KEY(1, 5, KEY_RESERVED),
	KEY(1, 6, KEY_RESERVED),
	KEY(1, 7, KEY_RESERVED),

	KEY(2, 0, KEY_RESERVED),
	KEY(2, 1, KEY_RESERVED),
	KEY(2, 2, KEY_VOLUMEUP),
	KEY(2, 3, KEY_VOLUMEDOWN),
	KEY(2, 4, KEY_SEND),
	KEY(2, 5, KEY_HOME),
	KEY(2, 6, KEY_END),
	KEY(2, 7, KEY_SEARCH),

	KEY(3, 0, KEY_RESERVED),
	KEY(3, 1, KEY_RESERVED),
	KEY(3, 2, KEY_RESERVED),
	KEY(3, 3, KEY_RESERVED),
	KEY(3, 4, KEY_RESERVED),
	KEY(3, 5, KEY_RESERVED),
	KEY(3, 6, KEY_RESERVED),
	KEY(3, 7, KEY_RESERVED),

	KEY(4, 0, KEY_RESERVED),
	KEY(4, 1, KEY_RESERVED),
	KEY(4, 2, KEY_RESERVED),
	KEY(4, 3, KEY_RESERVED),
	KEY(4, 4, KEY_RESERVED),
	KEY(4, 5, KEY_RESERVED),
	KEY(4, 6, KEY_RESERVED),
	KEY(4, 7, KEY_RESERVED),

	KEY(5, 0, KEY_RESERVED),
	KEY(5, 1, KEY_RESERVED),
	KEY(5, 2, KEY_RESERVED),
	KEY(5, 3, KEY_RESERVED),
	KEY(5, 4, KEY_RESERVED),
	KEY(5, 5, KEY_RESERVED),
	KEY(5, 6, KEY_RESERVED),
	KEY(5, 7, KEY_RESERVED),

	KEY(6, 0, KEY_RESERVED),
	KEY(6, 1, KEY_RESERVED),
	KEY(6, 2, KEY_RESERVED),
	KEY(6, 3, KEY_RESERVED),
	KEY(6, 4, KEY_RESERVED),
	KEY(6, 5, KEY_RESERVED),
	KEY(6, 6, KEY_RESERVED),
	KEY(6, 7, KEY_RESERVED),

	KEY(7, 0, KEY_RESERVED),
	KEY(7, 1, KEY_RESERVED),
	KEY(7, 2, KEY_RESERVED),
	KEY(7, 3, KEY_RESERVED),
	KEY(7, 4, KEY_RESERVED),
	KEY(7, 5, KEY_RESERVED),
	KEY(7, 6, KEY_RESERVED),
	KEY(7, 7, KEY_RESERVED),
};

static struct matrix_keymap_data evm5430_keymap_data = {
	.keymap                 = evm5430_keymap,
	.keymap_size            = ARRAY_SIZE(evm5430_keymap),
};

static struct omap4_keypad_platform_data evm5430_keypad_data = {
	.keymap_data            = &evm5430_keymap_data,
	.rows                   = 8,
	.cols                   = 8,
};

static struct omap_board_data keypad_data = {
	.id                     = 1,
};

static uint32_t board_keymap[] = {

	KEY(0, 0,  KEY_RESERVED)     , KEY(0, 1,  KEY_RESERVED),
	KEY(0, 2,  KEY_F7)           , KEY(0, 3,  KEY_ESC),
	KEY(0, 4,  KEY_F4)           , KEY(0, 5,  KEY_G),
	KEY(0, 6,  KEY_RESERVED)     , KEY(0, 7,  KEY_H),
	KEY(0, 8,  KEY_RESERVED)     , KEY(0, 9,  KEY_CYCLEWINDOWS),
	KEY(0, 10, KEY_RESERVED)     , KEY(0, 11, KEY_RESERVED),
	KEY(0, 12, KEY_BACKSPACE)    , KEY(0, 13, KEY_F11),
	KEY(0, 14, KEY_FORWARD)      , KEY(0, 15, KEY_INSERT),

	KEY(1, 0,  KEY_RIGHTSHIFT)   , KEY(1, 1,  KEY_RESERVED),
	KEY(1, 2,  KEY_W)            , KEY(1, 3,  KEY_Q),
	KEY(1, 4,  KEY_E)            , KEY(1, 5,  KEY_R),
	KEY(1, 6,  KEY_RESERVED)     , KEY(1, 7,  KEY_U),
	KEY(1, 8,  KEY_I)	     , KEY(1, 9,  KEY_RESERVED),
	KEY(1, 10, KEY_RESERVED)     , KEY(1, 11, KEY_RESERVED),
	KEY(1, 12, KEY_UP)           , KEY(1, 13, KEY_O),
	KEY(1, 14, KEY_P)            , KEY(1, 15, KEY_LEFT),

	KEY(2, 0,  KEY_RESERVED)     , KEY(2, 1,  KEY_RESERVED),
	KEY(2, 2,  KEY_2)            , KEY(2, 3,  KEY_1),
	KEY(2, 4,  KEY_3)            , KEY(2, 5,  KEY_4),
	KEY(2, 6,  KEY_RESERVED)     , KEY(2, 7,  KEY_7),
	KEY(2, 8,  KEY_8)            , KEY(2, 9,  KEY_RESERVED),
	KEY(2, 10, KEY_RESERVED)     , KEY(2, 11, KEY_RIGHTALT),
	KEY(2, 12, KEY_DOWN)         , KEY(2, 13, KEY_9),
	KEY(2, 14, KEY_0)            , KEY(2, 15, KEY_RIGHT),

	KEY(3, 0,  KEY_RESERVED)     , KEY(3, 1,  KEY_RIGHTCTRL),
	KEY(3, 2,  KEY_S)            , KEY(3, 3,  KEY_A),
	KEY(3, 4,  KEY_D)            , KEY(3, 5,  KEY_F),
	KEY(3, 6,  KEY_RESERVED)     , KEY(3, 7,  KEY_J),
	KEY(3, 8,  KEY_K)            , KEY(3, 9,  KEY_RESERVED),
	KEY(3, 10, KEY_RESERVED)     , KEY(3, 11, KEY_RESERVED),
	KEY(3, 12, KEY_ENTER)        , KEY(3, 13, KEY_L),
	KEY(3, 14, KEY_SEMICOLON)    , KEY(3, 15, KEY_RESERVED),
	KEY(4, 0,  KEY_LEFTSHIFT)    , KEY(4, 1,  KEY_RESERVED),
	KEY(4, 2,  KEY_X)            , KEY(4, 3,  KEY_Z),
	KEY(4, 4,  KEY_C)            , KEY(4, 5,  KEY_V),
	KEY(4, 6,  KEY_RESERVED)     , KEY(4, 7,  KEY_M),
	KEY(4, 8,  KEY_COMMA)        , KEY(4, 9,  KEY_RESERVED),
	KEY(4, 10, KEY_RESERVED)     , KEY(4, 11, KEY_RESERVED),
	KEY(4, 12, KEY_SPACE)        , KEY(4, 13, KEY_DOT),
	KEY(4, 14, KEY_SLASH)        , KEY(4, 15, KEY_END),

	KEY(5, 0,  KEY_RESERVED)     , KEY(5, 1,  KEY_LEFTCTRL),
	KEY(5, 2,  KEY_F6)           , KEY(5, 3,  KEY_TAB),
	KEY(5, 4,  KEY_F3)           , KEY(5, 5,  KEY_T),
	KEY(5, 6,  KEY_RESERVED)     , KEY(5, 7,  KEY_Y),
	KEY(5, 8,  KEY_LEFTBRACE)    , KEY(5, 9,  KEY_RESERVED),
	KEY(5, 10, KEY_RESERVED)     , KEY(5, 11, KEY_RESERVED),
	KEY(5, 12, KEY_RESERVED)     , KEY(5, 13, KEY_F10),
	KEY(5, 14, KEY_RIGHTBRACE)   , KEY(5, 15, KEY_HOME),
	KEY(6, 0,  KEY_RESERVED)     , KEY(6, 1,  KEY_RESERVED),
	KEY(6, 2,  KEY_F5)           , KEY(6, 3,  KEY_RESERVED),
	KEY(6, 4,  KEY_F2)           , KEY(6, 5,  KEY_5),
	KEY(6, 6,  KEY_FN)           , KEY(6, 7,  KEY_6),
	KEY(6, 8,  KEY_RESERVED)     , KEY(6, 9,  KEY_RESERVED),
	KEY(6, 10, KEY_MENU)         , KEY(6, 11, KEY_RESERVED),
	KEY(6, 12, KEY_BACKSLASH)    , KEY(6, 13, KEY_F9),
	KEY(6, 14, KEY_RESERVED)     , KEY(6, 15, KEY_RESERVED),

	KEY(7, 0,  KEY_RESERVED)     , KEY(7, 1,  KEY_RESERVED),
	KEY(7, 2,  KEY_F8)           , KEY(7, 3,  KEY_CAPSLOCK),
	KEY(7, 4,  KEY_F1)           , KEY(7, 5,  KEY_B),
	KEY(7, 6,  KEY_RESERVED)     , KEY(7, 7,  KEY_N),
	KEY(7, 8,  KEY_RESERVED)     , KEY(7, 9,  KEY_RESERVED),
	KEY(7, 10, KEY_RESERVED)     , KEY(7, 11, KEY_LEFTALT),
	KEY(7, 12, KEY_RESERVED)     , KEY(7, 13, KEY_F12),
	KEY(7, 14, KEY_RESERVED)     , KEY(7, 15, KEY_DELETE),
};

static struct matrix_keymap_data board_map_data = {
	.keymap                 = board_keymap,
	.keymap_size            = ARRAY_SIZE(board_keymap),
};

static struct smsc_keypad_data omap5_kp_data = {
	.keymap_data    = &board_map_data,
	.rows           = 8,
	.cols           = 16,
	.rep            = 1,
};

#ifndef CONFIG_MACH_OMAP_5430ZEBU
static struct __devinitdata emif_custom_configs custom_configs = {
	.mask	= EMIF_CUSTOM_CONFIG_LPMODE,
	.lpmode	= EMIF_LP_MODE_DISABLE
};
#endif

static void __init omap_i2c_hwspinlock_init(int bus_id, int spinlock_id,
					struct omap_i2c_bus_board_data *pdata)
{
	/* spinlock_id should be -1 for a generic lock request */
	if (spinlock_id < 0)
		pdata->handle = hwspin_lock_request();
	else
		pdata->handle = hwspin_lock_request_specific(spinlock_id);

	if (pdata->handle != NULL) {
		pdata->hwspin_lock_timeout = hwspin_lock_timeout;
		pdata->hwspin_unlock = hwspin_unlock;
	} else {
		pr_err("I2C hwspinlock request failed for bus %d\n", \
								bus_id);
	}
}

static struct omap_i2c_bus_board_data __initdata sdp4430_i2c_1_bus_pdata;
static struct omap_i2c_bus_board_data __initdata sdp4430_i2c_2_bus_pdata;
static struct omap_i2c_bus_board_data __initdata sdp4430_i2c_3_bus_pdata;
static struct omap_i2c_bus_board_data __initdata sdp4430_i2c_4_bus_pdata;
static struct omap_i2c_bus_board_data __initdata sdp4430_i2c_5_bus_pdata;

#include <video/omapdss.h>
#include <video/omap-panel-lg4591.h>

#define HDMI_GPIO_HPD 193

#ifdef CONFIG_OMAP5_SEVM_PALMAS
#define OMAP5_GPIO_END	0

static struct palmas_gpadc_platform_data omap5_palmas_gpadc = {
	.ch3_current = 0,
	.ch0_current = 0,
	.bat_removal = 0,
	.start_polarity = 0,
};

/* Initialisation Data for Regulators */

static struct palmas_reg_init omap5_smps12_init = {
	.warm_reset = 0,
	.roof_floor = 0,
	.mode_sleep = 0,
	.tstep = 0,
};

static struct palmas_reg_init omap5_smps45_init = {
	.warm_reset = 0,
	.roof_floor = 0,
	.mode_sleep = 0,
	.tstep = 0,
};

static struct palmas_reg_init omap5_smps6_init = {
	.warm_reset = 0,
	.roof_floor = 0,
	.mode_sleep = 1,
	.tstep = 0,
};

static struct palmas_reg_init omap5_smps7_init = {
	.warm_reset = 0,
	.roof_floor = 0,
	.mode_sleep = 1,
};

static struct palmas_reg_init omap5_smps8_init = {
	.warm_reset = 0,
	.roof_floor = 0,
	.mode_sleep = 0,
	.tstep = 0,
};

static struct palmas_reg_init omap5_smps9_init = {
	.warm_reset = 0,
	.roof_floor = 0,
	.mode_sleep = 0,
	.vsel = 0xbd,
};

static struct palmas_reg_init omap5_smps10_init = {
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo1_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo2_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo3_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo4_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo5_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo6_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo7_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo8_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldo9_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
	.no_bypass = 1,
};

static struct palmas_reg_init omap5_ldoln_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init omap5_ldousb_init = {
	.warm_reset = 0,
	.mode_sleep = 0,
};

static struct palmas_reg_init *palmas_omap_reg_init[] = {
	&omap5_smps12_init,
	NULL, /* SMPS123 not used in this configuration */
	NULL, /* SMPS3 not used in this configuration */
	&omap5_smps45_init,
	NULL, /* SMPS457 not used in this configuration */
	&omap5_smps6_init,
	&omap5_smps7_init,
	&omap5_smps8_init,
	&omap5_smps9_init,
	&omap5_smps10_init,
	&omap5_ldo1_init,
	&omap5_ldo2_init,
	&omap5_ldo3_init,
	&omap5_ldo4_init,
	&omap5_ldo5_init,
	&omap5_ldo6_init,
	&omap5_ldo7_init,
	&omap5_ldo8_init,
	&omap5_ldo9_init,
	&omap5_ldoln_init,
	&omap5_ldousb_init,

};

/* Constraints for Regulators */
static struct regulator_init_data omap5_smps12 = {
	.constraints = {
		.min_uV			= 600000,
	.max_uV			= 1310000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data omap5_smps45 = {
	.constraints = {
		.min_uV			= 600000,
		.max_uV			= 1310000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data omap5_smps6 = {
	.constraints = {
		.min_uV			= 1200000,
		.max_uV			= 1200000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data omap5_smps7 = {
	.constraints = {
		.min_uV			= 1800000,
		.max_uV			= 1800000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data omap5_smps8 = {
	.constraints = {
		.min_uV			= 600000,
		.max_uV			= 1310000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_consumer_supply omap5_adac_supply[] = {
	REGULATOR_SUPPLY("vcc", "soc-audio"),
};

static struct regulator_init_data omap5_smps9 = {
	.constraints = {
		.min_uV			= 2100000,
		.max_uV			= 2100000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
		.always_on		= 1,
	},
	.num_consumer_supplies	= ARRAY_SIZE(omap5_adac_supply),
	.consumer_supplies	= omap5_adac_supply,

};

static struct regulator_consumer_supply omap5_vbus_supply[] = {
	REGULATOR_SUPPLY("vbus", "1-0048"),
};

static struct regulator_init_data omap5_smps10 = {
	.constraints = {
		.min_uV			= 5000000,
		.max_uV			= 5000000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	= ARRAY_SIZE(omap5_vbus_supply),
	.consumer_supplies	= omap5_vbus_supply,
};

static struct regulator_init_data omap5_ldo1 = {
	.constraints = {
		.min_uV			= 2800000,
		.max_uV			= 2800000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
		.always_on		= 1,
	},
};

static struct regulator_consumer_supply omap5evm_lcd_panel_supply[] = {
	REGULATOR_SUPPLY("panel_supply", "omapdss_dsi.0"),
};

static struct regulator_init_data omap5_ldo2 = {
	.constraints = {
		.min_uV			= 2900000,
		.max_uV			= 2900000,
		.apply_uV               = true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
		.always_on              = true,
	},
	.num_consumer_supplies	= ARRAY_SIZE(omap5evm_lcd_panel_supply),
	.consumer_supplies	= omap5evm_lcd_panel_supply,
};

static struct regulator_init_data omap5_ldo3 = {
	.constraints = {
		.min_uV			= 3000000,
		.max_uV			= 3000000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data omap5_ldo4 = {
	.constraints = {
		.min_uV			= 2200000,
		.max_uV			= 2200000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data omap5_ldo5 = {
	.constraints = {
		.min_uV			= 1800000,
		.max_uV			= 1800000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data omap5_ldo6 = {
	.constraints = {
		.min_uV			= 1500000,
		.max_uV			= 1500000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_consumer_supply omap5_dss_phy_supply[] = {
	REGULATOR_SUPPLY("vdds_dsi", "omapdss"),
	REGULATOR_SUPPLY("vdds_dsi", "omapdss_dsi.0"),
	REGULATOR_SUPPLY("vdds_dsi", "omapdss_dsi.1"),
	REGULATOR_SUPPLY("vdds_hdmi", "omapdss_hdmi"),
};

static struct regulator_init_data omap5_ldo7 = {
	.constraints = {
		.min_uV			= 1500000,
		.max_uV			= 1500000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
		.apply_uV		= 1,
	},
	.num_consumer_supplies	= ARRAY_SIZE(omap5_dss_phy_supply),
	.consumer_supplies	= omap5_dss_phy_supply,
};

static struct regulator_init_data omap5_ldo8 = {
	.constraints = {
		.min_uV			= 1500000,
		.max_uV			= 1500000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_consumer_supply omap5_mmc1_io_supply[] = {
	REGULATOR_SUPPLY("vmmc_aux", "omap_hsmmc.0"),
};

static struct regulator_init_data omap5_ldo9 = {
	.constraints = {
		.min_uV			= 1800000,
		.max_uV			= 3000000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies  = ARRAY_SIZE(omap5_mmc1_io_supply),
	.consumer_supplies      = omap5_mmc1_io_supply,
};

static struct regulator_init_data omap5_ldoln = {
	.constraints = {
		.min_uV			= 1800000,
		.max_uV			= 1800000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data omap5_ldousb = {
	.constraints = {
		.min_uV			= 3250000,
		.max_uV			= 3250000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data *palmas_omap5_reg[] = {
	&omap5_smps12,
	NULL, /* SMPS123 not used in this configuration */
	NULL, /* SMPS3 not used in this configuration */
	&omap5_smps45,
	NULL, /* SMPS457 not used in this configuration */
	&omap5_smps6,
	&omap5_smps7,
	&omap5_smps8,
	&omap5_smps9,
	&omap5_smps10,

	&omap5_ldo1,
	&omap5_ldo2,
	&omap5_ldo3,
	&omap5_ldo4,
	&omap5_ldo5,
	&omap5_ldo6,
	&omap5_ldo7,
	&omap5_ldo8,
	&omap5_ldo9,
	&omap5_ldoln,
	&omap5_ldousb,
};

static struct palmas_pmic_platform_data omap5_palmas_pmic = {
	.reg_data = palmas_omap5_reg,
	.reg_init = palmas_omap_reg_init,

	.ldo6_vibrator = 0,
};

static struct palmas_resource_platform_data omap5_palmas_resource = {
	.clk32kg_mode_sleep = 0,
	.clk32kgaudio_mode_sleep = 0,
	.regen1_mode_sleep = 0,
	.regen2_mode_sleep = 0,
	.sysen1_mode_sleep = 0,
	.sysen2_mode_sleep = 0,

	.sysen2_mode_active = 1,

	.nsleep_res = 0,
	.nsleep_smps = 0,
	.nsleep_ldo1 = 0,
	.nsleep_ldo2 = 0,

	.enable1_res = 0,
	.enable1_smps = 0,
	.enable1_ldo1 = 0,
	.enable1_ldo2 = 0,

	.enable2_res = 0,
	.enable2_smps = 0,
	.enable2_ldo1 = 0,
	.enable2_ldo2 = 0,
};

static struct palmas_usb_platform_data omap5_palmas_usb = {
	.wakeup = 1,
};

static struct palmas_platform_data palmas_omap5 = {
	.gpio_base = OMAP5_GPIO_END,

	.power_ctrl = POWER_CTRL_NSLEEP_MASK | POWER_CTRL_ENABLE1_MASK |
			POWER_CTRL_ENABLE1_MASK,

	.gpadc_pdata = &omap5_palmas_gpadc,
	.pmic_pdata = &omap5_palmas_pmic,
	.usb_pdata = &omap5_palmas_usb,
	.resource_pdata = &omap5_palmas_resource,
};
#endif  /* CONFIG_OMAP5_SEVM_PALMAS */

static struct twl6040_codec_data twl6040_codec = {
	/* single-step ramp for headset and handsfree */
	.hs_left_step	= 0x0f,
	.hs_right_step	= 0x0f,
	.hf_left_step	= 0x1d,
	.hf_right_step	= 0x1d,
};

static struct twl6040_vibra_data twl6040_vibra = {
	.vibldrv_res = 8,
	.vibrdrv_res = 3,
	.viblmotor_res = 10,
	.vibrmotor_res = 10,
	.vddvibl_uV = 0,	/* fixed volt supply - VBAT */
	.vddvibr_uV = 0,	/* fixed volt supply - VBAT */
};

static struct twl6040_platform_data twl6040_data = {
	.codec		= &twl6040_codec,
	.vibra		= &twl6040_vibra,
	.audpwron_gpio	= 145,
	.irq_base	= TWL6040_CODEC_IRQ_BASE,
};

static struct platform_device omap5evm_dmic_codec = {
	.name	= "dmic-codec",
	.id	= -1,
};

static struct omap_abe_twl6040_data omap5evm_abe_audio_data = {
	/* Audio out */
	.has_hs		= ABE_TWL6040_LEFT | ABE_TWL6040_RIGHT,
	/* HandsFree through expasion connector */
	.has_hf		= ABE_TWL6040_LEFT | ABE_TWL6040_RIGHT,
	/* Earpiece */
	.has_ep		= 1,
	/* PandaBoard: FM TX, PandaBoardES: can be connected to audio out */
	.has_aux	= ABE_TWL6040_LEFT | ABE_TWL6040_RIGHT,
	/* PandaBoard: FM RX, PandaBoardES: audio in */
	.has_afm	= ABE_TWL6040_LEFT | ABE_TWL6040_RIGHT,
	.has_abe	= 1,
	.has_dmic	= 0,
	.has_hsmic	= 1,
	.has_mainmic	= 1,
	.has_submic	= 1,
	/* Jack detection. */
	.jack_detection	= 1,
	/* MCLK input is 19.2MHz */
	.mclk_freq	= 19200000,
	.card_name = "OMAP5EVM",

};

static struct platform_device omap5evm_abe_audio = {
	.name		= "omap-abe-twl6040",
	.id		= -1,
	.dev = {
		.platform_data = &omap5evm_abe_audio_data,
	},
};

static struct platform_device omap5evm_hdmi_audio_codec = {
	.name	= "hdmi-audio-codec",
	.id	= -1,
};

static struct platform_device *omap5evm_devices[] __initdata = {
	&omap5evm_dmic_codec,
	&omap5evm_hdmi_audio_codec,
	&omap5evm_abe_audio,
	&leds_gpio,
};

/*
 * Display monitor features are burnt in their EEPROM as EDID data. The EEPROM
 * is connected as I2C slave device, and can be accessed at address 0x50
 */
static struct i2c_board_info __initdata hdmi_i2c_eeprom[] = {
        {
                I2C_BOARD_INFO("eeprom", DDC_ADDR),
        },
};

static struct i2c_gpio_platform_data i2c_gpio_pdata = {
        .sda_pin                = 195,
        .sda_is_open_drain      = 0,
        .scl_pin                = 194,
        .scl_is_open_drain      = 0,
        .udelay                 = 2,            /* ~100 kHz */
};

static struct platform_device hdmi_edid_device = {
        .name                   = "i2c-gpio",
        .id                     = -1,
        .dev.platform_data      = &i2c_gpio_pdata,
};

static struct i2c_board_info __initdata omap5evm_i2c_1_boardinfo[] = {
#ifdef CONFIG_OMAP5_SEVM_PALMAS
	{
		I2C_BOARD_INFO("twl6035", 0x48),
		.platform_data = &palmas_omap5,
		.irq = OMAP44XX_IRQ_SYS_1N,
	},
#endif
        {
                I2C_BOARD_INFO("twl6040", 0x4b),
                .platform_data = &twl6040_data,
                .irq = OMAP44XX_IRQ_SYS_2N,
        },

};

#if 0
static struct qtm_touch_keyarray_cfg omap5evm_key_array_data[] = {
	{
		.ctrl = 0,
		.x_origin = 0,
		.y_origin = 0,
		.x_size = 0,
		.y_size = 0,
		.aks_cfg = 0,
		.burst_len = 0,
		.tch_det_thr = 0,
		.tch_det_int = 0,
		.rsvd1 = 0,
	},
};

static struct qtouch_ts_platform_data atmel_mxt224_ts_platform_data = {
	.irqflags       = (IRQF_TRIGGER_FALLING | IRQF_TRIGGER_LOW),
	.flags          = (QTOUCH_USE_MULTITOUCH | QTOUCH_FLIP_X |
			   QTOUCH_FLIP_Y | QTOUCH_CFG_BACKUPNV),
	.abs_min_x      = 0,
	.abs_max_x      = 1280,
	.abs_min_y      = 0,
	.abs_max_y      = 1024,
	.abs_min_p      = 0,
	.abs_max_p      = 255,
	.abs_min_w      = 0,
	.abs_max_w      = 15,
	.x_delta        = 0x00,
	.y_delta        = 0x00,
	.nv_checksum    = 0x187a,
	.fuzz_x         = 0,
	.fuzz_y         = 0,
	.fuzz_p         = 2,
	.fuzz_w         = 2,
	.hw_reset       = NULL,
	.gen_cmd_proc = {
		.reset  = 0x00,
		.backupnv = 0x00,
		.calibrate = 0x01,
		.reportall = 0x00,
	},
	.power_cfg      = {
		.idle_acq_int   = 0xff,
		.active_acq_int = 0xff,
		.active_idle_to = 0x0a,
	},
	.acquire_cfg    = {
		.charge_time    = 0x07,
		.atouch_drift   = 0x05,
		.touch_drift    = 0x14,
		.drift_susp     = 0x14,
		.touch_autocal  = 0x00,
		.sync           = 0x00,
		.cal_suspend_time = 0x0a,
		.cal_suspend_thresh = 0x00,
	},
	.multi_touch_cfg        = {
		.ctrl           = 0x83,
		.x_origin       = 0x00,
		.y_origin       = 0x00,
		.x_size         = 0x12,
		.y_size         = 0x0c,
		.aks_cfg        = 0x00,
		.burst_len      = 0x14,
		.tch_det_thr    = 0x28,
		.tch_det_int    = 0x02,
		.mov_hyst_init  = 0x0a,
		.mov_hyst_next  = 0x0a,
		.mov_filter     = 0x00,
		.num_touch      = 10,
		.orient         = 0x00,
		.mrg_timeout    = 0x00,
		.merge_hyst     = 0x0a,
		.merge_thresh   = 0x0a,
		.amp_hyst       = 0x00,
		.x_res          = 0x0fff,
		.y_res          = 0x0500,
		.x_low_clip     = 0x00,
		.x_high_clip    = 0x00,
		.y_low_clip     = 0x00,
		.y_high_clip    = 0x00,
		.x_edge_ctrl    = 0xD4,
		.x_edge_dist    = 0x42,
		.y_edge_ctrl    = 0xD4,
		.y_edge_dist    = 0x64,
		.jumplimit      = 0x3E,
	},
	.key_array      = {
		.cfg            = omap5evm_key_array_data,
		.num_keys   = ARRAY_SIZE(omap5evm_key_array_data),
	},
	.grip_suppression_cfg = {
		.ctrl           = 0x00,
		.xlogrip        = 0x00,
		.xhigrip        = 0x00,
		.ylogrip        = 0x00,
		.yhigrip        = 0x00,
		.maxtchs        = 0x00,
		.reserve0       = 0x00,
		.szthr1         = 0x00,
		.szthr2         = 0x00,
		.shpthr1        = 0x00,
		.shpthr2        = 0x00,
		.supextto       = 0x00,
	},
	.noise0_suppression_cfg = {
		.ctrl           = 0x07,
		.reserved       = 0x0000,
		.gcaf_upper_limit = 0x0019,
		.gcaf_lower_limit = 0xffe7,
		.gcaf_valid     = 0x04,
		.noise_thresh   = 0x14,
		.reserved1      = 0x00,
		.freq_hop_scale = 0x00,
		.burst_freq_0   = 0x0a,
		.burst_freq_1 = 0x07,
		.burst_freq_2 = 0x0c,
		.burst_freq_3 = 0x11,
		.burst_freq_4 = 0x14,
		.num_of_gcaf_samples = 0x04,
	},
	.touch_proximity_cfg = {
		.ctrl           = 0x00,
		.xorigin        = 0x00,
		.yorigin        = 0x00,
		.xsize          = 0x00,
		.ysize          = 0x00,
		.reserved       = 0x00,
		.blen           = 0x00,
		.fxddthr        = 0x0000,
		.fxddi          = 0x00,
		.average        = 0x00,
		.mvnullrate     = 0x0000,
		.mvdthr         = 0x0000,
	},
	.spt_commsconfig_cfg = {
		.ctrl           = 0x00,
		.command        = 0x00,
	},
	.spt_gpiopwm_cfg = {
		.ctrl           = 0x00,
		.reportmask     = 0x00,
		.dir            = 0x00,
		.intpullup      = 0x00,
		.out            = 0x00,
		.wake           = 0x00,
		.pwm            = 0x00,
		.period         = 0x00,
		.duty0          = 0x00,
		.duty1          = 0x00,
		.duty2          = 0x00,
		.duty3          = 0x00,
		.trigger0       = 0x00,
		.trigger1       = 0x00,
		.trigger2       = 0x00,
		.trigger3       = 0x00,
	},
	.onetouchgestureprocessor_cfg = {
		.ctrl   =       0x00,
		.numgest =      0x00,
		.gesten =       0x00,
		.process =      0x00,
		.tapto =        0x00,
		.flickto =      0x00,
		.dragto =       0x00,
		.spressto =     0x00,
		.lpressto =     0x00,
		.reppressto =   0x00,
		.flickthr =     0x00,
		.dragthr =      0x00,
		.tapthr =       0x00,
		.throwthr =     0x00,
	},
	.spt_selftest_cfg = {
		.ctrl = 0x03,
		.cmd = 0x00,
		.hisiglim0 = 0x36b0,
		.losiglim0 = 0x1b58,
		.hisiglim1 = 0x0000,
		.losiglim1 = 0x0000,
		.hisiglim2 = 0x0000,
		.losiglim2 = 0x0000,
	},
	.twotouchgestureprocessor_cfg = {
		.ctrl   =       0x03,
		.numgest =      0x01,
		.reserved =     0x00,
		.gesten =       0xe0,
		.rotatethr =    0x03,
		.zoomthr =      0x0063,
	},
	.spt_cte_cfg = {
		.ctrl = 0x00,
		.command = 0x00,
		.mode = 0x02,
		.gcaf_idle_mode = 0x20,
		.gcaf_actv_mode = 0x20,
	},
};

static struct i2c_board_info __initdata omap5evm_i2c_4_boardinfo[] = {
	{
		I2C_BOARD_INFO(QTOUCH_TS_NAME, 0x4a),
		.platform_data = &atmel_mxt224_ts_platform_data,
		.irq = OMAP_GPIO_IRQ(OMAP5_TOUCH_IRQ_1),
	},
};
#endif

/*
 * I2C GPIO Expander - TCA6424
 */

static struct pca953x_platform_data omap_5430evm_gpio_expander_info_0 = {
	.gpio_base	= OMAP_MAX_GPIO_LINES,
};

static struct i2c_board_info __initdata omap5evm_i2c_5_boardinfo[] = {
	{
		I2C_BOARD_INFO("smsc", 0x38),
		.platform_data = &omap5_kp_data,
		.irq = 151,
	},
	{
		I2C_BOARD_INFO("tca6424", 0x22),
		.platform_data = &omap_5430evm_gpio_expander_info_0,
	},
};

static int __init omap_5430evm_i2c_init(void)
{
	omap_i2c_hwspinlock_init(1, 0, &sdp4430_i2c_1_bus_pdata);
	omap_i2c_hwspinlock_init(2, 1, &sdp4430_i2c_2_bus_pdata);
	omap_i2c_hwspinlock_init(3, 2, &sdp4430_i2c_3_bus_pdata);
	omap_i2c_hwspinlock_init(4, 3, &sdp4430_i2c_4_bus_pdata);
	omap_i2c_hwspinlock_init(5, 4, &sdp4430_i2c_5_bus_pdata);

	omap_register_i2c_bus_board_data(1, &sdp4430_i2c_1_bus_pdata);
	omap_register_i2c_bus_board_data(2, &sdp4430_i2c_2_bus_pdata);
	omap_register_i2c_bus_board_data(3, &sdp4430_i2c_3_bus_pdata);
	omap_register_i2c_bus_board_data(4, &sdp4430_i2c_4_bus_pdata);
	omap_register_i2c_bus_board_data(5, &sdp4430_i2c_5_bus_pdata);

#ifdef CONFIG_OMAP5_SEVM_PALMAS
	omap_register_i2c_bus(1, 400, omap5evm_i2c_1_boardinfo,
				ARRAY_SIZE(omap5evm_i2c_1_boardinfo));
#else
	omap_register_i2c_bus(1, 400, NULL, 0);
#endif
	omap_register_i2c_bus(2, 400, NULL, 0);
	omap_register_i2c_bus(3, 400, NULL, 0);
	omap_register_i2c_bus(4, 400, NULL, 0);
	omap_register_i2c_bus(5, 400, omap5evm_i2c_5_boardinfo,
				ARRAY_SIZE(omap5evm_i2c_5_boardinfo));
	return 0;
}

int __init omap5evm_touch_init(void)
{
	gpio_request(OMAP5_TOUCH_IRQ_1, "atmel touch irq");
	gpio_direction_input(OMAP5_TOUCH_IRQ_1);

	gpio_request(OMAP5_TOUCH_RESET, "atmel reset");
	gpio_direction_output(OMAP5_TOUCH_RESET, 1);
	mdelay(100);
	gpio_direction_output(OMAP5_TOUCH_RESET, 0);
	mdelay(100);
	gpio_direction_output(OMAP5_TOUCH_RESET, 1);

	return 0;
}

static struct omap2_hsmmc_info mmc[] = {
	{
		.mmc		= 2,
		.caps		= MMC_CAP_4_BIT_DATA | MMC_CAP_8_BIT_DATA |
					MMC_CAP_1_8V_DDR,
		.gpio_cd	= -EINVAL,
		.gpio_wp	= -EINVAL,
		.nonremovable	= true,
		.ocr_mask	= MMC_VDD_29_30,
		.no_off_init	= true,
	},
	{
		.mmc		= 1,
		.caps		= MMC_CAP_4_BIT_DATA | MMC_CAP_UHS_SDR12 |
					MMC_CAP_UHS_SDR25 | MMC_CAP_UHS_DDR50,
		.gpio_cd	= 67,
		.gpio_wp	= -EINVAL,
	},
	{
		.mmc            = 3,
		.caps           = MMC_CAP_4_BIT_DATA | MMC_CAP_POWER_OFF_CARD,
		.gpio_cd        = -EINVAL,
		.gpio_wp        = -EINVAL,
		.ocr_mask       = MMC_VDD_165_195,
		.nonremovable   = true,
	},
	{}	/* Terminator */
};

#ifdef CONFIG_WL12XX_PLATFORM_DATA
static struct regulator_consumer_supply omap5_sdp5430_vmmc3_supply[] = {
	REGULATOR_SUPPLY("vmmc", "omap_hsmmc.2"),
};

static struct regulator_init_data sdp5430_vmmc3 = {
	.constraints            = {
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies  = ARRAY_SIZE(omap5_sdp5430_vmmc3_supply),
	.consumer_supplies      = omap5_sdp5430_vmmc3_supply,
};

static struct fixed_voltage_config sdp5430_vwlan = {
	.supply_name            = "vwl1271",
	.microvolts             = 1800000, /* 1.8V */
	.gpio                   = GPIO_WIFI_PMENA,
	.startup_delay          = 70000, /* 70msec */
	.enable_high            = 1,
	.enabled_at_boot        = 0,
	.init_data              = &sdp5430_vmmc3,
};

static struct platform_device omap_vwlan_device = {
	.name           = "reg-fixed-voltage",
	.id             = 1,
	.dev = {
		.platform_data = &sdp5430_vwlan,
	},
};

static void omap5_sdp5430_wifi_mux_init(void)
{
	omap_mux_init_gpio(gpio_wlan_irq, OMAP_PIN_INPUT |
				OMAP_PIN_OFF_WAKEUPENABLE);
	omap_mux_init_gpio(GPIO_WIFI_PMENA, OMAP_PIN_OUTPUT);

	omap_mux_init_signal("wlsdio_cmd.wlsdio_cmd",
				OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("wlsdio_clk.wlsdio_clk",
				OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("wlsdio_data0.wlsdio_data0",
				OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("wlsdio_data1.wlsdio_data1",
				OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("wlsdio_data2.wlsdio_data2",
				OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("wlsdio_data3.wlsdio_data3",
				OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP);
}

static struct wl12xx_platform_data omap5_sdp5430_wlan_data __initdata = {
	.board_ref_clock    = WL12XX_REFCLOCK_26,
	.board_tcxo_clock   = WL12XX_TCXOCLOCK_26,
};

static void omap5_sdp5430_wifi_init(void)
{
	omap5_sdp5430_wifi_mux_init();

	omap5_sdp5430_wlan_data.irq = OMAP_GPIO_IRQ(gpio_wlan_irq);

	if (gpio_request_one(gpio_wlan_irq, GPIOF_IN, "wlan"))
		printk(KERN_INFO "wlan: IRQ gpio request failure in board file\n");

	if (wl12xx_set_platform_data(&omap5_sdp5430_wlan_data))
		pr_err("Error setting wl12xx data\n");

	platform_device_register(&omap_vwlan_device);
}
#endif

/* USBB3 to SMSC LAN9730 */
#define GPIO_ETH_NRESET_SEVM	172
#define GPIO_ETH_NRESET_UEVM    15

/* USBB2 to SMSC 4640 HUB */
#define GPIO_HUB_NRESET_SEVM	173
#define GPIO_HUB_NRESET_UEVM    80

static struct usbhs_omap_board_data usbhs_bdata __initdata = {
	.port_mode[0] = OMAP_USBHS_PORT_MODE_UNUSED,
	.port_mode[1] = OMAP_EHCI_PORT_MODE_HSIC,
	.port_mode[2] = OMAP_EHCI_PORT_MODE_HSIC,
	.phy_reset  = true,
	.reset_gpio_port[0]  = -EINVAL,
	.reset_gpio_port[1]  = GPIO_HUB_NRESET_SEVM,
	.reset_gpio_port[2]  = GPIO_ETH_NRESET_SEVM
};

static void __init omap_ehci_ohci_init(void)
{
	usbhs_init(&usbhs_bdata);
}

#ifdef CONFIG_OMAP_MUX
static struct omap_board_mux board_mux[] __initdata = {
	{ .reg_offset = OMAP_MUX_TERMINATOR },
};
#else
#define board_mux NULL
#endif

struct omap_mux_setting omap5432_common_mux[] __initdata = {
        {                 
                .name = "emmc_clk.emmc_clk",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                 
                .name = "emmc_cmd.emmc_cmd",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                 
                .name = "emmc_data0.emmc_data0",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                 
                .name = "emmc_data1.emmc_data1",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                 
                .name = "emmc_data2.emmc_data2",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                 
                .name = "emmc_data3.emmc_data3",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                 
                .name = "emmc_data4.emmc_data4",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                 
                .name = "emmc_data5.emmc_data5",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                 
                .name = "emmc_data6.emmc_data6",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
	{                 
                .name = "emmc_data7.emmc_data7",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                                                                       
                /* GPIO_112 mcpdm clk */                                        
                .name = "abemcpdm_lb_clk.abemcpdm_lb_clk",                      
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,                        
        },                                                                      
        {                                                                       
                /* GPIO_111 mcpdm frame */                                      
                .name = "abemcpdm_frame.abemcpdm_frame",                        
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,                        
        },                                                                      
        {                                                                       
                /* GPIO_110 mcpdm dl data */                                    
                .name = "abemcpdm_dl_data.abemcpdm_dl_data",                    
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,                        
        },                                                                      
        {                                                                       
                /* GPIO_109 mcpdm ul data */                                    
                .name = "abemcpdm_ul_data.abemcpdm_ul_data",                    
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,                        
        },
        {
                .name = "abe_clks.abe_clks",
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,
        },
};                                                                              

/*
 * These device paths represent the onboard USB <-> Ethernet bridge, and
 * the WLAN module on Panda, both of which need their random or all-zeros       
 * mac address replacing with a per-cpu stable generated one                    
 */
static const char * const omap5evm_fixup_mac_device_paths[] = {                    
       "1-3:1.0",
       "mmc2:0001:2",
};

static struct panel_lg4591_data dsi_panel;
static struct omap_dss_board_info omap5evm_dss_data;

static void omap5evm_lcd_init(void)
{
	int r;

	r = gpio_request_one(dsi_panel.reset_gpio, GPIOF_DIR_OUT,
		"lcd1_reset_gpio");
	if (r)
		pr_err("%s: Could not get lcd1_reset_gpio\n", __func__);
}

static void omap5evm_hdmi_init(void)
{
	int r;

	r = gpio_request_one(HDMI_GPIO_HPD, GPIOF_DIR_IN,
				"hdmi_gpio_hpd");
	if (r)
		pr_err("%s: Could not get HDMI\n", __func__);

        /* Requesting HDMI HPD_EN GPIO and enable it, at bootup */              
        r = gpio_request_one(HDMI_HPD_EN_GPIO,                                  
                        GPIOF_OUT_INIT_HIGH, "HDMI_HPD_EN");                    
        if (r)                                                                  
                pr_err("Failed to get HDMI HPD EN GPIO\n");

	/* Need to configure HPD as a gpio in mux */
	omap_hdmi_init(0);
}

static void __init omap5evm_display_init(void)
{
	omap5evm_lcd_init();
	omap5evm_hdmi_init();
	omap_display_init(&omap5evm_dss_data);
}

static void lg_panel_set_power(bool enable)
{
}

static struct panel_lg4591_data dsi_panel = {
	.reset_gpio = 183,
	.set_power = lg_panel_set_power,
	.pin_config = {
		.num_pins = 8,
		.pins = {0, 1, 2, 3, 4, 5, 6, 7},
	},
};

static struct omap_dss_device omap5evm_lcd_device = {
	.name			= "lcd",
	.driver_name		= "lg4591",
	.type			= OMAP_DISPLAY_TYPE_DSI,
	.data			= &dsi_panel,
	.clocks = {
		.dispc = {
			.channel = {
				.lck_div	= 1,	/* LCD */
				.pck_div	= 2,	/* PCD */
				.lcd_clk_src	= OMAP_DSS_CLK_SRC_DSI_PLL_HSDIV_DISPC,
			},
			.dispc_fclk_src = OMAP_DSS_CLK_SRC_DSI_PLL_HSDIV_DISPC,
		},
		.dsi = {
			.regn		= 19,	/* DSI_PLL_REGN */
			.regm		= 233,	/* DSI_PLL_REGM */

			.regm_dispc	= 3,	/* PLL_CLK1 (M4) */
			.regm_dsi	= 3,	/* PLL_CLK2 (M5) */
			.lp_clk_div	= 9,	/* LPDIV */

			.dsi_fclk_src	= OMAP_DSS_CLK_SRC_DSI_PLL_HSDIV_DSI,
		},
	},
	.panel.dsi_mode		= OMAP_DSS_DSI_VIDEO_MODE,
	.channel		= OMAP_DSS_CHANNEL_LCD,
};

static int omap5evm_panel_enable_hdmi(struct omap_dss_device *dssdev)
{
        int r;

	pr_info("omap5evm_panel_enable_hdmi\n");

	r = gpio_request_one(HDMI_HPD_EN_GPIO,                                  
                        GPIOF_OUT_INIT_HIGH, "HDMI_HPD_EN");

        r = gpio_request_one(HDMI_OE_GPIO,
                                GPIOF_OUT_INIT_HIGH, "HDMI_OE");
        if (r)
                pr_err("Failed to get HDMI OE GPIO\n");

        return 0;
}

static void omap5evm_panel_disable_hdmi(struct omap_dss_device *dssdev)
{
	pr_info("omap5evm_panel_disable_hdmi\n");

	gpio_set_value_cansleep(HDMI_OE_GPIO, 0);

	gpio_free(HDMI_OE_GPIO);
}

static struct omap_dss_hdmi_data sdp54xx_hdmi_data = {
        .hpd_gpio = HDMI_GPIO_HPD,
};

static struct omap_dss_device omap5evm_hdmi_device = {
	.name = "hdmi",
	.driver_name = "hdmi_panel",
	.type = OMAP_DISPLAY_TYPE_HDMI,
	.platform_enable = omap5evm_panel_enable_hdmi,
	.platform_disable = omap5evm_panel_disable_hdmi,
	.channel = OMAP_DSS_CHANNEL_DIGIT,
	.data = &sdp54xx_hdmi_data,
};

static struct omap_dss_device *omap5evm_dss_devices[] = {
	&omap5evm_lcd_device,
	&omap5evm_hdmi_device,
};

static struct omap_dss_board_info omap5evm_dss_data = {
	.num_devices	= ARRAY_SIZE(omap5evm_dss_devices),
	.devices	= omap5evm_dss_devices,
	.default_device	= &omap5evm_hdmi_device,
};

static void __init omap54xx_common_init(void)
{
        omap_mux_init_array(omap5432_common_mux,                                
                                              ARRAY_SIZE(omap5432_common_mux)); 

	omap_5430evm_i2c_init();
#ifdef CONFIG_WL12XX_PLATFORM_DATA
	omap5_sdp5430_wifi_init();
#endif
	omap_emif_set_device_details(1, &lpddr2_elpida_4G_S4_x2_info,
			lpddr2_elpida_4G_S4_timings,
			ARRAY_SIZE(lpddr2_elpida_4G_S4_timings),
			&lpddr2_elpida_S4_min_tck,
			&custom_configs);

	omap_emif_set_device_details(2, &lpddr2_elpida_4G_S4_x2_info,
			lpddr2_elpida_4G_S4_timings,
			ARRAY_SIZE(lpddr2_elpida_4G_S4_timings),
			&lpddr2_elpida_S4_min_tck,
			&custom_configs);

        omap_register_mac_device_fixup_paths(omap5evm_fixup_mac_device_paths,
                                  ARRAY_SIZE(omap5evm_fixup_mac_device_paths));

	omap_serial_board_init(NULL, 2);
	omap_serial_board_init(NULL, 4);
	omap_sdrc_init(NULL, NULL);
	omap_hsmmc_init(mmc);
	i2c_register_board_info(0, hdmi_i2c_eeprom, ARRAY_SIZE(hdmi_i2c_eeprom));
	platform_device_register(&hdmi_edid_device);
	omap_ehci_ohci_init();

	platform_add_devices(omap5evm_devices, ARRAY_SIZE(omap5evm_devices));
	omap5evm_display_init();
}

struct omap_mux_setting omap5432_sevm_mux[] __initdata = {                                   
        {                                                                       
                /* GPIO 172 - Ethernet bridge nRESET */
                .name = "rfbi_data6.gpio6_172",
                .mode = OMAP_PIN_OUTPUT | OMAP_MUX_MODE6,                 
        },                                                                      
        {                                                                       
                /* GPIO 173 - Hub nRESET */
		.name = "rfbi_data7.gpio6_173",
                .mode = OMAP_PIN_OUTPUT | OMAP_MUX_MODE6,                 
        },
};


/* camera regulators */
static struct omap_rprm_regulator omap5evm_rprm_regulators[] = {
	{
		.name = "cam2pwr",
		.fixed = true,
	},
	{
		.name = "cam2csi",
		.fixed = true,
	},
};

static void __init omap_5430_sevm_init(void)
{
	int status;

	pr_info("Starting 5430 sEVM setup\n");

	omap_mux_init_array(omap5432_sevm_mux, ARRAY_SIZE(omap5432_sevm_mux));

        /* Disable pulls on DCC lines - necessary for EDID detection */         
        omap_writel(0x50000000, 0x4A002E20);                                    
	
	omap5evm_touch_init();
        omap5evm_sensor_init();

	omap54xx_common_init();

	status = omap4_keyboard_init(&evm5430_keypad_data, &keypad_data);
	if (status)
		pr_err("Keypad initialization failed: %d\n", status);

        /* Disable pulls on DCC lines - necessary for EDID detection */         
        omap_writel(0x50000000, 0x4A002E20);

	/* camera regulators */
	omap_rprm_regulator_init(omap5evm_rprm_regulators,
					ARRAY_SIZE(omap5evm_rprm_regulators));
}

struct omap_mux_setting omap5432_uevm_mux[] __initdata = {
	{
		/* I2C1 / PMIC scl */
		.name = "i2c1_pmic_scl.i2c1_pmic_scl",
		.mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
	},
        {                                                                       
                /* I2C1 / PMIC sda */                                           
		.name = "i2c1_pmic_sda.i2c1_pmic_sda",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        }, 
        {                                                                       
                /* I2C5 / expander scl */                                           
                .name = "i2c5_scl.i2c5_scl",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },                                                                      
        {                                                                       
                /* I2C5 / expander sda */                                           
		.name = "i2c5_sda.i2c5_sda",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE0,
        },
        {                                                                       
                /* GPIO152 Card detect */                                           
		.name = "uart6_rts.gpio5_152",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE6,
        },
        {                                                                       
                /* GPIO80 USB host reset */
		.name = "hsi2_caflag.gpio3_80",                                       
                .mode = OMAP_PIN_OUTPUT,
        },
        {                                                                       
                /* GPIO 15 Ethernet reset */                                       
		.name = "llib_wakereqin.gpio1_wk15",
                .mode = OMAP_PIN_OUTPUT | OMAP_MUX_MODE6,
        },
        {                                                                       
                /* HDMI CEC */                                           
		.name = "hdmi_cec.hdmi_cec",
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,
        },                                                                      
        {                                                                       
                /* HDMI HPD */                                           
		.name = "hdmi_hpd.gpio7_193",
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE6,
        },
        {                                                                       
                /* GPIO 194 HDMI EDID BITBANG I2C scl */                                       
		.name = "hdmi_ddc_scl.gpio7_194",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE6,
        },                                                                      
        {                                                                       
                /* GPIO 195 HDMI EDID BITBANG I2C sda */
		.name = "hdmi_ddc_sda.gpio7_195",
                .mode = OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE6,
        },
        {                                                                       
                /* GPIO_94 USB HOST 2 HSIC - hub */                                                  
		.name = "usbb2_hsic_strobe.usbb2_hsic_strobe",
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,    
        },
        {                                                                       
                /* GPIO_95 USB HOST 2 HSIC - hub */                                                  
		.name = "usbb2_hsic_data.usbb2_hsic_data",
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,    
        },
        {                                                                       
                /* GPIO_158 USB HOST 3 HSIC - ethernet */                                                  
		.name = "usbb3_hsic_strobe.usbb3_hsic_strobe",
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,    
        },
        {                                                                       
                /* GPIO_159 USB HOST 3 HSIC - ethernet */
		.name = "usbb3_hsic_data.usbb3_hsic_data",
                .mode = OMAP_PIN_INPUT | OMAP_MUX_MODE0,    
        },
        {                                                                       
                /* FREF_CLK1_OUT - Usb hub clock */                       
		.name = "fref_clk1_out.fref_clk1_out",
                .mode = OMAP_PIN_OUTPUT,    
        },
        {                                                                       
                /* FREF_CLK0_OUT - Camera clock */                       
		.name = "fref_clk0_out.fref_clk0_out",
                .mode = OMAP_PIN_OUTPUT,    
        },
	{
                /* GPIO_141 AUDPWRON */                       
		.name = "mcspi1_somi.gpio5_141",
                .mode = OMAP_PIN_OUTPUT | OMAP_MUX_MODE6,
        },
};


static void __init omap_5432_uevm_init(void)
{
	struct clk *phy_ref_clk;

	pr_info("Starting 5432 uEVM setup");

	/* SD Card Detect */
	mmc[1].gpio_cd = 152;

	/* uEVM-specific ethernet resets */
	usbhs_bdata.reset_gpio_port[1] = GPIO_HUB_NRESET_UEVM;
	usbhs_bdata.reset_gpio_port[2] = GPIO_ETH_NRESET_UEVM;

	omap_mux_init_array(omap5432_uevm_mux, ARRAY_SIZE(omap5432_uevm_mux));

        /* FREF_CLK1 provides the 19.2 MHz reference clock to the PHY */        
        phy_ref_clk = clk_get(NULL, "auxclk1_ck");                              
        if (IS_ERR(phy_ref_clk)) {                                              
                pr_err("Cannot request auxclk1\n");
        } else {
	        clk_set_rate(phy_ref_clk, 19200000);                                    
	        clk_enable(phy_ref_clk);
	}

	/* WLAN module IRQ */
	gpio_wlan_irq = 14;

	/* AUDPWRON gpio */
	twl6040_data.audpwron_gpio = 141;

	omap54xx_common_init();
}

static void __init omap_54xx_init(void)
{

	// this is part of the hack patch around virtual mapping of dt blob

	extern char dt_selected_model[64];
#if 0
	const char *model;
	pr_err("omap_54xx_init in\n");
        model = of_get_flat_dt_prop(of_get_flat_dt_root(), "model", NULL);    
	BUG_ON(!model);
	pr_err("model: %s\n", model);
#endif

	omap5_mux_init(board_mux, NULL, OMAP_PACKAGE_CBL);

	if (!strcmp(dt_selected_model, "TI OMAP5 uEVM"))
		omap_5432_uevm_init();
	else
		omap_5430_sevm_init();
}

static void __init omap_5430evm_map_io(void)
{
	pr_info("omap_5430evm_map_io\n");
//	omap2_set_globals_543x();
	omap54xx_map_common_io();
}

static const char *omap5_sevm_match[] __initdata = {
        "ti,omap5-sevm",
	"ti,omap5-uevm",
        NULL,
};

static void __init omap_5430evm_reserve(void)
{
	omap_rproc_reserve_cma(RPROC_CMA_OMAP5);
	omap_reserve();
}

MACHINE_START(OMAP5_SEVM, "OMAP5430 evm board")
	/* Maintainer: Santosh Shilimkar - Texas Instruments Inc */
	.atag_offset	= 0x100,
	.reserve	= omap_5430evm_reserve,
	.map_io		= omap_5430evm_map_io,
	.init_early	= omap54xx_init_early,
	.init_irq	= gic_init_irq,
	.handle_irq     = gic_handle_irq,
	.init_machine	= omap_54xx_init,
	.restart	= omap_prcm_restart,
	.timer		= &omap5_timer,
	.dt_compat	= omap5_sevm_match,
MACHINE_END


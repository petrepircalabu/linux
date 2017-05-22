/*
 * TI CDCM6208 2:8 Ultra Low Power, Low Jitter Clock Generator
 *
 * Copyright (C) 2017 Petre Pircalabu <petre.pircalabu@gmail.com>.
 *
 * Reference: http://www.ti.com/lit/ds/symlink/cdcm6208.pdf
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define DEBUG 1

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/gcd.h>

#define CDCM6208_INPUT_CLKS 	2

struct clk_cdcm6208_dev{
	struct regmap *regmap;
	struct i2c_client *i2c_client;
	struct clk *clks[CDCM6208_INPUT_CLKS];
};

static int cdcm6208_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct clk_cdcm6208_dev *pdev;
	const char *clk_names[] = {
		"primary",
		"secondary",
	};
	int i;

	dev_dbg(&client->dev, "%s\n", __func__);
	pdev = devm_kzalloc(&client->dev, sizeof(*pdev), GFP_KERNEL);
	if (!pdev)
		return -ENOMEM;

	pdev->i2c_client = client;

	for (i=0; i<CDCM6208_INPUT_CLKS; i++) {
		pdev->clks[i] = devm_clk_get(&client->dev, clk_names[i]);
	}

	pdev->clks[0] = devm_clk_get(&client->dev, "primary_clk");
	pdev->clks[1] = devm_clk_get(&client->dev, "secondary_clk");

	return 0;
}

static const struct i2c_device_id cdcm6208_id[] = {
	{ "cdcm6208", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, cdcm6208_id);

static const struct of_device_id clk_cdcm6208_of_match[] = {
	{ .compatible = "ti,cdcm6208" },
	{ },
};
MODULE_DEVICE_TABLE(of, clk_cdcm6208_of_match);

static struct i2c_driver cdcm6208_driver = {
	.driver = {
		.name = "cdcm6208",
		.of_match_table = of_match_ptr(clk_cdcm6208_of_match),
	},
	.probe		= cdcm6208_probe,
	.id_table	= cdcm6208_id,
};
module_i2c_driver(cdcm6208_driver);

MODULE_AUTHOR("Petre Pircalabu <petre.pircalabu@gmail.com");
MODULE_DESCRIPTION("TI CDCM6208 driver");
MODULE_LICENSE("GPL");

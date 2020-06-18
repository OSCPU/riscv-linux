/*
 * Xilinx SLCR driver
 *
 * Copyright (c) 2011-2013 Xilinx Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 * 02139, USA.
 */

#include <linux/io.h>
#include <linux/reboot.h>
#include <linux/mfd/syscon.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/clk/zynq.h>

/* register offsets */
#define SLCR_UNLOCK_OFFSET		0x8   /* SCLR unlock register */
#define SLCR_PS_RST_CTRL_OFFSET		0x200 /* PS Software Reset Control */
#define SLCR_A9_CPU_RST_CTRL_OFFSET	0x244 /* CPU Software Reset Control */
#define SLCR_REBOOT_STATUS_OFFSET	0x258 /* PS Reboot Status */
#define SLCR_PSS_IDCODE			0x530 /* PS IDCODE */
#define SLCR_L2C_RAM			0xA1C /* L2C_RAM in AR#54190 */

#define SLCR_UNLOCK_MAGIC		0xDF0D
#define SLCR_A9_CPU_CLKSTOP		0x10
#define SLCR_A9_CPU_RST			0x1
#define SLCR_PSS_IDCODE_DEVICE_SHIFT	12
#define SLCR_PSS_IDCODE_DEVICE_MASK	0x1F

static void __iomem *zynq_slcr_base;
static struct regmap *zynq_slcr_regmap;

/**
 * zynq_slcr_write - Write to a register in SLCR block
 *
 * @val:	Value to write to the register
 * @offset:	Register offset in SLCR block
 *
 * Return:	a negative value on error, 0 on success
 */
static int zynq_slcr_write(u32 val, u32 offset)
{
	return regmap_write(zynq_slcr_regmap, offset, val);
}

/**
 * zynq_slcr_unlock - Unlock SLCR registers
 *
 * Return:	a negative value on error, 0 on success
 */
static inline int zynq_slcr_unlock(void)
{
	zynq_slcr_write(SLCR_UNLOCK_MAGIC, SLCR_UNLOCK_OFFSET);

	return 0;
}

/**
 * zynq_early_slcr_init - Early slcr init function
 *
 * Return:	0 on success, negative errno otherwise.
 *
 * Called very early during boot from platform code to unlock SLCR.
 */
int __init zynq_early_slcr_init(void)
{
	struct device_node *np;

	np = of_find_compatible_node(NULL, NULL, "xlnx,zynq-slcr");
	if (!np) {
		pr_err("%s: no slcr node found\n", __func__);
		BUG();
	}

	zynq_slcr_base = of_iomap(np, 0);
	if (!zynq_slcr_base) {
		pr_err("%s: Unable to map I/O memory\n", __func__);
		BUG();
	}

	np->data = (__force void *)zynq_slcr_base;

	zynq_slcr_regmap = syscon_regmap_lookup_by_compatible("xlnx,zynq-slcr");
	if (IS_ERR(zynq_slcr_regmap)) {
		pr_err("%s: failed to find zynq-slcr\n", __func__);
		return -ENODEV;
	}

	/* unlock the SLCR so that registers can be changed */
	zynq_slcr_unlock();

	pr_info("%s mapped to %p\n", np->name, zynq_slcr_base);

	of_node_put(np);

	return 0;
}


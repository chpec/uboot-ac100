/*
 * (C) Copyright 2010 
 * NVIDIA Corporation <www.nvidia.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/sys_proto.h>

/******************************************
 * get_cpu_type(void) - extract cpu info
 ******************************************/
u32 get_cpu_type(void)
{
	return 0;
}

/******************************************
 * get_cpu_rev(void) - extract version info
 ******************************************/
u32 get_cpu_rev(void)
{
	u32 cpuid = 0;
	return cpuid;
}

/*************************************************************************
 * get_board_rev() - setup to pass kernel board revision information
 * returns:(bit[0-3] sub version, higher bit[7-4] is higher version)
 *************************************************************************/
u32 get_board_rev(void)
{
	return 0;
}

/*************************************************************
 *  get_device_type(): tell if GP/HS/EMU/TST
 *************************************************************/
u32 get_device_type(void)
{
	return 0;
}

#ifdef CONFIG_DISPLAY_CPUINFO
/**
 * Print CPU information
 */
int print_cpuinfo (void)
{
	char *cpu_s;

	cpu_s = "";

	printf("TEGRA2 %s\n",
			cpu_s);

	return 0;
}
#endif	/* CONFIG_DISPLAY_CPUINFO */

/*
 *  (C) Copyright 2010
 *  NVIDIA Corporation <www.nvidia.com>
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

#ifndef __MACH_TEGRA_PINMUX_T2_H
#define __MACH_TEGRA_PINMUX_T2_H

enum tegra_pingroup {
	TEGRA_PINGROUP_ATA = 0,
	TEGRA_PINGROUP_ATB,
	TEGRA_PINGROUP_ATC,
	TEGRA_PINGROUP_ATD,
	TEGRA_PINGROUP_ATE,
	TEGRA_PINGROUP_CDEV1,
	TEGRA_PINGROUP_CDEV2,
	TEGRA_PINGROUP_CRTP,
	TEGRA_PINGROUP_CSUS,
	TEGRA_PINGROUP_DAP1,
	TEGRA_PINGROUP_DAP2,
	TEGRA_PINGROUP_DAP3,
	TEGRA_PINGROUP_DAP4,
	TEGRA_PINGROUP_DDC,
	TEGRA_PINGROUP_DTA,
	TEGRA_PINGROUP_DTB,
	TEGRA_PINGROUP_DTC,
	TEGRA_PINGROUP_DTD,
	TEGRA_PINGROUP_DTE,
	TEGRA_PINGROUP_DTF,
	TEGRA_PINGROUP_GMA,
	TEGRA_PINGROUP_GMB,
	TEGRA_PINGROUP_GMC,
	TEGRA_PINGROUP_GMD,
	TEGRA_PINGROUP_GME,
	TEGRA_PINGROUP_GPU,
	TEGRA_PINGROUP_GPU7,
	TEGRA_PINGROUP_GPV,
	TEGRA_PINGROUP_HDINT,
	TEGRA_PINGROUP_I2CP,
	TEGRA_PINGROUP_IRRX,
	TEGRA_PINGROUP_IRTX,
	TEGRA_PINGROUP_KBCA,
	TEGRA_PINGROUP_KBCB,
	TEGRA_PINGROUP_KBCC,
	TEGRA_PINGROUP_KBCD,
	TEGRA_PINGROUP_KBCE,
	TEGRA_PINGROUP_KBCF,
	TEGRA_PINGROUP_LCSN,
	TEGRA_PINGROUP_LD0,
	TEGRA_PINGROUP_LD1,
	TEGRA_PINGROUP_LD10,
	TEGRA_PINGROUP_LD11,
	TEGRA_PINGROUP_LD12,
	TEGRA_PINGROUP_LD13,
	TEGRA_PINGROUP_LD14,
	TEGRA_PINGROUP_LD15,
	TEGRA_PINGROUP_LD16,
	TEGRA_PINGROUP_LD17,
	TEGRA_PINGROUP_LD2,
	TEGRA_PINGROUP_LD3,
	TEGRA_PINGROUP_LD4,
	TEGRA_PINGROUP_LD5,
	TEGRA_PINGROUP_LD6,
	TEGRA_PINGROUP_LD7,
	TEGRA_PINGROUP_LD8,
	TEGRA_PINGROUP_LD9,
	TEGRA_PINGROUP_LDC,
	TEGRA_PINGROUP_LDI,
	TEGRA_PINGROUP_LHP0,
	TEGRA_PINGROUP_LHP1,
	TEGRA_PINGROUP_LHP2,
	TEGRA_PINGROUP_LHS,
	TEGRA_PINGROUP_LM0,
	TEGRA_PINGROUP_LM1,
	TEGRA_PINGROUP_LPP,
	TEGRA_PINGROUP_LPW0,
	TEGRA_PINGROUP_LPW1,
	TEGRA_PINGROUP_LPW2,
	TEGRA_PINGROUP_LSC0,
	TEGRA_PINGROUP_LSC1,
	TEGRA_PINGROUP_LSCK,
	TEGRA_PINGROUP_LSDA,
	TEGRA_PINGROUP_LSDI,
	TEGRA_PINGROUP_LSPI,
	TEGRA_PINGROUP_LVP0,
	TEGRA_PINGROUP_LVP1,
	TEGRA_PINGROUP_LVS,
	TEGRA_PINGROUP_OWC,
	TEGRA_PINGROUP_PMC,
	TEGRA_PINGROUP_PTA,
	TEGRA_PINGROUP_RM,
	TEGRA_PINGROUP_SDB,
	TEGRA_PINGROUP_SDC,
	TEGRA_PINGROUP_SDD,
	TEGRA_PINGROUP_SDIO1,
	TEGRA_PINGROUP_SLXA,
	TEGRA_PINGROUP_SLXC,
	TEGRA_PINGROUP_SLXD,
	TEGRA_PINGROUP_SLXK,
	TEGRA_PINGROUP_SPDI,
	TEGRA_PINGROUP_SPDO,
	TEGRA_PINGROUP_SPIA,
	TEGRA_PINGROUP_SPIB,
	TEGRA_PINGROUP_SPIC,
	TEGRA_PINGROUP_SPID,
	TEGRA_PINGROUP_SPIE,
	TEGRA_PINGROUP_SPIF,
	TEGRA_PINGROUP_SPIG,
	TEGRA_PINGROUP_SPIH,
	TEGRA_PINGROUP_UAA,
	TEGRA_PINGROUP_UAB,
	TEGRA_PINGROUP_UAC,
	TEGRA_PINGROUP_UAD,
	TEGRA_PINGROUP_UCA,
	TEGRA_PINGROUP_UCB,
	TEGRA_PINGROUP_UDA,
	/* these pin groups only have pullup and pull down control */
	TEGRA_PINGROUP_CK32,
	TEGRA_PINGROUP_DDRC,
	TEGRA_PINGROUP_PMCA,
	TEGRA_PINGROUP_PMCB,
	TEGRA_PINGROUP_PMCC,
	TEGRA_PINGROUP_PMCD,
	TEGRA_PINGROUP_PMCE,
	TEGRA_PINGROUP_XM2C,
	TEGRA_PINGROUP_XM2D,
	TEGRA_MAX_PINGROUP,
};

#endif /*__MACH_TEGRA_PINMUX_T2_H*/
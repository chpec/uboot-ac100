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

#include <common.h>
#include "cpu.h"
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/nv_hardware_access.h>

#ifdef CONFIG_CMDLINE
extern ulong ATAGS_START;
char cmdline_copy[CMDLINE_MAX_LENGTH];
#endif

/*
 * Boot ROM initialize the odmdata in APBDEV_PMC_SCRATCH20_0, 
 * so we are using this value to identify memory size. 
 */

static unsigned int NvQuerySdramSize(void)
{
	/* 31:28 bits in OdmData used for ram size  */
	switch(NV_READ32(AP20_PMC_BASE + APBDEV_PMC_SCRATCH20_0)>>28)
	{
		case 1:
			 return(0x10000000); /* 256 MB */
		case 3:
			 return(0x40000000); /* 1GB */
		case 2:
		default:
			return(0x20000000); /* 512 MB */
	}
}

int arch_cpu_init(void)
{
#ifndef CONFIG_TEGRA2_NO_RM
#ifdef CONFIG_CMDLINE
        struct tag *tag;
#endif

#ifdef CONFIG_CMDLINE
        /* Copy over the command line from ATAGs to be used later */
        for_each_tag (tag, (struct tag *)ATAGS_START) {
                if (tag->hdr.tag != ATAG_CMDLINE)
                        continue;

                strncpy(cmdline_copy, tag->u.cmdline.cmdline, CMDLINE_MAX_LENGTH);
        }
#endif
#endif // CONFIG_TEGRA2_NO_RM 

	return 0;
}

gd_t *gd;

extern tegra2_sysinfo sysinfo;

/******************************************************************************
 * Routine: mux_init
 * Description: Does early system init of muxing.
 *****************************************************************************/
static void mux_init(void)
{
}

/******************************************************************************
 * Routine: per_init
 * Description: Does early system init of peripherals.
 *****************************************************************************/
static void per_init(void)
{
}

/******************************************************************************
 * Routine: s_init
 * Description: Does early system init of muxing and clocks.
 *              - Called path is with SRAM stack.
 *****************************************************************************/
void invalidate_dcache(u32);

void s_init(void)
{
#ifndef CONFIG_ICACHE_OFF
	icache_enable();
#endif

	invalidate_dcache(get_device_type());

	/* Init pin mux */
	mux_init();

	/* Init peripherals */
	per_init();
}

/******************************************************************************
 * Routine: dram_init
 * Description: sets uboots idea of sdram size
 *****************************************************************************/
int dram_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	/* We do not initialise DRAM here. We just query the size here */
	gd->bd->bi_dram[0].start = APXX_EXT_MEM_START;
	gd->bd->bi_dram[0].size = NvQuerySdramSize(); 
	return 0;
}

/******************************************************************************
 * Dummy function to handle errors for EABI incompatibility
 *****************************************************************************/
#if 0  
void raise(void)
{
}
#endif
/******************************************************************************
 * Dummy function to handle errors for EABI incompatibility
 *****************************************************************************/
void abort(void)
{
}

#ifdef CONFIG_DISPLAY_BOARDINFO
/**
 * Print board information
 */
int checkboard (void)
{
	printf("Board:   %s\n", sysinfo.board_string);
	return 0;
}
#endif	/* CONFIG_DISPLAY_BOARDINFO */

int cpu_eth_init(bd_t *bis)
{
#ifdef CONFIG_USB_ETHER
#ifdef CONFIG_USB_TEGRA2
	tegra2_udc_probe();
#endif
	return usb_eth_initialize(bis);
#else
	return -1;
#endif
}


#ifdef CONFIG_CMDLINE
static void setenv_default(char *envname)
{
	if(!strcmp("console",envname))
		setenv(envname,"ttyS0,57600n8");
	else if(!strcmp("videospec",envname))
		setenv(envname,"tegrafb");
	else if(!strcmp("tegraparts",envname))
		setenv(envname,TEGRAPARTS_DEFAULT);
	else if(!strcmp("tegraboot",envname))
		setenv(envname,TEGRABOOT_DEFAULT);
	else if(!strcmp("nfsport",envname))
		setenv(envname,TEGRA_NFSPORT_DEFAULT);
	else if(!strcmp("smpflag",envname))
		setenv(envname," ");
	else if(!strcmp("rootdelay",envname))
		setenv(envname,"15");	
	else if (!strcmp("mem",envname)){
		switch(gd->bd->bi_dram[0].size) {
			case 0x10000000: setenv("mem","192M@0M");
					 break;
			case 0x40000000: setenv("mem","448M@0M mem=512M@512M");
					 break;
			case 0x20000000: setenv("mem","448M@0M");
			default: setenv("mem","448M@0M");
		}
	}else
		printf("Error setting %s env variable \n",envname);

}

static void setenv_private(char *parse_str, char *envname)
{
	char *Ptr1, *Ptr2, *tempPtr; 
	char temp_cmd[CMDLINE_MAX_LENGTH];	
	char mem_string[30];
	
	/* take fastboot env value only if the uboot env value is null */
	if(getenv(envname))
		return;
	strcpy(	temp_cmd, cmdline_copy);
	tempPtr = strstr(temp_cmd, parse_str);
	if (tempPtr){
		if(!strcmp(parse_str,"nosmp")){
			setenv("smpflag","nosmp");
			return;
		}

		tempPtr += strlen(parse_str);
		if(!strcmp(parse_str, "ip=::::")){
			Ptr1 = strtok(tempPtr,":");
			Ptr1--;
		}
		else 
			Ptr1 = strtok(tempPtr," ");
		
		if(!strcmp(parse_str,"mem")) {
			/*
			 * strtok keep \0 at the end of the token, next search need to 
			 * start after that
			 */
			tempPtr = strstr(tempPtr+strlen(Ptr1)+1, parse_str);
			if(tempPtr) {
				Ptr2 = strtok(tempPtr, " ");
				if (Ptr2 != NULL){
					sprintf(mem_string,"%s %s",Ptr1+1,Ptr2);
					setenv(envname, mem_string);
					return;
				}
			}
		}
		if(Ptr1 != NULL)
			setenv(envname,Ptr1+1);
	}else {
		setenv_default(envname);
	}

}

void parse_fastboot_cmd()
{
	setenv_private("tegrapart", "tegraparts");
	setenv_private("tegraboot", "tegraboot");
	setenv_private("mem","mem");
	setenv_private("ip=::::", "nfsport");
	setenv_private("video", "videospec");
	setenv_private("console", "console");
	setenv_private("rootdelay","rootdelay");
	setenv_private("nosmp","smpflag");
}
#endif

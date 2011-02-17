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
#include <nand.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-types.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/nv_hardware_access.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/tegra2.h>
#include "sdmmc/nvboot_clocks_int.h"
#include "board.h"
#include <asm/arch/gpio.h>

/******************************************************************************
 * PLL CONFIGURATION & PARAMETERS for different clock generators:
 *-----------------------------------------------------------------------------
 * Reference frequency     13.0MHz         19.2MHz         12.0MHz     26.0MHz
 * ----------------------------------------------------------------------------
 * PLLU_ENABLE_DLY_COUNT   02 (02h)        03 (03h)        02 (02h)    04 (04h)
 * PLLU_STABLE_COUNT       51 (33h)        75 (4Bh)        47 (2Fh)   102 (66h)
 * PLL_ACTIVE_DLY_COUNT    05 (05h)        06 (06h)        04 (04h)    09 (09h)
 * XTAL_FREQ_COUNT        127 (7Fh)       187 (BBh)       118 (76h)   254 (FEh)
 *****************************************************************************/
static const UsbPllDelayParams s_UsbPllDelayParams[NvBootClocksOscFreq_Num] =
{
    /* ENABLE_DLY,  STABLE_CNT,  ACTIVE_DLY,  XTAL_FREQ_CNT */
    {0x02,         0x33,        0x05,        0x7F}, /* For ClocksOscFreq_13, */
    {0x03,         0x4B,        0x06,        0xBB}, /* For ClocksOscFreq_19_2*/
    {0x02,         0x2F,        0x04,        0x76}, /* For ClocksOscFreq_12 */
    {0x04,         0x66,        0x09,        0xFE}  /* For ClocksOscFreq_26 */
};

/******************************************************************************
 * PLLU configuration information (reference clock is osc/clk_m and PLLU-FOs 
 * are fixed at 12MHz/60MHz/480MHz).
 *
 *  reference frequency     13.0MHz      19.2MHz      12.0MHz      26.0MHz
 *  ----------------------------------------------------------------------
 *      DIVN                960 (3c0h)   200 (0c8h)   960 (3c0h)   960 (3c0h)
 *      DIVM                13 ( 0dh)      4 ( 04h)    12 ( 0ch)    26 ( 1ah)
 * Filter frequency (MHz)   1            4.8            6            2
 * CPCON                    1100b        0011b        1100b       1100b
 * LFCON0                   0            0            0           0
 *****************************************************************************/
static const UsbPllClockParams s_UsbPllBaseInfo[NvBootClocksOscFreq_Num] =
{
    /* DivN, DivM, DivP, CPCON,  LFCON */
    {0x3C0, 0x0D, 0x00, 0xC,      0}, /* For NvBootClocksOscFreq_13, */
    {0x0C8, 0x04, 0x00, 0x3,      0}, /* For NvBootClocksOscFreq_19_2 */
    {0x3C0, 0x0C, 0x00, 0xC,      0}, /* For NvBootClocksOscFreq_12 */
    {0x3C0, 0x1A, 0x00, 0xC,      0}  /* For NvBootClocksOscFreq_26 */
};

/******************************************************************************
 * Debounce values IdDig, Avalid, Bvalid, VbusValid, VbusWakeUp, and SessEnd.
 * Each of these signals have their own debouncer and for each of those one out
 * of 2 debouncing times can be chosen (BIAS_DEBOUNCE_A or BIAS_DEBOUNCE_B.)
 *
 * The values of DEBOUNCE_A and DEBOUNCE_B are calculated as follows:
 * 0xffff -> No debouncing at all
 * <n> ms = <n> *1000 / (1/19.2MHz) / 4
 * So to program a 1 ms debounce for BIAS_DEBOUNCE_A, we have:
 * BIAS_DEBOUNCE_A[15:0] = 1000 * 19.2 / 4  = 4800 = 0x12c0
 * We need to use only DebounceA for BOOTROM. We dont need the DebounceB
 * values, so we can keep those to default.
 *****************************************************************************/
static const NvU32 s_UsbBiasDebounceATime[NvBootClocksOscFreq_Num] =
{
    /* Ten milli second delay for BIAS_DEBOUNCE_A */
    0x7EF4,  /* For NvBootClocksOscFreq_13, */
    0xBB80,  /* For NvBootClocksOscFreq_19_2 */
    0x7530,  /* For NvBootClocksOscFreq_12 */
    0xFDE8   /* For NvBootClocksOscFreq_26 */
};

static const NvU32 s_UsbBiasTrkLengthTime[NvBootClocksOscFreq_Num] =
{
    /* 20 micro seconds delay after bias cell operation */
    5,  /* For NvBootClocksOscFreq_13, */
    7,  /* For NvBootClocksOscFreq_19_2 */
    5,  /* For NvBootClocksOscFreq_12 */
    9   /* For NvBootClocksOscFreq_26 */
};

/* UTMIP Idle Wait Delay */
static const NvU8 s_UtmipIdleWaitDelay    = 17;
/* UTMIP Elastic limit */
static const NvU8 s_UtmipElasticLimit     = 16;
/* UTMIP High Speed Sync Start Delay */
static const NvU8 s_UtmipHsSyncStartDelay = 9;

void board_usb_init(void);
void board_spi_init(void);

/*
 * Routine: board_init
 * Description: Early hardware init.
 */
int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	/* boot param addr */
	gd->bd->bi_boot_params = (NV_ADDRESS_MAP_SDRAM_BASE + 0x100);
	/* board id for Linux */
	gd->bd->bi_arch_number = LINUX_MACH_TYPE;

#ifdef CONFIG_TEGRA2_LCD
	gd->fb_base = LCD_FB_ADDR;
#endif

	board_spi_init();		/* do this early so UART mux is OK */
	board_usb_init();

	return 0;
}

/*
 * Routine: misc_init_r
 * Description: Configure board specific parts
 */
int misc_init_r(void)
{
	return 0;
}

/*
 * Routine: timer_init
 *
 * Description: init the timestamp and lastinc value
 *
 */
int timer_init(void)
{
    reset_timer();
    return 0;
}

/*
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers specific to the
 *		hardware. Many pins need to be moved from protect to primary
 *		mode.
 */
void set_muxconf_regs(void)
{
}

/***************************************************************************
 * Routines to be provided by corresponding device drivers.
 ***************************************************************************/
int board_nand_init(struct nand_chip *nand)
{
    return -1;
}

/***************************************************************************
 * Routines for UART initialization.
 ***************************************************************************/
void
NvBlAvpClockSetDivider(NvBool Enable, NvU32 Dividened, NvU32 Divisor)
{
    NvU32 val;

    if (Enable)
    {
        /* Set up divider for SCLK. */
        /* SCLK is used for AVP, AHB, and APB. */
        val = NV_DRF_DEF(CLK_RST_CONTROLLER, SUPER_SCLK_DIVIDER,
                         SUPER_SDIV_ENB, ENABLE)
              | NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_SCLK_DIVIDER,
                           SUPER_SDIV_DIVIDEND, Dividened - 1)
              | NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_SCLK_DIVIDER,
                           SUPER_SDIV_DIVISOR, Divisor - 1);
        NV_CLK_RST_WRITE(SUPER_SCLK_DIVIDER, val);
    }
    else
    {
        /* Disable divider for SCLK. */
        val = NV_DRF_DEF(CLK_RST_CONTROLLER, SUPER_SCLK_DIVIDER,
                         SUPER_SDIV_ENB, DISABLE);
        NV_CLK_RST_WRITE(SUPER_SCLK_DIVIDER, val);
    }
}

static char s_Hex2Char[] =
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
};

void NvBlUartClockInit(NvU32 reset_register,
		       NvU32 reset_mask,
		       NvU32 reset_enable,
		       NvU32 reset_disable,
		       NvU32 clock_register,
		       NvU32 clock_mask,
		       NvU32 clock_enable,
		       NvU32 clock_source_register,
		       NvU32 clock_source)
{
    NvU32 Reg;

    // 1. Assert Reset to UART D
    Reg = NV_READ32(NV_ADDRESS_MAP_PPSB_CLK_RST_BASE + reset_register);
    Reg = (Reg & ~reset_mask) | reset_enable;
    NV_WRITE32(NV_ADDRESS_MAP_PPSB_CLK_RST_BASE + reset_register, Reg);

    // 2. Enable clk to UART D
    Reg = NV_READ32(NV_ADDRESS_MAP_PPSB_CLK_RST_BASE + clock_register);
    Reg = (Reg & ~clock_mask) | clock_enable;
    NV_WRITE32(NV_ADDRESS_MAP_PPSB_CLK_RST_BASE + clock_register, Reg);

    // Override pllp setup for 216MHz operation.
    Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_BYPASS, ENABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_ENABLE, DISABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_REF_DIS, REF_ENABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_BASE_OVRRIDE, ENABLE)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_LOCK, 0x0)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVP, 0x1)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVN,
                       NVRM_PLLP_FIXED_FREQ_KHZ/500)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVM, 0x0C);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE,
                             PLLP_ENABLE, ENABLE, Reg);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE,
                             PLLP_BYPASS, DISABLE, Reg);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    // Enable pllp_out0 to UARTD.
    NV_WRITE32(NV_ADDRESS_MAP_PPSB_CLK_RST_BASE + clock_source_register,
	       clock_source);

    // wait for 2us
    NvBlAvpStallUs(2);

    // De-assert reset to UART D
    Reg = NV_READ32(NV_ADDRESS_MAP_PPSB_CLK_RST_BASE + reset_register);
    Reg = (Reg & ~reset_mask) | reset_disable;
    NV_WRITE32(NV_ADDRESS_MAP_PPSB_CLK_RST_BASE + reset_register, Reg);
}

void NvBlUartInitBase(NvU8 * uart_base)
{
    NvU32 divisor = (NVRM_PLLP_FIXED_FREQ_KHZ * 1000 /
		     NV_DEFAULT_DEBUG_BAUD / 16);

    // Set up UART parameters.
    NV_WRITE08(uart_base + UART_LCR_0,        0x80);
    NV_WRITE08(uart_base + UART_THR_DLAB_0_0, divisor);
    NV_WRITE08(uart_base + UART_IER_DLAB_0_0, 0x00);
    NV_WRITE08(uart_base + UART_LCR_0,        0x00);
    NV_WRITE08(uart_base + UART_IIR_FCR_0,    0x37);
    NV_WRITE08(uart_base + UART_IER_DLAB_0_0, 0x00);
    NV_WRITE08(uart_base + UART_LCR_0,        0x03); /* 8N1 */
    NV_WRITE08(uart_base + UART_MCR_0,        0x02);
    NV_WRITE08(uart_base + UART_MSR_0,        0x00);
    NV_WRITE08(uart_base + UART_SPR_0,        0x00);
    NV_WRITE08(uart_base + UART_IRDA_CSR_0,   0x00);
    NV_WRITE08(uart_base + UART_ASR_0,        0x00);

    NV_WRITE08(uart_base + UART_IIR_FCR_0,    0x31);

    // Flush any old characters out of the RX FIFO.
    while (NvBlUartRxReady(uart_base))
        (void)NvBlUartRx(uart_base);
}

static NV_INLINE NvU32 NvBlUartTxReady(NvU8 const * uart_base)
{
    return NV_READ8(uart_base + UART_LSR_0) & UART_LSR_0_THRE_FIELD;
}

NvU32 NvBlUartRxReady(NvU8 const * uart_base)
{
    return NV_READ8(uart_base + UART_LSR_0) & UART_LSR_0_RDR_FIELD;
}

static NV_INLINE void NvBlUartTx(NvU8 * uart_base, NvU8 c)
{
    NV_WRITE08(uart_base + UART_THR_DLAB_0_0, c);
}

NvU32 NvBlUartRx(NvU8 const * uart_base)
{
    return NV_READ8(uart_base + UART_THR_DLAB_0_0);
}

int NvBlUartPoll(void)
{
    if (NvBlUartRxReady((NvU8 *)NV_ADDRESS_MAP_APB_UARTA_BASE))
        return NvBlUartRx((NvU8 *) NV_ADDRESS_MAP_APB_UARTA_BASE);

    if (NvBlUartRxReady((NvU8 *)NV_ADDRESS_MAP_APB_UARTB_BASE))
        return NvBlUartRx((NvU8 *)NV_ADDRESS_MAP_APB_UARTB_BASE);

    if (NvBlUartRxReady((NvU8 *)NV_ADDRESS_MAP_APB_UARTD_BASE))
        return NvBlUartRx((NvU8 *)NV_ADDRESS_MAP_APB_UARTD_BASE);

    return -1;
}

int NvBlUartWrite(const void *ptr);
void NvBlPrintf(const char *format, ...);
void uart_post(char c);

static void
NvBlPrintU4(NvU8 byte)
{
    uart_post(s_Hex2Char[byte & 0xF]);
}

void
NvBlPrintU8(NvU8 byte)
{
    NvBlPrintU4((byte >> 4) & 0xF);
    NvBlPrintU4((byte >> 0) & 0xF);
}

void
NvBlPrintU32(NvU32 word)
{
    NvBlPrintU8((word >> 24) & 0xFF);
    NvBlPrintU8((word >> 16) & 0xFF);
    NvBlPrintU8((word >>  8) & 0xFF);
    NvBlPrintU8((word >>  0) & 0xFF);
}

void
NvBlVprintf(const char *format, va_list ap)
{
    char msg[256];
    sprintf(msg, format, ap);
    NvBlUartWrite(msg);
}

void
NvBlPrintf(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    NvBlVprintf(format, ap);
    va_end(ap);
}

void uart_post(char c)
{
#if defined(TEGRA2_TRACE)

#if (CONFIG_TEGRA2_ENABLE_UARTA)
    while (!NvBlUartTxReady((NvU8 *)NV_ADDRESS_MAP_APB_UARTA_BASE))
        ;
    NvBlUartTx((NvU8 *)NV_ADDRESS_MAP_APB_UARTA_BASE, c);
#endif

#if (CONFIG_TEGRA2_ENABLE_UARTB)
    while (!NvBlUartTxReady((NvU8 *)NV_ADDRESS_MAP_APB_UARTB_BASE))
        ;
    NvBlUartTx((NvU8 *)NV_ADDRESS_MAP_APB_UARTB_BASE, c);
#endif

#if (CONFIG_TEGRA2_ENABLE_UARTD)
    while (!NvBlUartTxReady((NvU8 *)NV_ADDRESS_MAP_APB_UARTD_BASE))
        ;
    NvBlUartTx((NvU8 *)NV_ADDRESS_MAP_APB_UARTD_BASE, c);
#endif

#endif
}

void PostZz(void)
{
    uart_post(0x0d);
    uart_post(0x0a);
    uart_post('Z');
    uart_post('z');

    NvBlAvpStallUs(2000);
}

void PostYy(void)
{
    NvBlAvpStallMs(20);
    uart_post(0x0d);
    uart_post(0x0a);
    uart_post('Y');
    uart_post('y');
}

void PostXx(void)
{
    NvBlAvpStallMs(20);
    uart_post(0x0d);
    uart_post(0x0a);
    uart_post('X');
    uart_post('x');
    uart_post(0x0d);
    uart_post(0x0a);
}

int
NvBlUartWrite(const void *ptr)
{
    const NvU8 *p = ptr;

    while (*p)
    {
        if (*p == '\n') {
            uart_post(0x0D);
        }
        uart_post(*p);
        p++;
    }
    return 0;
}

void debug_trace(int i)
{
    uart_post(i+'a');
    uart_post('.');
    uart_post('.');
}

void usbf_reset_controller(NvU32 UsbBase)
{
	int RegVal = 0;

	if (UsbBase == NV_ADDRESS_MAP_USB_BASE)
	{
		/* Enable clock to the USB controller */
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0);
		RegVal |= Bit22;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0);

		/* Reset the USB controller */
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_RST_DEVICES_L_0);
		RegVal |= Bit22;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_RST_DEVICES_L_0);
		udelay(2);
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_RST_DEVICES_L_0);
		RegVal &= ~Bit22;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_RST_DEVICES_L_0);
		udelay(2);

		/* Set USB1_NO_LEGACY_MODE to 1, registers are accessible 
		 * under base address 
		*/
		RegVal= readl(UsbBase+USB1_LEGACY_CTRL);
		RegVal |= Bit0;
		writel(RegVal, UsbBase+USB1_LEGACY_CTRL);
	}
	else if(UsbBase == NV_ADDRESS_MAP_USB3_BASE)
	{
		/* Enable clock to the USB3 controller */
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0+4);
		RegVal |= Bit27;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0+4);

		/* Reset USB3 controller */
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_RST_DEVICES_L_0+4);
		RegVal |= Bit27;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_RST_DEVICES_L_0+4);
		udelay(2);
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_RST_DEVICES_L_0+4);
		RegVal &= ~Bit27;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE +
				CLK_RST_CONTROLLER_RST_DEVICES_L_0+4);
		udelay(2);
	}

	/*
	 * Assert UTMIP_RESET in USB1/3_IF_USB_SUSP_CTRL register to put
	 * UTMIP1/3 in reset.
	 */
	RegVal= readl(UsbBase+USB_SUSP_CTRL);
	RegVal |= Bit11;
	writel(RegVal, UsbBase+USB_SUSP_CTRL);

	if(UsbBase == NV_ADDRESS_MAP_USB3_BASE)
	{
		/*
		 * Set USB3 to use UTMIP PHY by setting
		 * USB3_IF_USB_SUSP_CTRL.UTMIP_PHY_ENB  register to 1.
		 */
        	RegVal = readl(UsbBase+USB_SUSP_CTRL);
		RegVal |= Bit12;
		writel(RegVal, UsbBase+USB_SUSP_CTRL);
	}

}

void usb1_set_host_mode(void)
{
	int RegVal;

	/* Check whether remote host from USB1 is driving VBus */
	/* If not driving, we set GPIO USB1_VBus_En */
	RegVal= readl(NV_ADDRESS_MAP_USB_BASE+USB_PHY_VBUS_SENSORS_0);

	/* driven by remote host. do nothing here */
	if (RegVal & Bit26)
		return; 

	/* Seaboard platform uses PAD SLXK (GPIO D.00) as USB1_VBus_En */
	/* Config as GPIO */
	tg2_gpio_direction_output(3, 0, 1);

	/* Z_SLXK = 0, normal, not tristate */
	RegVal= readl(NV_ADDRESS_MAP_APB_MISC_BASE + 
			APB_MISC_PP_TRISTATE_REG_B_0);
	RegVal &= ~Bit7;
	writel(RegVal, NV_ADDRESS_MAP_APB_MISC_BASE + 
			APB_MISC_PP_TRISTATE_REG_B_0);
}

void board_usb_init(void)
{
	int RegVal;
	int i;
	NvU32 PlluStableTime =0;
	NvBootClocksOscFreq OscFreq;
	int UsbBase;
	int loop_count;
	int PhyClkValid;

	/* Get the Oscillator frequency */
	OscFreq = NvBootClocksGetOscFreq();

	/* Enable PLL U for USB */
	NvBootClocksStartPll(NvBootClocksPllId_PllU,
                         s_UsbPllBaseInfo[OscFreq].M,
                         s_UsbPllBaseInfo[OscFreq].N,
                         s_UsbPllBaseInfo[OscFreq].P,
                         s_UsbPllBaseInfo[OscFreq].CPCON,
                         s_UsbPllBaseInfo[OscFreq].LFCON,
                         &PlluStableTime);

	/* Initialize USB1 & USB3 controllers */
	for (i = 0; i < 2; i++)
	{
        	/* Select the correct base address for the ith controller. */
        	UsbBase = (i) ? NV_ADDRESS_MAP_USB3_BASE :
				NV_ADDRESS_MAP_USB_BASE;

        	/* Reset the usb controller. */
        	usbf_reset_controller(UsbBase);

        	/* Stop crystal clock by setting UTMIP_PHY_XTAL_CLOCKEN low */
		RegVal= readl(UsbBase+UTMIP_MISC_CFG1);
		RegVal &= ~Bit30;
		writel(RegVal, UsbBase+UTMIP_MISC_CFG1);

        	/* Follow the crystal clock disable by >100ns delay. */
		udelay(1);

        	/*
         	 * To Use the A Session Valid for cable detection logic,
         	 * VBUS_WAKEUP mux must be switched to actually use a_sess_vld
         	 * threshold.  This is done by setting VBUS_SENSE_CTL bit in
         	 * USB_LEGACY_CTRL register.
         	 */
        	if (UsbBase == NV_ADDRESS_MAP_USB_BASE)
        	{
			RegVal= readl(UsbBase+USB1_LEGACY_CTRL);
			RegVal |= (Bit2+Bit1);
			writel(RegVal, UsbBase+USB1_LEGACY_CTRL);
        	}

		/* PLL Delay CONFIGURATION settings
	 	 * The following parameters control the bring up of the plls:
	 	 */
		RegVal= readl(UsbBase+UTMIP_MISC_CFG1);
		RegVal &= 0xFFFC003F;
		RegVal |= (s_UsbPllDelayParams[OscFreq].StableCount << 6);

		RegVal &= 0xFF83FFFF;
		RegVal |= (s_UsbPllDelayParams[OscFreq].ActiveDelayCount << 18);
		writel(RegVal, UsbBase+UTMIP_MISC_CFG1);

		/* Set PLL enable delay count and Crystal frequency count */
		RegVal= readl(UsbBase+UTMIP_PLL_CFG1);
		RegVal &= 0x08FFFFFF;
		RegVal |= (s_UsbPllDelayParams[OscFreq].EnableDelayCount <<27);

		RegVal &= 0xFFFFF000;
		RegVal |= (s_UsbPllDelayParams[OscFreq].XtalFreqCount);
		writel(RegVal, UsbBase+UTMIP_PLL_CFG1);

		/* Setting the tracking length time. */
		RegVal= readl(UsbBase+UTMIP_BIAS_CFG1);
		RegVal &= 0xFFFFFF07;
		RegVal |= (s_UsbBiasTrkLengthTime[OscFreq] <<3);
		writel(RegVal, UsbBase+UTMIP_BIAS_CFG1);

		/* Program Debounce time for VBUS to become valid. */
		RegVal= readl(UsbBase+UTMIP_DEBOUNCE_CFG0);
		RegVal &= 0xFFFF0000;
		RegVal |= s_UsbBiasDebounceATime[OscFreq];
		writel(RegVal, UsbBase+UTMIP_DEBOUNCE_CFG0);

		/* Set UTMIP_FS_PREAMBLE_J to 1 */
		RegVal= readl(UsbBase+UTMIP_TX_CFG0);
		RegVal |= Bit19;
		writel(RegVal, UsbBase+UTMIP_TX_CFG0);

		/* Disable Battery charge enabling bit set to '1' for disable */
		RegVal= readl(UsbBase+UTMIP_BAT_CHRG_CFG0);
		RegVal |= Bit0;
		writel(RegVal, UsbBase+UTMIP_BAT_CHRG_CFG0);

		/* Set UTMIP_XCVR_LSBIAS_SEL to 0 */
		RegVal= readl(UsbBase+UTMIP_XCVR_CFG0);
		RegVal &= ~Bit21;
		writel(RegVal, UsbBase+UTMIP_XCVR_CFG0);

		/* Set bit 3 of UTMIP_SPARE_CFG0 to 1 */
		RegVal= readl(UsbBase+UTMIP_SPARE_CFG0);
		RegVal |= Bit3;
		writel(RegVal, UsbBase+UTMIP_SPARE_CFG0);

        	/* Configure the UTMIP_IDLE_WAIT and UTMIP_ELASTIC_LIMIT
         	 * Setting these fields, together with default values of the
		 * other fields, results in programming the registers below as
		 * follows:
         	 *         UTMIP_HSRX_CFG0 = 0x9168c000
         	 *         UTMIP_HSRX_CFG1 = 0x13
         	 */

		/* Set PLL enable delay count and Crystal frequency count */
		RegVal= readl(UsbBase+UTMIP_HSRX_CFG0);
		RegVal &= 0xFFF07FFF;
		RegVal |= s_UtmipIdleWaitDelay << 15;

		RegVal &= 0xFFFF83FF;
		RegVal |= s_UtmipElasticLimit << 10;
		writel(RegVal, UsbBase+UTMIP_HSRX_CFG0);

		/* Configure the UTMIP_HS_SYNC_START_DLY */
		RegVal= readl(UsbBase+UTMIP_HSRX_CFG1);
		RegVal &= 0xFFFFFFC1;
		RegVal |= s_UtmipHsSyncStartDelay << 1;
		writel(RegVal, UsbBase+UTMIP_HSRX_CFG1);

		/* Preceed  the crystal clock disable by >100ns delay. */
        	udelay(1);

		/* Resuscitate crystal clock by setting UTMIP_PHY_XTAL_CLOCKEN
		 */
		RegVal= readl(UsbBase+UTMIP_MISC_CFG1);
		RegVal |= Bit30;
		writel(RegVal, UsbBase+UTMIP_MISC_CFG1);

		/* Finished the per-controller init. */

		/* De-assert UTMIP_RESET to bring out of reset. */
		RegVal= readl(UsbBase+USB_SUSP_CTRL);
		RegVal &= ~Bit11;
		writel(RegVal, UsbBase+USB_SUSP_CTRL);

		loop_count = 100000;
		while (loop_count)
		{
			/* Wait for the phy clock to become valid in 100 ms */
			PhyClkValid = readl(UsbBase+USB_SUSP_CTRL) & Bit7;
			if (PhyClkValid)
				break;
			udelay(1);
			loop_count--;
		} /* while */
	} /* for */

	/* Disable by writing IC_ENB1 in USB2_CONTROLLER_2_USB2D_ICUSB_CTRL_0.*/
	RegVal= readl(NV_ADDRESS_MAP_USB3_BASE+ICUSB_CTRL);
	RegVal &= ~Bit3;
	writel(RegVal, NV_ADDRESS_MAP_USB3_BASE+ICUSB_CTRL);

	/* Setting STS field to 0 and PTS field to 0 */
	RegVal= readl(NV_ADDRESS_MAP_USB3_BASE+PORTSC1);
	RegVal &= ~(Bit31+Bit30+Bit29);
	writel(RegVal, NV_ADDRESS_MAP_USB3_BASE+PORTSC1);

	/* Deassert power down state for USB3 */
	RegVal= readl(NV_ADDRESS_MAP_USB3_BASE+UTMIP_XCVR_CFG0);
	RegVal &= ~(Bit18+Bit16+Bit14);
	writel(RegVal, NV_ADDRESS_MAP_USB3_BASE+UTMIP_XCVR_CFG0);

	RegVal= readl(NV_ADDRESS_MAP_USB3_BASE+UTMIP_XCVR_CFG1);
	RegVal &= ~(Bit4+Bit2+Bit0);
	writel(RegVal, NV_ADDRESS_MAP_USB3_BASE+UTMIP_XCVR_CFG1);

#if ((LINUX_MACH_TYPE == MACH_TYPE_SEABOARD) || \
     (LINUX_MACH_TYPE == MACH_TYPE_KAEN) || \
     (LINUX_MACH_TYPE == MACH_TYPE_AEBL))
	usb1_set_host_mode();
#endif
}

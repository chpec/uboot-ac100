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
#include "./sdmmc/nvboot_clocks_int.h"
#include "harmony.h"

void board_usb_init(void);

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
	gd->bd->bi_arch_number = MACH_TYPE_TEGRA_HARMONY;

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
void NvBlAvpStallUs(NvU32 MicroSec);
void NvBlAvpStallMs(NvU32 MilliSec);

void NvBlUartClockInitA(void)
{
    NvU32 Reg;

    // Avoid running this function more than once.
    static int initialized = 0;
    if (initialized)
        return;
    initialized = 1;

    // 1. Assert Reset to UART A
    NV_CLK_RST_READ(RST_DEVICES_L, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L,
                             SWR_UARTA_RST, ENABLE, Reg);
    NV_CLK_RST_WRITE(RST_DEVICES_L, Reg);

    // 2. Enable clk to UART A
    NV_CLK_RST_READ(CLK_OUT_ENB_L, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_L,
                             CLK_ENB_UARTA, ENABLE, Reg);
    NV_CLK_RST_WRITE(CLK_OUT_ENB_L, Reg);


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

    // Enable pllp_out0 to UARTA.
    Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_SOURCE_UARTA,
                     UARTA_CLK_SRC, PLLP_OUT0);
    NV_CLK_RST_WRITE(CLK_SOURCE_UARTA, Reg);


    // wait for 2us
    NvBlAvpStallUs(2);

    // De-assert reset to UART A
    NV_CLK_RST_READ(RST_DEVICES_L, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L,
                             SWR_UARTA_RST, DISABLE, Reg);
    NV_CLK_RST_WRITE(RST_DEVICES_L, Reg);

}

void NvBlUartClockInitD(void)
{
    NvU32 Reg;

    // 1. Assert Reset to UART D
    NV_CLK_RST_READ(RST_DEVICES_U, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_U,
                             SWR_UARTD_RST, ENABLE, Reg);
    NV_CLK_RST_WRITE(RST_DEVICES_U, Reg);

    // 2. Enable clk to UART D
    NV_CLK_RST_READ(CLK_OUT_ENB_U, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_U,
                             CLK_ENB_UARTD, ENABLE, Reg);
    NV_CLK_RST_WRITE(CLK_OUT_ENB_U, Reg);

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
    Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_SOURCE_UARTD,
                     UARTD_CLK_SRC, PLLP_OUT0);
    NV_CLK_RST_WRITE(CLK_SOURCE_UARTD, Reg);

    // wait for 2us
    NvBlAvpStallUs(2);

    // De-assert reset to UART D
    NV_CLK_RST_READ(RST_DEVICES_U, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_U,
                             SWR_UARTD_RST, DISABLE, Reg);
    NV_CLK_RST_WRITE(RST_DEVICES_U, Reg);

}

void
NvBlAvpClockSetDivider(NvBool Enable, NvU32 Dividened, NvU32 Divisor)
{
    NvU32 val;

    if (Enable)
    {
        // Set up divider for SCLK.
        // SCLK is used for AVP, AHB, and APB.
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
        // Disable divider for SCLK.
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

static NV_INLINE NvU32
NvBlUartTxReadyA(void)
{
    NvU32 Reg;

    NV_UARTA_READ(LSR, Reg);
    return Reg & UART_LSR_0_THRE_FIELD;
}

static NV_INLINE NvU32
NvBlUartTxReadyD(void)
{
    NvU32 Reg;

    NV_UARTD_READ(LSR, Reg);
    return Reg & UART_LSR_0_THRE_FIELD;
}

static NV_INLINE NvU32
NvBlUartRxReadyA(void)
{
    NvU32 Reg;

    NV_UARTA_READ(LSR, Reg);
    return Reg & UART_LSR_0_RDR_FIELD;
}

static NV_INLINE NvU32
NvBlUartRxReadyD(void)
{
    NvU32 Reg;

    NV_UARTD_READ(LSR, Reg);
    return Reg & UART_LSR_0_RDR_FIELD;
}

static NV_INLINE void
NvBlUartTxA(NvU8 c)
{
    NV_UARTA_WRITE(THR_DLAB_0, c);
}

static NV_INLINE void
NvBlUartTxD(NvU8 c)
{
    NV_UARTD_WRITE(THR_DLAB_0, c);
}

static NV_INLINE NvU32
NvBlUartRxA(void)
{
    NvU32 Reg;

    NV_UARTA_READ(THR_DLAB_0, Reg);
    return Reg;
}

static NV_INLINE NvU32
NvBlUartRxD(void)
{
    NvU32 Reg;

    NV_UARTD_READ(THR_DLAB_0, Reg);
    return Reg;
}

void
NvBlUartInitA(void)
{
    NvU32 Reg;

    // Avoid running this function more than once.
    static int initialized = 0;
    if (initialized)
        return;
    initialized = 1;

    NvBlUartClockInitA();

    /* Enable UARTA - Harmony board uses config4 */
    CONFIG(A,C,IRRX,UARTA); CONFIG(A,C,IRTX,UARTA);

    // Prepare the divisor value.
    Reg = NVRM_PLLP_FIXED_FREQ_KHZ * 1000 / NV_DEFAULT_DEBUG_BAUD / 16;

    // Set up UART parameters.
    NV_UARTA_WRITE(LCR,        0x80);
    NV_UARTA_WRITE(THR_DLAB_0, Reg);
    NV_UARTA_WRITE(IER_DLAB_0, 0x00);
    NV_UARTA_WRITE(LCR,        0x00);
    NV_UARTA_WRITE(IIR_FCR,    0x37);
    NV_UARTA_WRITE(IER_DLAB_0, 0x00);
    NV_UARTA_WRITE(LCR,        0x03);  // 8N1
    NV_UARTA_WRITE(MCR,        0x02);
    NV_UARTA_WRITE(MSR,        0x00);
    NV_UARTA_WRITE(SPR,        0x00);
    NV_UARTA_WRITE(IRDA_CSR,   0x00);
    NV_UARTA_WRITE(ASR,        0x00);

    NV_UARTA_WRITE(IIR_FCR,    0x31);

    // Flush any old characters out of the RX FIFO.
    while (NvBlUartRxReadyA())
        (void)NvBlUartRxA();
}

void
NvBlUartInitD(void)
{
    NvU32 Reg;

    NvBlUartClockInitD();

    /* Enable UARTD - Harmony board uses config2 */
    CONFIG(A,B,GMC,UARTD);

    // Prepare the divisor value.
    Reg = NVRM_PLLP_FIXED_FREQ_KHZ * 1000 / NV_DEFAULT_DEBUG_BAUD / 16;

    // Set up UART parameters.
    NV_UARTD_WRITE(LCR,        0x80);
    NV_UARTD_WRITE(THR_DLAB_0, Reg);
    NV_UARTD_WRITE(IER_DLAB_0, 0x00);
    NV_UARTD_WRITE(LCR,        0x00);
    NV_UARTD_WRITE(IIR_FCR,    0x37);
    NV_UARTD_WRITE(IER_DLAB_0, 0x00);
    NV_UARTD_WRITE(LCR,        0x03);  // 8N1
    NV_UARTD_WRITE(MCR,        0x02);
    NV_UARTD_WRITE(MSR,        0x00);
    NV_UARTD_WRITE(SPR,        0x00);
    NV_UARTD_WRITE(IRDA_CSR,   0x00);
    NV_UARTD_WRITE(ASR,        0x00);

    NV_UARTD_WRITE(IIR_FCR,    0x31);

    // Flush any old characters out of the RX FIFO.
    while (NvBlUartRxReadyD())
        (void)NvBlUartRxD();
}

int
NvBlUartPoll(void)
{
    if (NvBlUartRxReadyA())
        return NvBlUartRxA();

    if (NvBlUartRxReadyD())
        return NvBlUartRxD();

    return -1;
}

int NvBlUartWrite(const void *ptr);
void NvBlPrintf(const char *format, ...);

static void
NvBlPrintU4(NvU8 byte)
{
    NvBlPrintf("%c", s_Hex2Char[byte & 0xF]);
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
    while (!NvBlUartTxReadyA())
        ;
    NvBlUartTxA(c);
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

void UsbfResetController(NvU32 UsbBase)
{
	int RegVal = 0;

	if (UsbBase == NV_ADDRESS_MAP_USB_BASE)
	{
		/* Enable clock to the USB controller */
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0);
		RegVal |= Bit22;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0);

		/* Reset the USB controller */
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_RST_DEVICES_L_0);
		RegVal |= Bit22;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_RST_DEVICES_L_0);
		udelay(2);
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_RST_DEVICES_L_0);
		RegVal &= ~Bit22;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_RST_DEVICES_L_0);
		udelay(2);

		/* Set USB1_NO_LEGACY_MODE to 1 */
		RegVal= readl(UsbBase+USB1_LEGACY_CTRL);
		RegVal |= Bit0;
		writel(RegVal, UsbBase+USB1_LEGACY_CTRL);
	}
	else if(UsbBase == NV_ADDRESS_MAP_USB3_BASE)
	{
		/* Enable clock to the USB3 controller */
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0+4);
		RegVal |= Bit27;
		writel(RegVal, NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0+4);

		/* Reset USB3 controller */
		RegVal= readl(NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_RST_DEVICES_L_0+4);
		if (RegVal & Bit27)
		{
			writel(RegVal, NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_RST_DEVICES_L_0+4);
			udelay(2);
			RegVal= readl(NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_RST_DEVICES_L_0+4);
			RegVal &= ~Bit27;
			writel(RegVal, NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_RST_DEVICES_L_0+4);
			udelay(2);
		}
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
        UsbBase = (i) ? NV_ADDRESS_MAP_USB3_BASE : NV_ADDRESS_MAP_USB_BASE;

        /* Reset the usb controller. */
        UsbfResetController(UsbBase);

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

	/* Disable Batery charge enabling bit set to '1' for disable */
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
         * Setting these fields, together with default values of the other
         * fields, results in programming the registers below as follows:
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

	/* Resuscitate  crystal clock by setting UTMIP_PHY_XTAL_CLOCKEN */
	RegVal= readl(UsbBase+UTMIP_MISC_CFG1);
	RegVal |= Bit30;
	writel(RegVal, UsbBase+UTMIP_MISC_CFG1);
	}

	/* Finished the per-controller init. */

	/* De-assert UTMIP_RESET in USB3_IF_USB_SUSP_CTRL register
	 * to bring out of reset.
	 */
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
	}

	/* Disable by writing IC_ENB1  in USB2_CONTROLLER_2_USB2D_ICUSB_CTRL_0. */
	RegVal= readl(NV_ADDRESS_MAP_USB3_BASE+ICUSB_CTRL);
	RegVal &= ~Bit3;
	writel(RegVal, NV_ADDRESS_MAP_USB3_BASE+ICUSB_CTRL);

	/* Setting STS field to 0 and PTS field to 0 */
	RegVal= readl(NV_ADDRESS_MAP_USB3_BASE+PORTSC1);
	RegVal &= ~(Bit31+Bit30+Bit29);
	writel(RegVal, NV_ADDRESS_MAP_USB3_BASE+PORTSC1);

	/* Deassert power down state */
	RegVal= readl(UsbBase+UTMIP_XCVR_CFG0);
	RegVal &= ~(Bit18+Bit16+Bit14);
	writel(RegVal, UsbBase+UTMIP_XCVR_CFG0);

	RegVal= readl(UsbBase+UTMIP_XCVR_CFG1);
	RegVal &= ~(Bit4+Bit2+Bit0);
	writel(RegVal, UsbBase+UTMIP_XCVR_CFG1);
#if 0
	/* Hold the reset for UTMIP3. */
	RegVal= readl(UsbBase+USB_SUSP_CTRL);
	RegVal |= Bit11;
	writel(RegVal, UsbBase+USB_SUSP_CTRL);

	loop_count = 100000;
	while (loop_count)
	{
        //Wait for the phy clock to become 0 in 100 ms
        PhyClkValid = readl(UsbBase+USB_SUSP_CTRL) & Bit7;
	if (!PhyClkValid)
		break;
        udelay(1);
        loop_count--;
	}
#endif
}

/*
 * Copyright (c) 2008 Texas Instruments
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * (C) Copyright 2009 Stefan Roese <sr@denx.de>, DENX Software Engineering
 *
 * Original Author Guenter Gebhardt
 * Copyright (C) 2006 Micronas GmbH
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
#ifdef CONFIG_CMD_USB
#include <devices.h>
#include <usb.h>

#ifdef USE_PROC_COMM
#include <asm/arch/proc_comm_clients.h>
#endif

uint8_t usbhs_ulpi_phy_read_reg(uint32_t address)
{
    uint32_t cmd;
    uint32_t tmp;
    uint32_t port = 0;

    cmd = ((address << USB_OTG_HS_ULPI_VIEWPORT__ULPIADDR___S) |    // reg addr
           (port  << USB_OTG_HS_ULPI_VIEWPORT__ULPIPORT___S) |      // port
            USB_OTG_HS_ULPI_VIEWPORT__ULPIRUN___M );                // run bit

    IO_WRITE32(USBH1_USB_OTG_HS_ULPI_VIEWPORT, cmd);
    do
    {
       tmp = IO_READ32(USBH1_USB_OTG_HS_ULPI_VIEWPORT);
    }  while ((tmp & USB_OTG_HS_ULPI_VIEWPORT__ULPIRUN___M) != 0);

    tmp &= USB_OTG_HS_ULPI_VIEWPORT__ULPIDATRD___M;
    tmp = tmp >> USB_OTG_HS_ULPI_VIEWPORT__ULPIDATRD___S;

    return(tmp);
}

void usbhs_ulpi_phy_write_reg(uint32_t address, uint32_t write_data)
{
    uint32_t cmd;
    uint32_t tmp;
    uint32_t port = 0;

    cmd = ((address << USB_OTG_HS_ULPI_VIEWPORT__ULPIADDR___S) |    // reg addr
           (port  << USB_OTG_HS_ULPI_VIEWPORT__ULPIPORT___S) |      // port
            USB_OTG_HS_ULPI_VIEWPORT__ULPIRW___M |                  // write
            (write_data & 0xFF) |                                   // write data
            USB_OTG_HS_ULPI_VIEWPORT__ULPIRUN___M );                // run bit

    IO_WRITE32(USBH1_USB_OTG_HS_ULPI_VIEWPORT, cmd);
    do
    {
       tmp = IO_READ32(USBH1_USB_OTG_HS_ULPI_VIEWPORT);
    }  while ((tmp & USB_OTG_HS_ULPI_VIEWPORT__ULPIRUN___M) != 0);

}

void usbhs_ulpi_phy_init(void)
{

    // Set core to ULPI mode, clear connect status change (CSC)
    IO_WRITE32(USBH1_USB_OTG_HS_PORTSC, 0x80000002);

#if defined(USE_PROC_COMM) && defined(USE_PROC_COMM_USB_PHY_RESET)
    debug("Using PROC_COMM for PHY RESET\n");
    //assumption: resets both usb host and phy (in APPS_RESET)
    proc_comm_usb_reset_phy();
    udelay(1000); //delay still needed to release USBHS core reset.

#else
    debug("Not using PROC_COMM for PHY RESET\n");
    // Reset USBHS core
    IO_WRITE32(APPS_RESET, (IO_READ32(APPS_RESET) | APPS_RESET__USBH___M));

    // Reset USB PHY
    IO_WRITE32(APPS_RESET, (IO_READ32(APPS_RESET) | APPS_RESET__USB_PHY___M));
    udelay(1000);
    // Release PHY reset
    IO_WRITE32(APPS_RESET, (IO_READ32(APPS_RESET) & ~APPS_RESET__USB_PHY___M));
    // Release USBHS core reset
    IO_WRITE32(APPS_RESET, (IO_READ32(APPS_RESET) & ~APPS_RESET__USBH___M));

#endif /* defined(USE_PROC_COMM) && defined(USE_PROC_COMM_USB_PHY_RESET) */

    // Set core to ULPI mode, clear connect status change (CSC)
    IO_WRITE32(USBH1_USB_OTG_HS_PORTSC, 0x80000002);

    udelay(1000);
}

int ehci_hcd_init_qc(uint32_t *hccr_ptr, uint32_t *hcor_ptr)
{
#ifndef USE_PROC_COMM
    uint32_t reg;
#endif /*USE_PROC_COMM*/

    // Initialize capability register structure pointer
    *hccr_ptr = USBH1_USB_OTG_HS_CAPLENGTH;
    // Initialize operational register structure pointer
    *hcor_ptr = *hccr_ptr + IO_READ8(USBH1_USB_OTG_HS_CAPLENGTH);
#ifndef USE_PROC_COMM
    // Enable USBH1 clock  (ZZZZ smem)
    reg = IO_READ32(USBH_NS_REG);
    reg &= 0xFFFFF0FF;
    reg |= 0x00000B00;    // enable USBH1 clock
    IO_WRITE32(USBH_NS_REG, reg);
#else /*USE_PROC_COMM defined */
    debug("BEFORE:: USBH_NS_REG=0x%08x\n", IO_READ32(USBH_NS_REG));
    debug("BEFORE:: GLBL_CLK_ENA=0x%08x\n", IO_READ32(GLBL_CLK_ENA));
    proc_comm_enable_hsusb_clk();
    debug("AFTER_ENABLE:: USBH_NS_REG=0x%08x\n", IO_READ32(USBH_NS_REG));
    debug("AFTER_ENABLE:: GLBL_CLK_ENA=0x%08x\n", IO_READ32(GLBL_CLK_ENA));
#endif /*USE_PROC_COMM*/

    // Initialize the PHY
    // ZZZZ replace this with an smem_proc_comm call???
    usbhs_ulpi_phy_init();

    // Enable IDpullup to start ID pin sampling
    usbhs_ulpi_phy_write_reg(0xB, 0x01);

    // Vbus power select = 1, stream disable mode,
    // host controller mode, setup lockouts off
    IO_WRITE32(USBH1_USB_OTG_HS_USBMODE, 0x3B);

    // Use the AHB transactor
    IO_WRITE32(USBH1_USB_OTG_HS_AHB_MODE, 0x0);

    // Check to see if ULPI is in normal sync state, if not perform a wake-up
    if ((IO_READ32(USBH1_USB_OTG_HS_ULPI_VIEWPORT) & USB_OTG_HS_ULPI_VIEWPORT__ULPISS___M) == 0)
    {
        // Wake up the ULPI (assume port 0)
        IO_WRITE32(USBH1_USB_OTG_HS_ULPI_VIEWPORT, USB_OTG_HS_ULPI_VIEWPORT__ULPIWU___M);
        while (IO_READ32(USBH1_USB_OTG_HS_ULPI_VIEWPORT) & USB_OTG_HS_ULPI_VIEWPORT__ULPIWU___M);

        // Check again if in normal sync state,
        if ((IO_READ32(USBH1_USB_OTG_HS_ULPI_VIEWPORT) & USB_OTG_HS_ULPI_VIEWPORT__ULPISS___M) == 0)
        {
            printf("ERROR: ULPI not in normal sync state\n");
            return(-1);
        }
    }

    // Without PROC_COMM assume it has been turned on before U-boot.
#ifdef USE_PROC_COMM
    proc_comm_usb_vbus_power(1);
#endif /*USE_PROC_COMM*/

    // Set Power Power bit in the port status/control reg
    IO_WRITE32(USBH1_USB_OTG_HS_PORTSC, (IO_READ32(USBH1_USB_OTG_HS_PORTSC) | USB_OTG_HS_PORTSC__PP___M));

    return 0;
}

int ehci_hcd_stop_qc(uint32_t hccr, uint32_t hcor)
{
#ifndef USE_PROC_COMM
    uint32_t reg;
#endif /*USE_PROC_COMM*/


#ifdef USE_PROC_COMM
    proc_comm_usb_vbus_power(0);
    debug("BEFORE:: USBH_NS_REG=0x%08x\n", IO_READ32(USBH_NS_REG));
    debug("BEFORE:: GLBL_CLK_ENA=0x%08x\n", IO_READ32(GLBL_CLK_ENA));
    proc_comm_disable_hsusb_clk();
    debug("AFTER_DISABLE:: USBH_NS_REG=0x%08x\n", IO_READ32(USBH_NS_REG));
    debug("AFTER_DISABLE:: GLBL_CLK_ENA=0x%08x\n", IO_READ32(GLBL_CLK_ENA));
#else /*USE_PROC_COMM not defined */
    //cant turn off vbus without proc_comm
    // Disable USBH1 clock
    reg = IO_READ32(USBH_NS_REG);
    reg &= 0xFFFFF0FF;    // disable USBH1-clock
    IO_WRITE32(USBH_NS_REG, reg);
#endif /*USE_PROC_COMM*/

    return 0;
}


#ifdef CONFIG_SYS_USB_EVENT_POLL

// Function derived from drivers\usb\musb_hcd.c (Texas Instruments)

extern unsigned char new[];

/*
 * This function polls for USB keyboard data.
 */
void usb_event_poll()
{
	device_t *dev;
	struct usb_device *usb_kbd_dev;
	struct usb_interface_descriptor *iface;
	struct usb_endpoint_descriptor *ep;
	int pipe;
	int maxp;
    static uint last_poll = 0;

    /* Proceed only if the last kbd poll was at least 40msec ago.
     * If the keyboard is polled to often, the boot countdown
     * runs very slow. This doesn't affect other kbd usage.
     */
    if (get_timer(last_poll) < (CONFIG_SYS_HZ/25))
    {
        return;
    }

	/* Get the pointer to USB Keyboard device pointer */
	dev = device_get_by_name("usbkbd");
	usb_kbd_dev = (struct usb_device *)dev->priv;
	iface = &usb_kbd_dev->config.if_desc[0];
	ep = &iface->ep_desc[0];
	pipe = usb_rcvintpipe(usb_kbd_dev, ep->bEndpointAddress);

	/* Submit an interrupt transfer request */
	maxp = usb_maxpacket(usb_kbd_dev, pipe);
	usb_submit_int_msg(usb_kbd_dev, pipe, &new[0],
			maxp > 8 ? 8 : maxp, ep->bInterval);

    usb_kbd_dev->irq_status = 0x0;
    usb_kbd_dev->irq_act_len = 0x8;
    /* call the interrupt handler to process the data */
    usb_kbd_dev->irq_handle(usb_kbd_dev);

    /* Remember time of last poll */
    last_poll = get_timer(0);
}
#endif /* CONFIG_SYS_USB_EVENT_POLL */
#endif /* CONFIG_CMD_USB */

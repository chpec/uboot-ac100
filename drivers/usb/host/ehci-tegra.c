/*
 * Copyright (c) 2009 NVIDIA Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY FOR THE
 * PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  THIS SOFTWARE IS
 * PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * See the GNU General Public License for more details.
 * A copy of the GNU General Public License is attached hereto and may also
 * be received by contacting EmbeddedInfo@nvidia.com.  If a copy is not
 * attached, then, then you may retrieve a copy of the license at:
 * http://www.fsf.org/licensing/licenses/gpl.txt
 * A copy of the source code governed under the GPL may be received by
 * contacting EmbeddedInfo@nvidia.com
 */

#include <common.h>
#include <usb.h>

#include "ehci.h"
#include "ehci-core.h"

#include <asm/errno.h>

/* USB_CONTROLLER_INSTANCES, NvUSBx_x defined in header file like tegra2_harmony.h, tegra2_seaboard.h */
int USB_base_addr[5] = {
	NvUSBx_0,
	NvUSBx_1,
	NvUSBx_2,
	NvUSBx_3,
	0
};
int USB_EHCI_TEGRA_BASE_ADDR=NvUSBx_0;

#if LINUX_MACH_TYPE == MACH_TYPE_TEGRA_SEABOARD
void usb1_set_host_mode(void);
#endif

/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int ehci_hcd_init(void)
{
    /* EHCI registers start at offset 0x100 */
    hccr = (struct ehci_hccr *)(USB_EHCI_TEGRA_BASE_ADDR + 0x100);
    hcor = (struct ehci_hcor *)((uint32_t) hccr
            + HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

    printf("Tegra ehci init hccr %x and hcor %x hc_length %d\n",
        (uint32_t)hccr, (uint32_t)hcor,
        (uint32_t)HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

    return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(void)
{
#if LINUX_MACH_TYPE == MACH_TYPE_TEGRA_SEABOARD
        usb1_set_host_mode();
#endif
	ehci_writel(&hcor->or_usbcmd, 0);
	udelay(1000);
	ehci_writel(&hcor->or_usbcmd, 2);
	udelay(1000);
	return 0;
}

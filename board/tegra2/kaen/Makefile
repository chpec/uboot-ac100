#
#  (C) Copyright 2010
#  NVIDIA Corporation <www.nvidia.com>
#
#
#  See file CREDITS for list of people who contributed to this
#  project.
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#  MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

$(shell mkdir -p $(obj)../common/sdmmc)
$(shell mkdir -p $(obj)../common/crypto)
ifdef CONFIG_TEGRA2_LCD
$(shell mkdir -p $(obj)../common/lcd)
$(shell mkdir -p $(obj)../common/lcd/gpinit)
endif

LIB	= $(obj)lib$(BOARD).a

COBJS	:= $(BOARD).o
ifdef CONFIG_TEGRA2_KEYBOARD
COBJS	+= $(BOARD)_kbc.o
endif
COBJS	+= ../common/board.o
COBJS	+= ../common/sdmmc/nvboot_sdmmc.o
COBJS	+= ../common/sdmmc/nvboot_reset.o
COBJS	+= ../common/sdmmc/nvboot_util.o
COBJS	+= ../common/sdmmc/nvboot_clocks.o
COBJS	+= ../common/sdmmc/nvboot_pads.o
COBJS	+= ../common/sdmmc/nvboot_pinmux.o
COBJS	+= ../common/sdmmc/nvboot_pinmux_tables.o
ifdef CONFIG_TEGRA2_LP0
COBJS   += ../common/crypto/aes_ref.o
COBJS   += ../common/crypto/crypto.o
endif
ifdef CONFIG_TEGRA2_LCD
COBJS	+= gpinit.o
COBJS	+= ../common/lcd/gpinit/gp-util.o
COBJS	+= ../common/lcd/gpinit/gp-dc.o
COBJS	+= ../common/lcd/gpinit/gp-clk.o
COBJS	+= ../common/lcd/gpinit/gp-pinmux.o
COBJS	+= ../common/lcd/gpinit/gp-pinmux-t2-tables.o
COBJS	+= ../common/lcd/nvboot_lcd.o
endif

SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	rm -f $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak $(obj).depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

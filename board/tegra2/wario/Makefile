#
# Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
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

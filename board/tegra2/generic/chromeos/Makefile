#
# Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Alternatively, this software may be distributed under the terms of the
# GNU General Public License ("GPL") version 2 as published by the Free
# Software Foundation.
#

include $(TOPDIR)/config.mk

LIB	= $(obj)libchromeos_hardware_interface.a

COBJS-$(CONFIG_CHROMEOS) += cpu_state.o
COBJS-$(CONFIG_CHROMEOS) += eeprom_driver.o
COBJS-$(CONFIG_CHROMEOS) += gpio.o
COBJS-$(CONFIG_CHROMEOS) += nvram.o
COBJS-$(CONFIG_CHROMEOS) += tpm_driver.o

COBJS	:= $(COBJS-y)
OBJS	:= $(addprefix $(obj),$(COBJS))

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

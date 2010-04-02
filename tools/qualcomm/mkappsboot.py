#!/usr/bin/python

#Copyright (c) 2010, Code Aurora Forum. All rights reserved.

#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License version 2 and
#only version 2 as published by the Free Software Foundation.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
#02110-1301, USA.


#usage: ./mkappsboot.py u-boot.bin appsboot.mbn

import sys
import struct

# read the bin file
bin = open(sys.argv[1], "rb")
loader = bin.read()
bin.close()

# construct the mbn header
size = len(loader)
base = 0xE0000000
header = struct.pack("10I",
    0x00000005, # appsbl
    0x00000003, # part version
    0x00000000,
    base,
    size,
    size,
    base + size,
    0x00000000,
    base + size,
    0x00000000,
)

# write the mbn file with the header and code
mbn = open(sys.argv[2], "wb+")
mbn.seek(0)
mbn.truncate()
mbn.write(header)
mbn.write(loader)
mbn.close()

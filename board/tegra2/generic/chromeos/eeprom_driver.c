/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

/* Implementation of per-board EEPROM driver functions */

#include <common.h>
#include <malloc.h>
#include <chromeos/hardware_interface.h>

/* TODO Replace dummy functions below with real implementation. */

int lock_down_eeprom(void) { return 0; }

/* TODO Replace mem_seek/read with spi_seek/read when moving firmware storage
 * from NAND to SPI Flash */

struct context_t {
	void *begin, *cur, *end;
};

static off_t mem_seek(void *context, off_t offset, enum whence_t whence)
{
	void *begin, *cur, *end;

	begin = ((struct context_t *) context)->begin;
	cur = ((struct context_t *) context)->cur;
	end = ((struct context_t *) context)->end;

	if (whence == SEEK_SET)
		cur = begin + offset;
	else if (whence == SEEK_CUR)
		cur = cur + offset;
	else if (whence == SEEK_END)
		cur = end + offset;
	else {
		debug("mem_seek: unknown whence value: %d\n", whence);
		return -1;
	}

	if (cur < begin) {
		debug("mem_seek: offset underflow: %p < %p\n", cur, begin);
		return -1;
	}

	if (cur >= end) {
		debug("mem_seek: offset exceeds size: %p >= %p\n", cur, end);
		return -1;
	}

	((struct context_t *) context)->cur = cur;
	return cur - begin;
}

static ssize_t mem_read(void *context, void *buf, size_t count)
{
	void *begin, *cur, *end;

	if (count == 0)
		return 0;

	begin = ((struct context_t *) context)->begin;
	cur = ((struct context_t *) context)->cur;
	end = ((struct context_t *) context)->end;

	if (count > end - cur)
		count = end - cur;

	memcpy(buf, cur, count);
	((struct context_t *) context)->cur += count;

	return count;
}

int init_firmware_storage(firmware_storage_t *f)
{
	struct context_t *context;

	context = (struct context_t *) malloc(sizeof(struct context_t));
	context->begin = (void *) TEXT_BASE;
	context->cur = (void *) TEXT_BASE;
	context->end = (void *) TEXT_BASE + CONFIG_FIRMWARE_SIZE;

	f->seek = mem_seek;
	f->read = mem_read;
	f->context = (void *) context;

	return 0;
}

int release_firmware_storage(firmware_storage_t *f)
{
	free(f->context);
	return 0;
}

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

/*
 * Defines useful constants for working with I2C.
 */

#ifndef _TEGRA2_I2C_H
#define _TEGRA2_I2C_H

/* Convert the number of bytes to word. */
#define BYTES_TO_WORD(ReqSize) (((ReqSize) + 3) >> 2)

#define I2CSPEED 100

/* Wait time to poll the status for completion. */
enum { I2C_TIMEOUT_USEC = 1000};

/* I2C fifo depth. */
enum { I2C_FIFO_DEPTH = 8};


/// Flag to indicate the I2C write/read operation.
#define I2C_IS_WRITE		0x00000001
/// Flag to indicate the I2C slave address type as 10-bit or not.
#define I2C_IS_10_BIT_ADDRESS	0x00000002
/// Flag to indicate the I2C transaction with repeat start.
#define I2C_USE_REPEATED_START	0x00000004
/// Flag to indicate that the I2C slave will not generate ACK.
#define I2C_NO_ACK		0x00000008
/// Flag to indicate software I2C transfer using GPIO.
#define I2C_SOFTWARE_CONTROLLER	0x00000010
#define I2C_NO_STOP		0x00000020

/// Contians the I2C transaction details.
typedef struct
{
    /// Flags to indicate the transaction details, like write/read operation,
    /// slave address type 10-bit or 7-bit and the transaction uses repeat
    /// start or a normal transaction.
    NvU32 flags;
    /// I2C slave device address. The address can be initialized as:
    NvU32 address;
    /// Number of bytes to be transferred.
    NvU32 num_bytes;
    /// Send/receive buffer. For I2C send operation this buffer should be
    /// filled with the data to be sent to the slave device. For I2C receive
    /// operation this buffer is filled with the data received from the slave device.
    NvU8 *buf;
    NvBool is_10bit_address;
} I2c_Transaction_Info;

#endif

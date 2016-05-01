/*
 *  Copyright (C) 2016 Ed Schaller <schallee@darkmist.net>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef usb_h
#define usb_h

#include <libusb-1.0/libusb.h>
#include <stdint.h>
#include <sys/types.h>
	/* for ssize_t */

typedef union
{
	uint16_t u16;
	struct
	{
		uint8_t bus;
		uint8_t addr;
	} ba;
} usb_bus_addr_t;

extern const usb_bus_addr_t usb_null_bus_addr;

usb_bus_addr_t mk_bus_addr(uint8_t bus, uint8_t addr);
usb_bus_addr_t parse_bus_addr(const char *str);
usb_bus_addr_t* parse_bus_addr_strss(const char **strs, usb_bus_addr_t *result);
void devs_add_ref(libusb_device **devs);
void devs_del_ref(libusb_device **devs);
libusb_device **find_devs_by_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, ssize_t *num_devs_ret);
libusb_device_handle **find_dev_handles_by_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, ssize_t *num_devs_ret);
libusb_device **find_devs_by_bus_addr_strs_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, const char** bus_addr_strs, size_t num_bus_addr_strs);
libusb_device **find_devs_by_bus_addr_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, const usb_bus_addr_t *bus_addrs, size_t num_bus_addrs);
libusb_device_handle **devs_open_handles(libusb_device **devs, size_t num_devs);
libusb_device_handle **find_dev_handles_by_bus_addr_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, const usb_bus_addr_t *bus_addrs, size_t num_bus_addrs);
libusb_device_handle **find_dev_handles_by_bus_addr_strs_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, const char **bus_addr_strs, size_t num_bus_addr_strs);
void free_devs(libusb_device **devs);
void free_dev_handles(libusb_device_handle **dev_handles);
void dev_handles_close(libusb_device_handle **dev_handles);

typedef void (*dev_handle_apply_t)(libusb_device_handle *dev_handle);
void dev_handles_apply(libusb_device_handle **devs, dev_handle_apply_t func);

typedef void (*dev_apply_t)(libusb_device *dev);
void devs_apply(libusb_device **devs, dev_apply_t func);

#endif

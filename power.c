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

#include "power.h"
#include <stdint.h>
#include <string.h>
#include "die.h"
#include "debug.h"
#include "usb.h"

#define USB_TIMEOUT		500

#define QUERY_BMREQUEST_TYPE	0xc0
#define QUERY_BREQUEST		0x01
#define QUERY_WVALUE		0x0081
#define QUERY_WINDEX		0x0000
#define QUERY_WLENGTH		0x0001
#define QUERY_TIMEOUT		USB_TIMEOUT
#define QUERY_IS_ON		0xa0

#define SET_BMREQUEST_TYPE	0x40
#define SET_BREQUEST		0x01
#define SET_WVALUE		0x0001
#define SET_WINDEX_ON		0xa0
#define SET_WINDEX_OFF		0x20
#define SET_WLENGTH		0x01
#define SET_TIMEOUT		USB_TIMEOUT

bool get_state(libusb_device_handle *dev_handle)
{
	uint8_t buf[QUERY_WLENGTH];
	int transfered;
	libusb_device *dev;

	/* zero buf so we're not sending random stack data to the
	 * device */
	memset(buf, 0, sizeof(buf));
	if((transfered = libusb_control_transfer(dev_handle, QUERY_BMREQUEST_TYPE, QUERY_BREQUEST, QUERY_WVALUE, QUERY_WINDEX, buf, QUERY_WLENGTH, QUERY_TIMEOUT))!=QUERY_WLENGTH)
	{
		dev = libusb_get_device(dev_handle);
		die_usb(transfered, "Unable to query device %02x:%02x state", libusb_get_bus_number(dev), libusb_get_device_address(dev));
	}
	return buf[0] == QUERY_IS_ON;
}

bool is_on(libusb_device_handle *dev_handle)
{
	return get_state(dev_handle);
}

bool is_off(libusb_device_handle *dev_handle)
{
	return !get_state(dev_handle);
}

void set_state(libusb_device_handle *dev_handle, bool state)
{
	uint8_t buf[SET_WLENGTH];
	int usb_err;
	libusb_device *dev;

	debug("\tTurning %s", state ? "on" : "off");
	/* zero buf so we're not sending random stack data to the
	 * device */
	memset(buf, 0, sizeof(buf));
	if((usb_err = libusb_control_transfer(dev_handle, SET_BMREQUEST_TYPE, SET_BREQUEST, SET_WVALUE, state ? SET_WINDEX_ON : SET_WINDEX_OFF, buf, SET_WLENGTH, SET_TIMEOUT))!=SET_WLENGTH)
	{
		dev = libusb_get_device(dev_handle);
		die_usb(usb_err, "Unable to set device %02x:%02x %s", libusb_get_bus_number(dev), libusb_get_device_address(dev), state ? "on" : "off");
	}
	debug("\ttransfer amount was %d buf[0]=0x%02x", usb_err, buf[0]);
}

void turn_on(libusb_device_handle *dev_handle)
{
	set_state(dev_handle, true);
}

void turn_off(libusb_device_handle *dev_handle)
{
	set_state(dev_handle, false);
}


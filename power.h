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

#ifndef power_h
#define power_h

#include <libusb-1.0/libusb.h>
#include <stdbool.h>

/**
 * Get current switch state of device.
 * @param dev_handle Handle to device
 * @returns true if on, false otherwise.
 */
bool get_state(libusb_device_handle *dev_handle);

/**
 * Is the power switch on?
 * @param dev_handle Handle to device
 * @returns true if on, false otherwise.
 */
bool is_on(libusb_device_handle *dev_handle);

/**
 * Is the power switch off?
 * @param dev_handle Handle to device
 * @returns true if on, false otherwise.
 */
bool is_off(libusb_device_handle *dev_handle);

/**
 * Set the state of the power switch.
 * @param dev_handle Handle to device
 * @param state The switch state to set. true=on
 */
void set_state(libusb_device_handle *dev_handle, bool state);

/**
 * Turn the switch off
 * @param dev_handle Handle to device
 */
void turn_off(libusb_device_handle *dev_handle);

/**
 * Turn the switch on
 * @param dev_handle Handle to device
 */
void turn_on(libusb_device_handle *dev_handle);

#endif

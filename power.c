#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "die.h"
#include "debug.h"
#include "usb.h"

#define VENDOR	0x067b
#define PRODUCT	0x2303

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

/**
 * Get current switch state of device.
 * @param dev_handle Handle to device
 * @returns true if on, false otherwise.
 */
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

/**
 * Is the power switch on?
 * @param dev_handle Handle to device
 * @returns true if on, false otherwise.
 */
bool is_on(libusb_device_handle *dev_handle)
{
	return get_state(dev_handle);
}

/**
 * Is the power switch off?
 * @param dev_handle Handle to device
 * @returns true if on, false otherwise.
 */
bool is_off(libusb_device_handle *dev_handle)
{
	return !get_state(dev_handle);
}

/**
 * Set the state of the power switch.
 * @param dev_handle Handle to device
 * @param state The switch state to set. true=on
 */
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

/**
 * Turn the switch on
 * @param dev_handle Handle to device
 */
void turn_on(libusb_device_handle *dev_handle)
{
	set_state(dev_handle, true);
}

/**
 * Turn the switch off
 * @param dev_handle Handle to device
 */
void turn_off(libusb_device_handle *dev_handle)
{
	set_state(dev_handle, false);
}

int main(int argc, const char **argv)
{
	libusb_context *ctx = NULL;
	int usb_err;
	ssize_t num_devs;
	libusb_device **devs = NULL;
	libusb_device **dev_ptr = NULL;
	libusb_device_handle *dev_handle = NULL;
	bool kernel_was_active = false;
	uint8_t bus;
	uint8_t addr;

#	ifdef DEBUG
		enable_debug();
#	endif

	debug("argc=%d", argc);
	if((usb_err = libusb_init(&ctx)))
		die_usb(usb_err, "Unable to initialize context");
	debug("usb_err=%d, ctx=0x%0" PRIxPTR, usb_err, (intptr_t)ctx);
	libusb_set_debug(ctx, 3);

	if(!(devs = find_devs_by_vend_prod(ctx, VENDOR, PRODUCT, &num_devs)))
		die("No devices found.");

	debug("Found %ld devices", (long int)num_devs);

	for(dev_ptr=devs;*dev_ptr;dev_ptr++)
	{
		bus = libusb_get_bus_number(*dev_ptr);
		addr = libusb_get_device_address(*dev_ptr);
		debug("bus=%02x addr=%02x", bus, addr);
		if((usb_err=libusb_open(*dev_ptr, &dev_handle)))
			die_usb(usb_err, "Unable to open device %02x:%02x", bus, addr);
		switch((usb_err=libusb_kernel_driver_active(dev_handle, 0)))
		{
			case 0:
				debug("\tNo kernel driver active.");
				break;
			case 1:
				debug("\tKernel driver active.");
				kernel_was_active = true;
				if((usb_err = libusb_detach_kernel_driver(dev_handle, 0)))
					die_usb(usb_err, "Unable to detach kernel driver for %02x:%02x", bus, addr);
				break;
			default:
				die_usb(usb_err, "Unable to determine if kernel driver is active for %02x:%02x.", bus, addr);
		}

		if(is_on(dev_handle))
		{
			debug("\ton");
			if(argc == 2 && !strcmp(argv[1], "off"))
				turn_off(dev_handle);
		}
		else
		{
			debug("\toff");
			if(argc == 2 && !strcmp(argv[1], "on"))
				turn_on(dev_handle);
		}

		if(kernel_was_active && (usb_err=libusb_attach_kernel_driver(dev_handle, 0)))
				die_usb(usb_err, "Unable to reattach kernel driver for %02x:%02x", bus, addr);
		libusb_close(dev_handle);
	}

	free_devs(devs);

	libusb_exit(ctx);
	return EXIT_SUCCESS;
}

#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "power.h"
#include "usb.h"
#include "debug.h"
#include "die.h"

#define VENDOR	0x067b
#define PRODUCT	0x2303

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

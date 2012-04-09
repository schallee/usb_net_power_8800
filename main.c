#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <inttypes.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "power.h"
#include "usb.h"
#include "debug.h"
#include "die.h"

#define VENDOR	0x067b
#define PRODUCT	0x2303

typedef enum 
{
	on, off, status
} run_mode_t;

static void print_state(libusb_device_handle *dev_handle)
{
	libusb_device *dev;
	bool state;

	dev = libusb_get_device(dev_handle);
	state = get_state(dev_handle);

	printf("%03o:%03o %s\n",
		libusb_get_bus_number(dev),
		libusb_get_device_address(dev),
		state ? "on" : "off");
}

int main(int argc, const char **argv)
{
	run_mode_t mode;
	int usb_err;
	libusb_context *ctx;
	libusb_device_handle **dev_handles;
	const char **bus_addr_strs;
	ssize_t num;

	if(argc == 1)
	{	// show all 
		mode = status;
		num = 0;
		bus_addr_strs = &(argv[1]);
	}
	else
	{	// at least 1 arg
		bus_addr_strs = &(argv[2]);
		num = argc - 2;
		if(!strcasecmp(argv[1], "on"))
			mode = on;
		else if(!strcasecmp(argv[1], "off"))
			mode = off;
		else if(!strcasecmp(argv[1], "status"))
			mode = status;
		else
		{
			mode = status;
			bus_addr_strs = &(argv[1]);
			num = 1;
		}
	}

	if((usb_err = libusb_init(&ctx)))
		die_usb(usb_err, "Unable to intialize libusb context");
	libusb_set_debug(ctx, 3);

	if(num)
		dev_handles = find_dev_handles_by_bus_addr_strs_vend_prod(ctx, VENDOR, PRODUCT, bus_addr_strs, num);
	else
		dev_handles = find_dev_handles_by_vend_prod(ctx, VENDOR, PRODUCT, &num);

	switch(mode)
	{
		case on:
			dev_handles_apply(dev_handles, turn_on);
			break;
		case off:
			dev_handles_apply(dev_handles, turn_off);
			break;
		case status:
			dev_handles_apply(dev_handles, print_state);
			break;
		default:
			die("Unknown value for mode %d", (int)mode);
	}

	dev_handles_close(dev_handles);
	libusb_exit(ctx);
}

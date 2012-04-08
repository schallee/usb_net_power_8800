#include "usb.h"
#include "die.h"
#include <stdbool.h>
#include <stdlib.h>

libusb_device *find_dev_by_bus_addr(libusb_context *ctx, uint8_t bus, uint8_t addr)
{
	libusb_device ** devices;
	libusb_device ** dev_ptr;
	ssize_t num_devices = 0;

	if((num_devices = libusb_get_device_list(ctx, &devices))<0)
		die_usb(num_devices, "Unable to enumerate devices");
	/* libusb_get_device_list's last entry is null according to docs */
	for(dev_ptr = devices; *dev_ptr; dev_ptr++)
	{
		if(libusb_get_bus_number(*dev_ptr) == bus && libusb_get_device_address(*dev_ptr) == addr)
		{
			libusb_ref_device(*dev_ptr);
			break;
		}
	}
	/* we've added an extra ref so our's won't be freed */
	libusb_free_device_list(devices, true);
	/* If last entry in list is NULL, not finding it will return
	 * NULL here. */
	return *dev_ptr;
}

libusb_device **find_devs_by_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, ssize_t *num_devs_found)
{
	libusb_device ** devices = NULL;
	libusb_device ** found = NULL;
	struct libusb_device_descriptor desc;
	ssize_t num_devices = 0;
	ssize_t num_found=0;
	int usb_err;
	ssize_t i;

	if((num_devices = libusb_get_device_list(ctx, &devices))<0)
		die_usb(num_devices, "Unable to enumerate devices");
	if(!(found = (libusb_device **)malloc(sizeof(libusb_device*)*(num_devices+1))))
		die_std("Failed to malloc device array");
	for(i=0;i<num_devices;i++)
	{
		if((usb_err = libusb_get_device_descriptor(devices[i], &desc)))
			die_usb(usb_err, "Unable to get device descriptor");
		if(desc.idVendor == vendor && desc.idProduct == product)
			found[num_found++] = devices[i];
		else
			libusb_unref_device(devices[i]);
	}
	libusb_free_device_list(devices, false);
	found[num_found] = NULL;
	if(num_devs_found)
		*num_devs_found = num_found;
	if(num_found)
		return found;
	free(found);
	return NULL;
}

void free_devs(libusb_device **devs)
{
	libusb_device **ptr;

	if(!devs)
		return;
	for(ptr=devs;*ptr;ptr++)
		libusb_unref_device(*ptr);
	free(devs);
}

#include "usb.h"
#include "die.h"
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

usb_bus_addr_t const usb_null_bus_addr = {0};

usb_bus_addr_t mk_bus_addr(uint8_t bus, uint8_t addr)
{
	usb_bus_addr_t ret;

	ret.ba.bus = bus;
	ret.ba.addr = addr;
	return ret;
}

usb_bus_addr_t parse_bus_addr(const char *str)
{
	char* end;
	long int l;
	uint8_t bus;
	uint8_t addr;

	l = strtol(str, &end, 0);
	if(l < 0)
		die("Bus is negative (was %ld).", l);
	if(l > 0xFF)
		die("Bus is larger than allowed (was %ld).", l);
	if(!l)
		die("Zero is not a valid bus.");
	bus = (uint8_t)l;
	if(*end != ':')
		die("Bus is not followed by a colen (was '%c').", *end);
	end++;
	l = strtol(end, &end, 0);
	if(l < 0)
		die("Address is negative (was %ld).", l);
	if(l > 0xFF)
		die("Address is larger than allowed (was %ld).", l);
	if(!l)
		die("Zero is not a valid address.");
	addr = (uint8_t)l;
	for(;isspace(*end);end++);
	if(*end)
		die("Unexpected characters after bus:address (was %s)", end);
	return mk_bus_addr(bus, addr);
}

usb_bus_addr_t* parse_bus_addr_strs(const char **strs, usb_bus_addr_t *result)
{
	usb_bus_addr_t *result_ptr;

	for(result_ptr=result;*strs;strs++,result_ptr++)
		*result_ptr = parse_bus_addr(*strs);
	return result;
}

static libusb_device *find_dev_by_bus_addr(libusb_device **devs, usb_bus_addr_t bus_addr)
{
	libusb_device **ptr;

	for(ptr=devs;*ptr;ptr++)
	{
		if(libusb_get_bus_number(*ptr) != bus_addr.ba.bus)
			continue;
		if(libusb_get_device_address(*ptr) == bus_addr.ba.addr)
			return *ptr;
	}
	return NULL;
}

static libusb_device **find_devs_by_bus_addrs(libusb_device **all_devs, const usb_bus_addr_t *bus_addrs, libusb_device **devs)
{
	libusb_device **dev_ptr;

	for(dev_ptr=devs;bus_addrs->u16;bus_addrs++,dev_ptr++)
		if(!(*devs = find_dev_by_bus_addr(all_devs, *bus_addrs)))
			die("Unable to find device %03o:%03o", bus_addrs->ba.bus, bus_addrs->ba.addr);
	return devs;
}

/*
typedef void *(*dev_appy_arg1_t)(libusb_device *dev, void *arg);

static void *devs_apply_arg1(libusb_device **devs, dev_apply_arg1_t func, void *arg)
{
	void *ret;

	for(;*devs;devs++)
		ret = func(*devs, arg);
	return ret;
}
*/

typedef void (*dev_appy_t)(libusb_device *dev);

void devs_apply(libusb_device **devs, dev_apply_t func)
{
	for(;*devs;devs++)
		func(*devs);
}

static void dev_ref_apply(libusb_device *dev)
{
	libusb_ref_device(dev);
}

void devs_add_ref(libusb_device **devs)
{
	devs_apply(devs, dev_ref_apply);
}

void devs_del_ref(libusb_device **devs)
{
	devs_apply(devs, libusb_unref_device);
}

libusb_device **find_devs_by_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, ssize_t *num_devs_ret)
{
	libusb_device ** all_devs = NULL;
	libusb_device ** devs = NULL;
	struct libusb_device_descriptor desc;
	ssize_t num_all_devs = 0;
	ssize_t num_devs=0;
	int usb_err;
	ssize_t i;

	if((num_all_devs = libusb_get_device_list(ctx, &all_devs))<0)
		die_usb(num_all_devs, "Unable to enumerate devices");
	if(!(devs = (libusb_device **)malloc(sizeof(libusb_device*)*(num_all_devs+1))))
		die_std("Failed to malloc device array");
	for(i=0;i<num_all_devs;i++)
	{
		if((usb_err = libusb_get_device_descriptor(all_devs[i], &desc)))
			die_usb(usb_err, "Unable to get device descriptor");
		if(desc.idVendor == vendor && desc.idProduct == product)
			devs[num_devs++] = all_devs[i];
		else
			libusb_unref_device(all_devs[i]);
	}
	libusb_free_device_list(all_devs, false);
	devs[num_devs] = NULL;
	if(num_devs_ret)
		*num_devs_ret = num_devs;
	if(!num_devs)
	{
		free(devs);
		return NULL;
	}
	return devs;
}

libusb_device_handle **find_dev_handles_by_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, ssize_t *num_devs_ret)
{
	libusb_device ** devs;
	libusb_device_handle ** dev_handles;
	ssize_t local_num;

	if(!(devs = find_devs_by_vend_prod(ctx, vendor, product, &local_num)))
		die("Failed to find devices by vendor and product.");
	if(!(dev_handles = devs_open_handles(devs, local_num)))
		die("Failed to open devices");
	free_devs(devs);
	if(num_devs_ret)
		*num_devs_ret = local_num;
	return dev_handles;
}

static size_t count_vector(const void **v)
{
	size_t c = 0;

	for(;*v;v++,c++);
	return c;
}

static size_t count_bus_addr_vector(const usb_bus_addr_t *bus_addrs)
{
	size_t c = 0;

	for(;bus_addrs->u16;bus_addrs++,c++);
	return c;
}

static void devs_check_vendor_product(uint16_t vendor, uint16_t product, libusb_device **devs)
{
	int usb_err; 
	struct libusb_device_descriptor desc;

	for(;*devs;devs++)
	{
		if((usb_err = libusb_get_device_descriptor(*devs, &desc)))
			die_usb(usb_err, "Unable to get device descriptor");
		if(desc.idVendor != vendor && desc.idProduct != product)
			die("Device %03o:%03o is not %04x:%04x but is %04x:%04x.",
				libusb_get_bus_number(*devs),
				libusb_get_device_address(*devs),
				vendor, product,
				desc.idVendor, desc.idProduct);
	}
}

libusb_device **find_devs_by_bus_addr_strs_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, const char** bus_addr_strs, size_t num_bus_addr_strs)
{
	usb_bus_addr_t *bus_addrs;
	libusb_device ** devs;

	if(num_bus_addr_strs == (size_t)(-1))
		num_bus_addr_strs = count_vector((const void **)bus_addr_strs);
	if(!(bus_addrs = (usb_bus_addr_t*)malloc(sizeof(usb_bus_addr_t)*(num_bus_addr_strs+1))))
		die_std("Failed to malloc bus address array.");
	bus_addrs[num_bus_addr_strs] = usb_null_bus_addr;
	if(!parse_bus_addr_strs(bus_addr_strs, bus_addrs))
		die("Unable to parse bus address strings");
	if(!(devs = find_devs_by_bus_addr_vend_prod(ctx, vendor, product, bus_addrs, num_bus_addr_strs)))
		die("Unable to find devices by bus, address, vendor and product.");
	free(bus_addrs);
	return devs;
}

libusb_device **find_devs_by_bus_addr_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, const usb_bus_addr_t *bus_addrs, size_t num_bus_addrs)
{
	libusb_device ** all_devs;
	ssize_t num_all_devs;
	libusb_device ** devs;

	if(num_bus_addrs == (size_t)(-1))
		num_bus_addrs = count_bus_addr_vector(bus_addrs);
	if(!(devs = (libusb_device **)malloc(sizeof(libusb_device*)*(num_bus_addrs+1))))
		die_std("Failed to malloc device array");
	devs[num_bus_addrs] = NULL;
	if((num_all_devs = libusb_get_device_list(ctx, &all_devs))<0)
	{
		free(devs);
		die_usb(num_all_devs, "Unable to enumerate devices");
	}
	if(!find_devs_by_bus_addrs(all_devs, bus_addrs, devs))
		die("Unable to find devices by bus and address");
	devs_add_ref(devs);

	libusb_free_device_list(all_devs, true);

	devs_check_vendor_product(vendor, product, devs);

	return devs;
}

libusb_device_handle **devs_open_handles(libusb_device **devs, size_t num_devs)
{
	libusb_device_handle **dev_handles;
	int usb_err;
	size_t i;

	if(num_devs == (size_t)(-1))
		num_devs = count_vector((const void**)devs);
	if(!(dev_handles = (libusb_device_handle**)malloc(sizeof(libusb_device_handle*)*(num_devs+1))))
		die_std("Unable to allocate array for device handles.");
	dev_handles[num_devs] = NULL;
	for(i=0;i<num_devs;i++)
	{
		if((usb_err = libusb_open(devs[i], &(dev_handles[i]))))
			die_usb(usb_err, "Unable to open device %03o:%03o", 
				libusb_get_bus_number(devs[i]),
				libusb_get_device_address(devs[i]));
	}
	return dev_handles;
}

libusb_device_handle **find_dev_handles_by_bus_addr_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, const usb_bus_addr_t *bus_addrs, size_t num_bus_addrs)
{
	libusb_device **devs;
	libusb_device_handle **dev_handles;

	if(num_bus_addrs == (size_t)(-1))
		num_bus_addrs = count_bus_addr_vector(bus_addrs);
	if(!(devs = find_devs_by_bus_addr_vend_prod(ctx, vendor, product, bus_addrs, num_bus_addrs)))
		die("Unable to find devices by bus, addr, vendor and product.");
	if(!(dev_handles = devs_open_handles(devs, num_bus_addrs)))
		die("Unable to open device handles.");
	free_devs(devs);
	return dev_handles;
}

libusb_device_handle **find_dev_handles_by_bus_addr_strs_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, const char **bus_addr_strs, size_t num_bus_addr_strs)
{
	libusb_device **devs;
	libusb_device_handle **dev_handles;

	if(num_bus_addr_strs == (size_t)(-1))
		num_bus_addr_strs = count_vector((const void **)bus_addr_strs);
	if(!(devs = find_devs_by_bus_addr_strs_vend_prod(ctx, vendor, product, bus_addr_strs, num_bus_addr_strs)))
		die("Unable to find devices by bus, addr, vendor and product.");
	if(!(dev_handles = devs_open_handles(devs, num_bus_addr_strs)))
		die("Unable to open device handles.");
	free_devs(devs);
	return dev_handles;
}

void free_devs(libusb_device **devs)
{
	if(!devs)
		return;
	devs_del_ref(devs);
	free(devs);
}

void free_dev_handles(libusb_device_handle **dev_handles)
{
	if(!dev_handles)
		return;
	dev_handles_close(dev_handles);
	free(dev_handles);
}

typedef void (*dev_handle_appy_t)(libusb_device_handle *dev_handle);

void dev_handles_apply(libusb_device_handle **dev_handles, dev_handle_apply_t func)
{
	for(;*dev_handles;dev_handles++)
		func(*dev_handles);
}

void dev_handles_close(libusb_device_handle **dev_handles)
{
	dev_handles_apply(dev_handles, libusb_close);
}

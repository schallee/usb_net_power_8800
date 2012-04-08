#ifndef usb_h
#define usb_h

#include <libusb-1.0/libusb.h>
#include <stdint.h>
#include <sys/types.h>
	/* for ssize_t */

/**
 * Enumerates usb devices looking for bus and addr.
 * @param ctx The libusb context
 * @param bus The bus the devis on
 * @param addr The address  on that bus
 * @returns pointer to device or NULL if not found.
 */
libusb_device *find_dev_by_bus_addr(libusb_context *ctx, uint8_t bus, uint8_t addr);

/**
 * Enumerates usb devices and returns matches.
 * @param ctx The libusb context
 * @param vendor The vendor to look for
 * @param product The product to look for
 * @param num_devs_found If not NULL, pointer to ssize_t to hold the
 * 	number found.
 * @returns Malloced array of pointers to devices devices or NULL if
 *	none were found. The returned array is terminated with a NULL
 * 	entry.
 */
libusb_device **find_devs_by_vend_prod(libusb_context *ctx, uint16_t vendor, uint16_t product, ssize_t *num_devs_found);

/**
 * Unrefferences and frees a array of device pointers.
 * @param devs Array of devices to free. This should be terminated with
 * 	NULL. A NULL value is allowed here.
 */
void free_devs(libusb_device **devs);

#endif

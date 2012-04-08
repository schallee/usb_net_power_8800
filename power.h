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

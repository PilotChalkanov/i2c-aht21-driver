#!/bin/bash

# Script to unload the I2C AHT21 driver module

MODULE_NAME="aht21"
DEVICE_NAME="aht21"

echo "Unloading ${MODULE_NAME} module..."

if lsmod | grep -q "${MODULE_NAME}"; then
    sudo rmmod "${MODULE_NAME}"
    echo "${MODULE_NAME} module unloaded successfully!"
else
    echo "${MODULE_NAME} module is not loaded"
    exit 1
fi

# Remove the device file
if [ -e "/dev/${DEVICE_NAME}" ]; then
    echo "Removing /dev/${DEVICE_NAME}..."
    sudo rm -f "/dev/${DEVICE_NAME}"
fi

# Verify the module is unloaded
if ! lsmod | grep -q "${MODULE_NAME}"; then
    echo "Module removal verified"
    dmesg | tail -3
else
    echo "Error: Failed to unload ${MODULE_NAME} module"
    exit 1
fi

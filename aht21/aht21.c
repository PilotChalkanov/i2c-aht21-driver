
/* Copyright 2026 Nikolay Chalkanov */
/*
AHT21 Temperature and Humidity Sensor Driver
Author: Nikolay Chalkanov
This driver interfaces with the AHT21 sensor over I2C, allowing userspace applications to read temperature and humidity data. The driver registers a misc device for user interaction and handles I2C

From the manufacturer's datasheet, the AHT21 sensor operates as follows:

## 1. Send CMD
1.1. The MCU initiates communication by sending a start condition followed by the 7-bit I2C device address (0x38)
 and a SDA direction bit x (read R:‘1’, write W: ‘0’).
1.2. After the falling edge of the 8th SCL clock, the SDA pin (ACK) is pulled low to indicate that
the sensor data reception is normal.
1.3.  After issuing the initialization command 0xBE and the measurement command 0xAC, the MCU must wait until the
measurement is completed.

## 2. Read Measurement
2.1.  Wait 40ms after power-on. Before reading the temperature and humidity values, first check whether
    the calibration enable bit Bit [3] of the status word is 1
    (you can get a byte of status word by sending 0x71). If not 1, need to send 0xBE command (for initialization),
    this command parameter has two bytes, the first byte is 0x08, the second byte is 0x00, and then wait for 10ms.
2.2.  Send the 0xAC command directly (trigger measurement). The parameter of this command has
    two bytes, the first byte is 0x33 and the second byte is 0x00.
2.3.  Wait for 80ms to wait for the measurement to be completed. If the read status word Bit [7] is 0,
    it indicates that the measurement is completed, and then six bytes can be read in a row; otherwise,
    continue to wait.
2.4.  After receiving six bytes, the next byte is the CRC check data, the user can read it as needed.
    If the receiving end needs CRC check, then send it after receiving the sixth byte ACK response,
    otherwise NACK is sent out. CRC initial value is 0XFF, CRC8 check polynomial is:
2.5.  Calculate the temperature and humidity values.
    Note: The calibration status check in the first step only needs to be checked at power-on.
    No operation is required during the normal acquisition process.
*/

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "aht21.h"

#define DEVICE_NAME "aht21"
#define AHT21_I2C_ADDR 0x38

// CMDs
#define AHT21_CMD_INIT 0xBE
#define AHT21_CMD_MEASURE 0xAC
#define AHT21_CMD_RESET 0xBA


struct aht21_data {
    struct i2c_client *client;
    struct miscdevice miscdev;
};



/*
Initial implementation of file operations.
The open, read, and release functions are currently placeholders
and need to be implemented to handle user interactions with the device.
*/
static int aht21_open(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t aht21_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    return 0;
}

static int aht21_release(struct inode *inode, struct file *file) {
    return 0;
}

/*
Initializes the AHT21 sensor by sending the init CMD as per datsheet 1.1, preparing it for measurement.
*/
static int aht21_init_sensor(struct i2c_client *client) {
    u8 init_cmd[3] = {AHT21_CMD_INIT, 0x08, 0x00};
    int ret;

    dev_info(&client->dev, "Initializing AHT21 sensor\n");

    ret = i2c_master_send(client, init_cmd, 3);
    if (ret < 0) {
        dev_err(&client->dev, "Failed to initialize sensor: %d\n", ret);
        return ret;
    }

    msleep(10);  // Wait for initialization
    return 0;
}

static const struct file_operations aht21_fops = {
    .owner = THIS_MODULE,
    .open = aht21_open,
    .read = aht21_read,
    .release = aht21_release,
};

/*
Driver probe func.
Check for I2C functionality, allocate memory for device data, register misc device, and set client data.
*/
static int aht21_probe(struct i2c_client *client, const struct i2c_device_id *id) {
    struct aht21_data *aht21;
    PDEBUG("AHT21 sensor probed successfully\n");
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        PDEBUG("AHT21: I2C functionality not supported\n");
        return -EIO;
    }
    // init sensor, so that it is ready to read
    if (aht21_init_sensor(client)) {
        PDEBUG("AHT21: Sensor initialization failed\n");
        return -EIO;
    }
    aht21 = devm_kzalloc(&client->dev, sizeof(struct aht21_data), GFP_KERNEL);
    if (!aht21) {
        PDEBUG("AHT21: Failed to allocate memory\n");
        return -ENOMEM;
    }
    aht21->client = client;
    aht21->miscdev.minor = MISC_DYNAMIC_MINOR;
    aht21->miscdev.name = DEVICE_NAME;
    aht21->miscdev.fops = &aht21_fops;
    aht21->miscdev.parent = &client->dev;
    if (misc_register(&aht21->miscdev)) {
        PDEBUG("AHT21: Failed to register misc device\n");
        return -EIO;
    }
    i2c_set_clientdata(client, aht21);
    PDEBUG("AHT21 sensor initialized successfully\n");
    return 0;
}

static int aht21_remove(struct i2c_client *client) {
    struct aht21_data *data = i2c_get_clientdata(client);
    if (data) {
        devm_kfree(&client->dev, data);
        misc_deregister(&data->miscdev);
    }
    PDEBUG("AHT21 sensor removed\n");
    return 0;
}

static const struct of_device_id aht21_of_match[] = {
    { .compatible = "sensaht21,aht21" },
    { }
};
MODULE_DEVICE_TABLE(of, aht21_of_match);

static const struct i2c_device_id aht21_id[] = {
    { "aht21", 0 },
    { }
};

static struct i2c_driver aht21_driver = {
    .driver = {
        .name = "aht21",
        .of_match_table = aht21_of_match,
        .owner = THIS_MODULE,
    },
    .probe = aht21_probe,
    .remove = aht21_remove,
    .id_table = aht21_id,
};

module_i2c_driver(aht21_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikolay Chalkanov");
MODULE_DESCRIPTION("AHT21 Temperature and Humidity Sensor Driver");
MODULE_VERSION("1.0");

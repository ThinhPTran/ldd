#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include "exb.h"

MODULE_AUTHOR("Nguyen Tien Dat");
MODULE_LICENSE("Dual BSD/GPL");

/******************************** exb driver *****************************/
static ssize_t show_driver_desc(struct device_driver *driver, char *buf)
{
	struct exb_driver *exb_drv = to_exb_driver(driver);
	sprintf(buf, "%s\n", exb_drv->desc);
	return strlen(buf);
}
//tao ra bien driver_attr_driver_desc
DRIVER_ATTR(driver_desc, S_IRUGO, show_driver_desc, NULL);

int exb_register_driver(struct exb_driver *exb_drv)
{
	int ret;
	// dang ky exb_driver voi device model
	exb_drv->driver.bus = &exb_bus_type;
	ret = driver_register(&exb_drv->driver);
	if (ret)
		return ret;

	// tao mot file ung voi driver_attribute trong sysfs
	ret = driver_create_file(&exb_drv->driver, &driver_attr_driver_desc);

	printk("register exb driver completed\n");
	return ret;
}
EXPORT_SYMBOL(exb_register_driver);

void exb_unregister_driver(struct exb_driver *exb_drv)
{
	driver_unregister(&exb_drv->driver);
	printk("unregister exb driver completed\n");
}
EXPORT_SYMBOL(exb_unregister_driver);

/****************************** exb bus controller ******************************/
static char *message = "$Viet Nam hung cuong $";

// tao ra cac su kien uevent.
static int exb_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	if (add_uevent_var(env, "EXBBUS_MSG=%s", message))
		return -ENOMEM;

	printk("create an uevent\n");
	return 0;
}

// kiem tra xem driver co the dieu khien duoc device khong.
static int exb_match(struct device *dev, struct device_driver *driver)
{
	int ret = strncmp(dev_name(dev), driver->name, strlen(driver->name));
	if (ret == 0)
		printk("found a exb driver for the device\n");
	return (!ret);
}

// dinh nghia kieu bus exb_bus_type
struct bus_type exb_bus_type = {
        .name = "exb",
        .match = exb_match,
        .uevent  = exb_uevent,
};

// ham giai phong exb host controller
static void exb_host_release(struct device *dev)
{
        printk(KERN_DEBUG "release exb host controller\n");
}

// dinh nghia thiet bi exb host controller.
struct device exb_host = {
        .init_name = "exb_host_controller",
        .release  = exb_host_release
};

// tao ra mot cau truc bus_attr_bus_msg
static ssize_t show_bus_msg(struct bus_type *bus, char *buf)
{
        return snprintf(buf, PAGE_SIZE, "%s\n", message);
}
static BUS_ATTR(bus_msg, S_IRUGO, show_bus_msg, NULL);

static int __init exb_bus_init(void)
{
	int ret;

	//dang ky kieu bus exb_bus_type voi device model
	ret = bus_register(&exb_bus_type);
	if (ret)
		return ret;
	printk("register exb_bus_type with device model successfully\n");

	//tao ra mot file trong sysfs tuong ung voi thuoc tinh bus_msg
	if (bus_create_file(&exb_bus_type, &bus_attr_bus_msg))
		printk(KERN_ERR "Unable to create bus_attr_bus_msg\n");

	//dang ky exb host controller voi device model
	ret = device_register(&exb_host);
	if (ret)
		printk(KERN_ERR "Unable to register %s\n", exb_host.init_name);
	printk("register exb host controller successfully\n");
	return ret;
}

static void exb_bus_exit(void)
{
	device_unregister(&exb_host);
	bus_unregister(&exb_bus_type);
}

module_init(exb_bus_init);
module_exit(exb_bus_exit);

/******************************** exb device *****************************/
/*
 * ham giai phong exb device
 */
static void exb_dev_release(struct device *dev)
{
        printk(KERN_DEBUG "release exb device\n");
}


int register_exb_device(struct exb_dev *exbdev)
{
	int ret = 0;
        exbdev->dev.bus = &exb_bus_type;//kieu bus cua thiet bi
        exbdev->dev.parent = &exb_host;//thiet bi cha cua exbdev la exb_bus
        exbdev->dev.release = exb_dev_release;
        ret = device_register(&exbdev->dev);
	printk("register exb device completed\n");
	return ret;
}
EXPORT_SYMBOL(register_exb_device);

void unregister_exb_device(struct exb_dev *exbdev)
{
        device_unregister(&exbdev->dev);
	printk("unregister exb device completed\n");
}
EXPORT_SYMBOL(unregister_exb_device);


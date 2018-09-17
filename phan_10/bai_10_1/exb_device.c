#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/uaccess.h>
#include "exb.h"		/* local definitions */

MODULE_AUTHOR("Nguyen Tien Dat");
MODULE_LICENSE("Dual BSD/GPL");

#define DEV_NAME "VN_exb"

struct exb_dev * exb_device;

static ssize_t exb_show_dev_desc(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct exb_dev *edev = dev_get_drvdata(dev);

	return sprintf(buf, "%s", edev->desc);
}

static DEVICE_ATTR(dev_desc, S_IRUGO, exb_show_dev_desc, NULL);

int exb_device_init(void)
{
	// dang ky device voi device model
	exb_device = kmalloc(sizeof(struct exb_dev), GFP_KERNEL);
	if (!exb_device) {
		return -ENOMEM;
	}
	memset(exb_device, 0, sizeof(struct exb_dev));

	exb_device->desc = kmalloc(64 * sizeof(char), GFP_KERNEL);
	sprintf(exb_device->desc, "Thiet bi exb duoc san xuat tai Viet Nam\n");
	dev_set_name(&exb_device->dev, DEV_NAME);
	dev_set_drvdata(&exb_device->dev, exb_device);
	register_exb_device(exb_device);

	device_create_file(&exb_device->dev, &dev_attr_dev_desc);
	printk(KERN_INFO "init exb device done");

	return 0;
}

void exb_device_cleanup(void)
{
	unregister_exb_device(exb_device);
	kfree(exb_device);
}

module_init(exb_device_init);
module_exit(exb_device_cleanup);

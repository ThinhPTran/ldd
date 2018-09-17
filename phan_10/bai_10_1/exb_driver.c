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

#define DEV_COMPATIBLE "VN_exb"

int exb_driver_probe(struct device *dev)
{
	printk("exb_driver_probe return success\n");
	return 0;//tra ve 0 de thong bao thanh cong
}

static struct exb_driver exb_drv = {
        .desc = "Driver duoc viet boi ky su Viet Nam",
        .module = THIS_MODULE,
        .driver = {
                .name = DEV_COMPATIBLE,
		.probe = exb_driver_probe,
        },
};

int exb_driver_init(void)
{
	/*
	 * dang ky driver voi device model
	 */
	exb_register_driver(&exb_drv);

	return 0;
}

void exb_driver_cleanup(void)
{
	exb_unregister_driver(&exb_drv);
}

module_init(exb_driver_init);
module_exit(exb_driver_cleanup);

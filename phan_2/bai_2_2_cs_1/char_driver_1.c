/*
 * char_driver_1.c - vi du ve cap phat tinh device number
 */
#include <linux/module.h> /* tat ca cac module deu can thu vien nay */
#include <linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */

#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "An example about allocating device number statically"
 
dev_t dev_num = MKDEV(235, 0);
static int __init char_driver_init(void)
{
	/* cap phat tinh device number */
	register_chrdev_region(dev_num, 1, "char_dev");
	printk("Insert character driver successfully\n");

	return 0;
}

void __exit char_driver_exit(void)
{
	unregister_chrdev_region(dev_num, 1);
	printk("Remove character driver successfully\n");
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("2.1");
MODULE_SUPPORTED_DEVICE("testdevice");


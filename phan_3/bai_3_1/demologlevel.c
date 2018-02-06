/*  
 *  demologlevel.c - Demonstrates how to use log level
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */

#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "A demostration about log levels"

static int __init init_demo_loglevel(void)
{
	printk("default level\n");
	printk(KERN_EMERG "emergency level\n");
	printk(KERN_ALERT "alert level\n");
	printk(KERN_CRIT "critical level\n");
	printk(KERN_ERR "error level\n");
	printk(KERN_WARNING "warning level\n");
	printk(KERN_NOTICE "notice level\n");
	printk(KERN_INFO "information level\n");
	printk(KERN_DEBUG "debug level\n");
	return 0;
}

static void __exit exit_demo_loglevel(void)
{
	printk(KERN_INFO "end demo log level\n");
}

module_init(init_demo_loglevel);
module_exit(exit_demo_loglevel);

MODULE_LICENSE("GPL"); /* What license of this module? */
MODULE_AUTHOR(DRIVER_AUTHOR); /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */
MODULE_SUPPORTED_DEVICE("virtualmodule"); /* Which types of devices does this module supports */

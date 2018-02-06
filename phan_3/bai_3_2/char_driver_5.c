/*
 * char_driver_5.c - minh hoa dung printk de debug trong character driver
 */
#include <linux/module.h> /* tat ca cac module deu can thu vien nay */
#include <linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */

#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "An simple example about character driver"
#define MEM_SIZE 1024

dev_t dev_num = 0;
static struct class * device_class;
static struct cdev *example_cdev;
uint8_t *kernel_buffer;
unsigned open_cnt = 0;

static int example_open(struct inode *inode, struct file *filp);
static int example_release(struct inode *inode, struct file *filp);
static ssize_t example_read(struct file *filp, char __user *user_buf, size_t len, loff_t * off);
static ssize_t example_write(struct file *filp, const char *user_buf, size_t len, loff_t * off);
 
static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = example_read,
	.write          = example_write,
	.open           = example_open,
	.release        = example_release,
};
 
static int example_open(struct inode *inode, struct file *filp)
{
	open_cnt++;
	printk("Handle opened event %u times\n", open_cnt);
	return 0;
}

static int example_release(struct inode *inode, struct file *filp)
{
        printk("Handle closed event %u times\n", open_cnt);
        return 0;
}
 
static ssize_t example_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	copy_to_user(user_buf, kernel_buffer, MEM_SIZE);
	printk("Handle read event %u times\n", open_cnt);
	return MEM_SIZE;
}
static ssize_t example_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	copy_from_user(kernel_buffer, user_buf, len);
	printk("Handle write event %u times\n", open_cnt);
	return len;
}
 

static int __init char_driver_init(void)
{
	/* cap phat dong device number */
	if(alloc_chrdev_region(&dev_num, 0, 1, "char_dev") < 0) {
		printk(KERN_ERR "Cannot allocate device number\n");
		return -1;
	}
	printk("Insert character driver successfully. major(%d), minor(%d)\n", MAJOR(dev_num), MINOR(dev_num));

	/* tao device file /dev/char_device */
	if((device_class = class_create(THIS_MODULE, "class_char_dev")) == NULL) {
		printk(KERN_ERR "Cannot create class device\n");
		goto free_dev_num;
	}
	if(device_create(device_class, NULL, dev_num, NULL,"char_device") == NULL) {
		printk(KERN_ERR "Cannot create device\n");
		goto remove_class;
	}

	/* tao kernel buffer */
	if((kernel_buffer = kmalloc(MEM_SIZE , GFP_KERNEL)) == 0) {
		printk(KERN_ERR "Cannot allocate memory in kernel\n");
		goto remove_device;
	}

	/* lien ket cac ham entry point cua driver voi device file */
	if((example_cdev = cdev_alloc()) == NULL) {
		printk(KERN_ERR "Cannot create cdev structure\n");
		goto free_kbuffer;
	}
	cdev_init(example_cdev, &fops);
	if(cdev_add(example_cdev, dev_num, 1) < 0) {
		printk(KERN_ERR "Cannot bind device number and cdev\n");
		goto remove_cdev;
	}
	return 0;

remove_cdev:
	cdev_del(example_cdev);
free_kbuffer:
	kfree(kernel_buffer);
remove_device:
	device_destroy(device_class, dev_num);
remove_class:
	class_destroy(device_class);
free_dev_num:
	unregister_chrdev_region(dev_num, 1);

	return -1;
}

void __exit char_driver_exit(void)
{
	cdev_del(example_cdev);
	kfree(kernel_buffer);
	device_destroy(device_class,dev_num);
	class_destroy(device_class);
	unregister_chrdev_region(dev_num, 1);
	printk("Remove character driver successfully.\n");
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("3.1");
MODULE_SUPPORTED_DEVICE("testdevice");


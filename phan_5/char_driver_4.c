/*
 * char_driver_4.c - character driver hoan chinh
 */
#include <linux/module.h> /* tat ca cac module deu can thu vien nay */
#include <linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */
#include <linux/ioctl.h>

typedef struct {
	int rd;
	int wr;
} status_t;

#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "A simple example about character driver"
#define MEM_SIZE 1024
#define ENABLE 1
#define DISABLE 0
#define RESET_STATISTIC _IO(200, 0)
#define WR_CONTROL_DEV _IOW(200, 1, int32_t *)
#define RD_STATUS_DEV _IOR(200, 2, status_t *)

dev_t dev_num = 0;
static struct class * device_class;
static struct cdev *example_cdev;
uint8_t *kernel_buffer;
unsigned open_cnt = 0;
unsigned control = ENABLE;

static int example_open(struct inode *inode, struct file *filp);
static int example_release(struct inode *inode, struct file *filp);
static ssize_t example_read(struct file *filp, char __user *user_buf, size_t len, loff_t * off);
static ssize_t example_write(struct file *filp, const char *user_buf, size_t len, loff_t * off);
static long example_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = example_read,
	.write          = example_write,
	.open           = example_open,
	.unlocked_ioctl = example_ioctl,
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
	if (control == DISABLE) return 0;

	copy_to_user(user_buf, kernel_buffer, MEM_SIZE);
	printk("Handle read event %u times\n", open_cnt);
	return MEM_SIZE;
}
static ssize_t example_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	if (control == DISABLE) return 0;

	copy_from_user(kernel_buffer, user_buf, len);
	printk("Handle write event %u times\n", open_cnt);
	return len;
}
 
static long example_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
		case RESET_STATISTIC:
			open_cnt = 0;
			printk("reset open_cnt = %d\n", open_cnt);
			break;
		case WR_CONTROL_DEV:
			copy_from_user(&control, (int32_t*) arg, sizeof(control));
			printk("%s device\n", (control == DISABLE)?"disable":"enable");
			break;
		case RD_STATUS_DEV:
		{
			status_t status;
			if(control == DISABLE) {
				status.rd = 0;
				status.wr = 0;
			} else {
				status.rd = 1;
				status.wr = 1;
			}
			copy_to_user((status_t*)arg, &status, sizeof(status));
		}
			break;
	}
	return 0;
}
static int __init char_driver_init(void)
{
	/* cap phat dong device number */
	alloc_chrdev_region(&dev_num, 0, 1, "char_dev");
	printk("Insert character driver successfully. major(%d), minor(%d)\n", MAJOR(dev_num), MINOR(dev_num));

	/* tao device file /dev/char_device */
	device_class = class_create(THIS_MODULE, "class_char_dev");
	device_create(device_class, NULL, dev_num, NULL,"char_device");

	/* tao kernel buffer */
	kernel_buffer = kmalloc(MEM_SIZE , GFP_KERNEL);

	/* lien ket cac ham entry point cua driver voi device file */
	example_cdev = cdev_alloc();
	cdev_init(example_cdev, &fops);
	cdev_add(example_cdev, dev_num, 1);

	return 0;
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
MODULE_VERSION("2.4");
MODULE_SUPPORTED_DEVICE("testdevice");


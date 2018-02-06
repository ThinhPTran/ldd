/*
 * char_driver_6.c - su dung procfs de debug
 */
#include <linux/module.h> /* tat ca cac module deu can thu vien nay */
#include <linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */
#include<linux/proc_fs.h> /* thu vien chua cac ham tao/huy file trong procfs */

#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "An simple example about character driver"
#define MEM_SIZE 1024
#define NUM_STATUS 8192

dev_t dev_num = 0;
static struct class * device_class;
static struct cdev *example_cdev;
uint8_t *kernel_buffer;
unsigned open_cnt = 0;

/* cau truc luu tru trang thai cua module nay */
typedef struct status_s {
	unsigned int key;
	unsigned char value;
} status_t;
status_t status_module[NUM_STATUS];
int read = 1;

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

static int open_proc(struct inode *inode, struct file *file);
static int release_proc(struct inode *inode, struct file *file);
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);

static struct file_operations proc_fops = {
	.open = open_proc,
	.read = read_proc,
	.write = write_proc,
	.release = release_proc
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

static int open_proc(struct inode *inode, struct file *file)
{
	printk("Handle opening proc file\t");
	return 0;
}
 
static int release_proc(struct inode *inode, struct file *file)
{
	printk("Handle closing proc file\n");
	return 0;
}
 
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{
	printk("Handle reading proc file\n");
	if(read)
		read = 0;
	else{
		read = 1;
		return 0;
	}

	int i;
	char tmp[20];
	int num;
	int start = 0;
	for(i = 0; i < NUM_STATUS; i++) {
		num = sprintf(tmp, "status %u: %u\n", status_module[i].key, status_module[i].value);
		copy_to_user(buffer + start, tmp, num);
		start += num;
	}

	return length;
}
 
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	printk(KERN_INFO "No action to handle writing proc file.\n");
	return len;
}

static int __init char_driver_init(void)
{
	int i;
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

	/* khoi tao status_kbuf va control_kbuf */
	for(i = 0; i < NUM_STATUS; i++) {
		status_module[i].key = i;
		status_module[i].value = i % 256;
	}

	/* tao file trong thu muc /proc */
	proc_create("example_proc", 0666, NULL, &proc_fops);

	return 0;
}

void __exit char_driver_exit(void)
{
	remove_proc_entry("example_proc", NULL);
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
MODULE_VERSION("3.2");
MODULE_SUPPORTED_DEVICE("testdevice");


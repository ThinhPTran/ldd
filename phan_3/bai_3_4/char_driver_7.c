/*
 * char_driver_7.c - su dung procfs de debug
 */
#include<linux/module.h> /* tat ca cac module deu can thu vien nay */
#include<linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */
#include<linux/proc_fs.h> /* thu vien chua cac ham tao/huy file trong procfs */
#include<linux/seq_file.h> /* thu vien chua cac ham phuc vu doc du lieu tu /proc file */

#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "An simple example about character driver"
#define MEM_SIZE 1024
#define NUM_STATUS 65536

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

static void *example_seq_start(struct seq_file *s, loff_t *pos)
{
	static unsigned long count = 0;

	printk("seq_start: *pos(%Ld), count(%lu)\n", *pos, count);
	if ( *pos == 0 ) {
		return &count;
	} else {
		*pos = 0;
		return NULL; //ket thuc qua trinh doc tu /proc file
	}
}

static int example_seq_show(struct seq_file *s, void *v)
{
	unsigned long *c = (unsigned long *) v;//day la bien count
	for(*c = 0; *c < NUM_STATUS; (*c)++)
		seq_printf(s, "status %u: %u\n", status_module[*c].key, status_module[*c].value);

	printk("seq_show *v(%lu)\n", *c);
	return 0;
}

static void *example_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	unsigned long *c = (unsigned long *)v;//v bien count trong example_seq_start
	if(*c < NUM_STATUS) {
		printk("seq_next *v(%lu)\n", *c);
		return v;
	}

	*pos = 1;
	printk("seq_next set *pos(%Lu). Go to seq_stop\n", *pos);
	return NULL;//chuyen toi seq_stop
}

static void example_seq_stop(struct seq_file *s, void *v)
{
	printk("seq_stop: nothing to do\n");
}

static struct seq_operations seq_ops = {
	.start = example_seq_start,
	.next  = example_seq_next,
	.stop  = example_seq_stop,
	.show  = example_seq_show
};

/* Khi user application mo file /proc/example_proc, ham nay duoc goi */
static int proc_open(struct inode *inode, struct file *file)
{
	printk("open proc file\n");
	return seq_open(file, &seq_ops);
};

/* Cau truc nay chua cac ham quan ly /proc file */
static struct file_operations proc_fops = {
	.owner   = THIS_MODULE,
	.open    = proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};

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
MODULE_VERSION("3.3");
MODULE_SUPPORTED_DEVICE("testdevice");


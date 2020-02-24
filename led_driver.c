#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>

#include "led_operation.h"


static int led_drv_open(struct inode *node, struct file *file);
static ssize_t led_drv_read(struct file *file, char __user *buffer, size_t size, loff_t *offset);
static ssize_t led_drv_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset);
static int led_drv_release(struct inode *node, struct file *file);



#define MIN(a, b) (a < b ? a : b)

/*1.确定主设备号            0为让内核分配*/

static int major = 0;

// static char kernel_buffer[1024];

static struct class *board_led_class;

struct led_operations *pointer_led_opr;

/*2.定义自己的file_operation结构体     */

static const struct file_operations led_drv = {
	.owner	 = THIS_MODULE,
	.open    = led_drv_open,
	.read    = led_drv_read,
	.write   = led_drv_write,
	.release = led_drv_release,
};

/*3.实现2中定义的函数*/

static int led_drv_open (struct inode *node, struct file *file)
{
    /*从node节点中获取次设备号，利用iminor(node)函数实现*/
    int minor = iminor(node);
    printk("%s %s line %d\n",__FILE__, __FUNCTION__, __LINE__);
    /*根据次设备号 和 status 初始化LED和BEEP的GPIO*/
    pointer_led_opr->init(minor);
    return 0;
}


static ssize_t led_drv_read (struct file *file, char __user *buffer, size_t size, loff_t *offset)
{
    int err;
    char status;
    struct inode *inode = file_inode(file);
    int minor = iminor(inode);
    printk("%s %s line %d\n",__FILE__, __FUNCTION__, __LINE__);

    status = (char)pointer_led_opr->check(minor);
    err = copy_to_user(buffer,&status,1);
    return 1;
}

/*write(fd, &val, 1);*/
static ssize_t led_drv_write (struct file *file, const char __user *buffer, size_t size, loff_t *offset)
{
    int err;
    char status;
    struct inode *inode = file_inode(file);
    int minor = iminor(inode);
    printk("%s %s line %d\n",__FILE__, __FUNCTION__, __LINE__);
    
    err = copy_from_user(&status,buffer,1);
    /*根据次设备号 和 status 控制LED和BEEP*/
    pointer_led_opr->ctl(minor,status);
    return 1;
}

static int led_drv_release (struct inode *node, struct file *file)
{
    printk("%s %s line %d\n",__FILE__, __FUNCTION__, __LINE__);
    return 0;
}

/*4.把file_operation结构体告诉内核，注册驱动程序*/
/*5.入口函数注册驱动程序，安装驱动程序时，会调用入口函数，同时注册驱动程序*/
static int __init led_init(void)
{
    int err;
    printk("%s %s line %d\n",__FILE__, __FUNCTION__, __LINE__);
    major = register_chrdev(0,"board_led",&led_drv);
    board_led_class = class_create(THIS_MODULE, "board_led_class");
    err = PTR_ERR(board_led_class);
    if (IS_ERR(board_led_class)){
        printk("%s %s line %d\n",__FILE__, __FUNCTION__, __LINE__);
        unregister_chrdev(major,"board_led");
        return -1;
    }

    device_create(board_led_class, NULL,
			     MKDEV(major, 0), NULL, "board_led");
    device_create(board_led_class, NULL,
			     MKDEV(major, 1), NULL, "board_beep");

    pointer_led_opr = get_board_led_operation();

	return 0;
}


/*6.出口函数注销驱动程序     */

void __init led_exit(void)
{
    printk("%s %s line %d\n",__FILE__, __FUNCTION__, __LINE__);
    device_destroy(board_led_class,MKDEV(major, 0));
    device_destroy(board_led_class,MKDEV(major, 1));
    class_destroy(board_led_class);
    unregister_chrdev(major,"board_led");
}


module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");


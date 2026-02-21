/*******************************************************************************
* Copyright (c) 2015 Song Yang @ ittraining
* 
* All rights reserved.
* This program is free to use, but the ban on selling behavior.
* Modify the program must keep all the original text description.
*
* 保留所有權利。
* 本程式可任意使用，但是禁止販售行為。
* 修改程式時必須保留所有原有文字說明。
* 
* Email: onionys@ittraining.com.tw
* Blog : http://blog.ittraining.com.tw
*******************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include<linux/interrupt.h>
#include<linux/delay.h>

// use the completion
#include<linux/completion.h>
// - staticly make the my_completion
DECLARE_COMPLETION(my_completion);

#define MY_MAJOR  200
#define MY_MINOR  0
#define MY_DEV_COUNT 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ITtraining.com.tw");
MODULE_DESCRIPTION("A Simple GPIO Device Driver module for RaspPi");

// -- ABOUT CHAR DEVICE ABOUT REGION
//     -- FILE OPERATION 
static int     my_open( struct inode *, struct file * );
static ssize_t my_read( struct file * ,        char *  , size_t, loff_t *);
static ssize_t my_write(struct file * , const  char *  , size_t, loff_t *);
static int     my_close(struct inode *, struct file * );
struct file_operations my_fops = {
        read    :       my_read,
        write   :       my_write,
        open    :       my_open,
        release :       my_close,
        owner   :       THIS_MODULE
};
//     -- CHAR DEVICE
struct cdev my_cdev;

// -- MESSAGE EXCHANGE BUFF ABOUT
#define MSG_SIZE 32
static char *msg = NULL;


/*
 * INIT_MODULE -- MODULE START --
 * */
int init_module(void)
{
	// -- GET -- DEVICE NUMBER
	dev_t devno;
	unsigned int count = MY_DEV_COUNT;
	int err;
	devno = MKDEV(MY_MAJOR, MY_MINOR);
	register_chrdev_region(devno, count , "mydev");

	// -- GET -- CHAR DEVICE 
	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;
	err = cdev_add(&my_cdev, devno, count);

	// -- CHECK ERROR  
	if (err < 0)
	{
		printk("Device Add Error\n");
		return -1;
	}

	// -- ALLOC A MEMORY BUFFER FOR MESSAGE EXCHANGE
	msg = (char *)kmalloc(MSG_SIZE, GFP_KERNEL);
	if (msg !=NULL)
		printk("malloc allocator address: 0x%p\n", msg);

	printk("Hello World. This is my first char dev.\n");
	printk("'mknod /dev/mydev c %d 0'.\n", MY_MAJOR);

    return 0;
}


/*
 * CLEANUP_MODULE -- MODULE END --
 * */
void cleanup_module(void)
{
	dev_t devno;
	printk("Goodbye\n");

	if (msg){
        /* release the malloc */
        kfree(msg);
	}

	// -- release the char device 
	devno = MKDEV(MY_MAJOR, MY_MINOR);
	unregister_chrdev_region(devno, MY_DEV_COUNT);
	cdev_del(&my_cdev);
}


/*
 * file operation: OPEN 
 * */
static int my_open(struct inode *inod, struct file *fil)
{
	int major;
	int minor;
	major = imajor(inod);
	minor = iminor(inod);
	printk("\n*****Some body is opening me at major %d  minor %d*****\n",major, minor);
	return 0;
}


/*
 * file operation: READ
 * */
static ssize_t my_read(struct file *filp, char *buff, size_t len, loff_t *off)
{
	int major, minor;
	long res;

	major = MAJOR(filp->f_path.dentry->d_inode->i_rdev);
	minor = MINOR(filp->f_path.dentry->d_inode->i_rdev);
	printk("FILE OPERATION READ:%d:%d\n", major, minor);


	// -- COMPLETION : wait_for_completion 
	// -- which will be completed by call 
	// -- write() function in user space.
	printk("sorry there is something to do.\n");
	printk("please wait for complete...\n");
	wait_for_completion(&my_completion);

	strcpy(msg,"READ COMPLETE!");

	if(len < strlen(msg))
		res = copy_to_user( buff, msg, len);
	else
		res = copy_to_user( buff, msg, strlen(msg));

	return res ;
}


/*
 * file operation: WRITE
 * */
static ssize_t my_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	int major,minor;
	long res;
	char _buff[32];
	memset(_buff, 0, 32);

	major = MAJOR(filp->f_path.dentry->d_inode->i_rdev);
	minor = MINOR(filp->f_path.dentry->d_inode->i_rdev);

	printk("FILE OPERATION WRITE:%d:%d\n",major,minor);
	printk("now the thing is done!....Complete!!!!!\n");
	complete(&my_completion);


	if(len <= MSG_SIZE )
		res = copy_from_user( _buff , buff, len );
	else
		res = copy_from_user( _buff , buff, 32);
	printk("msg: %s", _buff);
	return res;
}


/*
 * file operation : CLOSE
 * */
static int my_close(struct inode *inod, struct file *filp)
{
	int minor;
	minor = MINOR(filp->f_path.dentry->d_inode->i_rdev);
	printk("*****Some body is closing me at major %d*****\n",minor);
	return 0;
}

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#define OK 0

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlos Cardoso Dias");
MODULE_DESCRIPTION("A simple hello world Linux module.");
MODULE_VERSION("0.01");

static int lkm_hello_world_init(void){
	int onesec = msecs_to_jiffies(1000);

        pr_info("mykmod loaded %u jiffies\n", (unsigned)onesec);

	msleep(10000);
	return 100;
}

static void __exit lkm_hello_world_exit(void) {
	printk(KERN_INFO "Goodbye, World!\n");
}

module_init(lkm_hello_world_init);
module_exit(lkm_hello_world_exit);


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("linux usr");
MODULE_DESCRIPTION("Example of init_waitqueue_head");

static struct task_struct *wait_thread;
static struct task_struct *wake_up_thread;
wait_queue_head_t wqueue;
int wait_flag = 0;

/* wait_thread_fn - executes when wait_thread is created,
 * waits for an event from wake_up_thread,
 * prints a message and sleeps for 1000ms,
 * stops when kthread_stop is called */

int wait_thread_fn(void *data)
{
	while(!kthread_should_stop()) {
		pr_info("waiting for event \n");
		wait_event_interruptible(wqueue,wait_flag != 0);
		pr_info("thread 1 execution\n");
		msleep(1000);
	}
	return 0;
}

/* wake_up_thread_fn - executes when wake_up_thread is created,
 * prints a message and sleeps for 1000ms,
 * sends wake up signal to wait_thread,
 * stops when kthread_stop is called */

int wake_up_thread_fn(void *data)
{
	while(!kthread_should_stop()) {
		pr_info("thread 2 execution\n");
		wait_flag = 1;
		msleep(1000);
	}
	wake_up_interruptible(&wqueue);
	return 0;
}

/* thread_start - creates thread and starts execution */

void thread_start(void)
{
	void *ptr = NULL;
	//init_waitqueue_head(&wqueue);
	pr_info("initialized waitqueue head\n");
	//msleep(10000);
	struct task_struct *str = NULL;
	printk("hello\n");
	printk("wait_thread is %p\n", wait_thread);
	wait_thread = __kmalloc_noprof(800, GFP_KERNEL);
	memset(wait_thread, 'A', 800);
	printk("wait_thread after allocation: %p\n", wait_thread);
	printk("wait_thread %c\n", wait_thread->__state);
	kfree(wait_thread);
	printk("wait_thread after free: %p\n", wait_thread);
	wait_thread = kthread_run(wait_thread_fn,NULL,"init_waitqueue_head wait_thread example");
	printk("str now: %p\n", wait_thread);
	#if 0
    if (IS_ERR(wait_thread))
        pr_info("error creating thread\n");
    else
        pr_info("kthread created successfully\n");
    wake_up_thread = kthread_run(wake_up_thread_fn,NULL,"init_waitqueue_head wake_up_thread example");
    if(IS_ERR(wake_up_thread))
        pr_info("error creating thread\n");
    else
        pr_info("kthread created successfully\n");
#endif
}

/* init_waitqueue_head_init - calls thread_start,
 * executes when module is loaded */

static int __init init_waitqueue_head_init(void)
{
	//pr_info("inside kthread init function\n");
	thread_start();
    return 0;
}

/* thread_stop - destroys thread and stops execution */

void thread_stop(void)
{
    kthread_stop(wake_up_thread);
    kthread_stop(wait_thread);
    pr_info("destroyed threads\n");
}

/* init_waitqueue_head_exit - calls thread_stop,
 * executes when module is unloaded */

static void __exit init_waitqueue_head_exit(void)
{
    pr_info("inside kthread exit function\n");
    thread_stop();
}

module_init(init_waitqueue_head_init);
module_exit(init_waitqueue_head_exit);

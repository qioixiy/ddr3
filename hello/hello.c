#include <linux/init.h>
#include <linux/module.h>
#include <asm/current.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
	printk(KERN_ALERT "hello init\n");
	printk(KERN_ALERT "the process is \"%s\", pid %i\n", current->comm);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "hello exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

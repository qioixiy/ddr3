#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <asm/current.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

static char *whom = "world";
static int howmany = 1;
module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

static void show_howmany_whom(void)
{
  int time = howmany;

  while(time--) {
    printk(KERN_ALERT "hello %s\n", whom);
  }
}

static int hello_init(void)
{
  printk(KERN_ALERT "hello init\n");
  printk(KERN_ALERT "the process is \"%s\", pid %i\n", current->comm);
  
  show_howmany_whom();
  return 0;
}

static void hello_exit(void)
{
  printk(KERN_ALERT "hello exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

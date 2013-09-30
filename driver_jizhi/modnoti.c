#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

static struct notifier_block *pnb = NULL;
static char *mstate[]={"LIVE", "COMMING", "GOING"};

//pnb callback function
int get_notify(struct notifier_block *p, unsigned long v, void *m)
{
  printk("module <%s> is %s, p->priority=%d\n", ((struct module *)m)->name, mstate[v], p->priority);

  return 0;
}

static int hello_init()
{
  //alloc struct notifier_block 
  pnb = kzalloc(sizeof(struct notifier_block), GFP_KERNEL);
  
  if (!pnb) {
    return -1;
  }

  pnb->notifier_call = get_notify;
  pnb->priority = 10;
  register_module_notifier(pnb);
  printk("A listening module is comming...\n");
  
  return 0;
}

static void hello_exit()
{
  unregister_module_notifier(pnb);
  kfree(pnb);
  
  printk("A listening module is going\n");
}

module_init(hello_init);
module_exit(hello_exit);


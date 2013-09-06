#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/pci.h>

MODULE_LICENSE("Dual BSD/GPL");

#define NUM_CMOS_BANKS 2

struct cmos_dev {
	unsigned short current_pointer;
	unsigned int size;
	int bank_number;

	struct cdev cdev;
	char name[10];
} *cmos_devp[NUM_CMOS_BANKS];

static struct file_operations cmos_fops = {
  .owner = THIS_MODULE,
  /*.open = cmos_open,
  .release = cmos_release,
  .read = cmos_read,
  .write = cmos_write,
  .llseek = cmos_llseek,
  .ioctl = cmos_ioctl,//*/
};

static dev_t cmos_dev_number;
struct class *cmos_class;

#define CMOS_BANK_SIZE (0XFF * 8)
#define DEVICE_NAME "cmos"

#define COMS_BANK0_INDEX_PORT 0X70
#define COMS_BANK0_DATA_PORT 0X71
#define COMS_BANK1_INDEX_PORT 0X72
#define COMS_BANK1_DATA_PORT 0X73

unsigned char addrports[NUM_CMOS_BANKS] = {
	COMS_BANK0_INDEX_PORT,
	COMS_BANK1_INDEX_PORT,
};

unsigned char dataports[NUM_CMOS_BANKS] = {
	COMS_BANK0_DATA_PORT,
	COMS_BANK1_DATA_PORT,
};

int __init cmos_init(void)
{
	int i, ret;

	printk("cmos_init");

	if (alloc_chrdev_region(&cmos_dev_number, 0, NUM_CMOS_BANKS, DEVICE_NAME) < 0) {
		printk(KERN_DEBUG "Can`t register device\n");
		return -1;
	}

	return 0;

	
	for (i = 0; i < NUM_CMOS_BANKS; i++) {
		cmos_devp[i] = kmalloc(sizeof(struct cmos_dev), GFP_KERNEL);
		if (!cmos_devp[i]) {
			printk("Bad Kmalloc\n");
			return -ENOMEM;
		}
		
		/*Request IO region*/
		sprintf(cmos_devp[i]->name, "cmos%d", i);
		if (!(request_region(addrports[i], 2, cmos_devp[i]->name))) {
			printk("cmos: IO port 0x%x is not free.\n", addrports[i]);
			return -EIO;
		}

		//fill int the bank number to correlate this device with the corresponding CMOS bank
		cmos_devp[i]->bank_number = i;

		//connect the major/minor number to the cdev
		cdev_init(&cmos_devp[i]->cdev, &cmos_fops);
		cmos_devp[i]->cdev.owner = THIS_MODULE;

		//connect the major number to the cdev
		ret = cdev_add(&cmos_devp[i]->cdev, (cmos_dev_number + i), 1);
		if (ret) {
			printk("Bad cdev\n");
			return ret;
		}

		//send uevent to udev, so it'll create /dev nodes
		device_create(cmos_class, NULL, MKDEV(MAJOR(cmos_dev_number), i),
						"cmos%d", i);
	}

	printk("CMOS Driver initialized.\n");
	return 0;
}

void __exit cmos_cleanup()
{
	int i;

	//release the major number
	unregister_chrdev_region((cmos_dev_number), NUM_CMOS_BANKS);

	//release IO region
	for(i = 0; i < NUM_CMOS_BANKS; i++) {
		device_destroy(cmos_class, MKDEV(MAJOR(cmos_dev_number), i));
		release_region(addrports[i], 2);
		cdev_del(&cmos_devp[i]->cdev);
		kfree(cmos_devp[i]);
	}

	//destroy cmos_class
	class_destroy(cmos_class);
	return ;
}

module_init(cmos_init);
module_exit(cmos_cleanup);


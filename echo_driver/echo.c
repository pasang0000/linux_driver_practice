#include <linux/module.h>

// module attributes
MODULE_LICENSE("GPL");  // this avoid kernel taint warning
MODULE_DESCRIPTION("Echo devier driver");
MODULE_AUTHOR("Sangho Park");

// called when module is loaded, similar to main()
int init_module(void)
{
    printk(KERN_INFO "INFO\n");
    return 0;
}

// called when module is unloaded, similar to destructor in OOP
// If cleanup_module does not exist, the module is inserted permanently.
void cleanup_module(void)
{
}

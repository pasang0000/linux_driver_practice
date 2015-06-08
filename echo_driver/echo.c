#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>        /* for put_user */
#include <linux/delay.h>        /* msleep */

// module attributes
MODULE_LICENSE("GPL");  // this avoid kernel taint warning
MODULE_DESCRIPTION("Echo devier driver");
MODULE_AUTHOR("Sangho Park");

#define DEV_MAJOR   250
#define DEV_NAME    "echo"

// file_operations
// If not, structure initialization may fail
static int __open(struct inode *, struct file *);
static ssize_t  __read(struct file *, char *, size_t, loff_t *);
static ssize_t  __write(struct file *, const char *, size_t, loff_t *);
static int __release_(struct inode *, struct file *);

struct echo_device {
    char    buf[1024];
    int     open;
    int     pos;
    int     end;
};
static struct echo_device   _dev;

// structure containing callbacks
static struct file_operations fops =
{
    .open = __open,
    .read = __read,
    .write = __write,
    .release = __release_,
};

// called when module is loaded, similar to main()
int init_module(void)
{
    int maj = register_chrdev(DEV_MAJOR, DEV_NAME, &fops);  // register driver with major:99

    if (maj < 0)
    {
        printk(KERN_ALERT "%s device registration failed.\n", DEV_NAME);
        return maj;
    }

    printk(KERN_INFO DEV_NAME " device registered.\n");
    printk(KERN_INFO "Major: %d\n", maj);

    memset(&_dev, 0, sizeof(_dev));

    return 0;
}

// called when module is unloaded, similar to destructor in OOP
// If cleanup_module does not exist, the module is inserted permanently.
void cleanup_module(void)
{
    unregister_chrdev(DEV_MAJOR, DEV_NAME);
}

// called when 'open' system call is called on the device file
static int __open(struct inode *inode, struct file *file)
{
    _dev.open++;
    printk(KERN_INFO DEV_NAME " opened %d times\n", _dev.open);
    return 0;
}

// called when 'read' system call is called on the file
static ssize_t  __read(struct file *file, char *buf, size_t len, loff_t *off)
{
    int count = 0;
    while (len && (_dev.buf[_dev.pos] != 0))
    {
        put_user(_dev.buf[_dev.pos], buf++);  // copy a byte from kernel space to user space
        count++;
        len--;
        _dev.pos++;
    }
    return count;
}

// called when 'write' system call is called on the file
static ssize_t  __write(struct file *file, const char *buf, size_t len, loff_t *off)
{
    int i;
    for (i = _dev.pos = 0; i < len; i++)
    {
        _dev.buf[i] = buf[i];
    }
    _dev.buf[i] = '\0';
    return i;
}

// called when 'close' system call is called on the file
static int __release_(struct inode *inode, struct file *file)
{
    printk(KERN_INFO DEV_NAME " device closed\n");
    return 0;
}


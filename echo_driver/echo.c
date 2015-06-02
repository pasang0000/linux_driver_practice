#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>        /* for put_user */
#include <linux/delay.h>        /* msleep */

// module attributes
MODULE_LICENSE("GPL");  // this avoid kernel taint warning
MODULE_DESCRIPTION("Echo devier driver");
MODULE_AUTHOR("Sangho Park");

#define DEV_MAJOR   0
#define DEV_NAME    "echo"

// file_operations
// If not, structure initialization may fail
static int __open(struct inode *, struct file *);
static ssize_t  __read(struct file *, char *, size_t, loff_t *);
static ssize_t  __write(struct file *, const char *, size_t, loff_t *);
static int __release_(struct inode *, struct file *);

static char     _buf[1024];
static int      _open = 0;
static int      _pos = 0;

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
    _open++;
    printk(KERN_INFO DEV_NAME " opened %d times\n", _open);
    return 0;
}

// called when 'read' system call is called on the file
static ssize_t  __read(struct file *file, char *buf, size_t len, loff_t *off)
{
    int count = 0;
    while (len && (_buf[_pos] != 0))
    {
        put_user(_buf[_pos], buf++);  // copy a byte from kernel space to user space
        count++;
        len--;
        _pos++;
    }
    return count;
}

// called when 'write' system call is called on the file
static ssize_t  __write(struct file *file, const char *buf, size_t len, loff_t *off)
{
    int i;
    for (i = _pos = 0; i < len; i++)
    {
        _buf[i] = buf[i];
    }
    _buf[i] = '\0';
    return i;
}

// called when 'close' system call is called on the file
static int __release_(struct inode *inode, struct file *file)
{
    printk(KERN_INFO DEV_NAME " device closed\n");
    return 0;
}


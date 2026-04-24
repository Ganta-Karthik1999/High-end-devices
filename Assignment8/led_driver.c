#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>

#define DEVICE_NAME "myleds"
#define CLASS_NAME  "myleds_class"

#define IO_LED 21
#define OFFSET 569

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;
static struct gpio_desc *led;

static int led_state = 0;

static int my_open(struct inode *inode, struct file *file)
{
    pr_info("myleds: device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("myleds: device closed\n");
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *user_buf,
                        size_t len, loff_t *off)
{
    char kbuf[32];

    if (len >= sizeof(kbuf))
        len = sizeof(kbuf) - 1;

    if (copy_from_user(kbuf, user_buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    if (strncmp(kbuf, "led=on", 6) == 0) {
        gpiod_set_value(led, 1);
        led_state = 1;
        pr_info("myleds: LED ON\n");
    } 
    else if (strncmp(kbuf, "led=off", 7) == 0) {
        gpiod_set_value(led, 0);
        led_state = 0;
        pr_info("myleds: LED OFF\n");
    } 
    else {
        pr_info("myleds: invalid command\n");
        return -EINVAL;
    }

    return len;
}

static ssize_t my_read(struct file *file, char __user *user_buf,
                       size_t len, loff_t *off)
{
    char status[16];
    int status_len;

    if (*off > 0)
        return 0;

    status_len = snprintf(status, sizeof(status),
                          "led=%s\n", led_state ? "on" : "off");

    if (copy_to_user(user_buf, status, status_len))
        return -EFAULT;

    *off += status_len;
    return status_len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

static int __init myleds_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&my_cdev, &fops);

    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0)
        goto unregister_dev;

    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        goto delete_cdev;
    }

    my_device = device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        ret = PTR_ERR(my_device);
        goto destroy_class;
    }

    led = gpio_to_desc(IO_LED + OFFSET);
    if (!led) {
        pr_err("myleds: failed to get GPIO\n");
        ret = -ENODEV;
        goto destroy_device;
    }

    ret = gpiod_direction_output(led, 0);
    if (ret) {
        pr_err("myleds: failed to set GPIO output\n");
        goto destroy_device;
    }

    pr_info("myleds: driver loaded, device created at /dev/myleds\n");
    return 0;

destroy_device:
    device_destroy(my_class, dev_num);
destroy_class:
    class_destroy(my_class);
delete_cdev:
    cdev_del(&my_cdev);
unregister_dev:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static void __exit myleds_exit(void)
{
    gpiod_set_value(led, 0);
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("myleds: driver unloaded\n");
}

module_init(myleds_init);
module_exit(myleds_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Assignment 8 LED Character Device Driver");

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Project driver: buttons speed + PWM LEDs using hrtimer");

#define DEVICE_NAME "project"
#define CLASS_NAME  "project_class"

/*
 * CHANGE THESE GPIO NUMBERS ACCORDING TO YOUR WIRING.
 * These are BCM GPIO numbers.
 */
#define GPIO_LED1   17+569
#define GPIO_LED2   27+569
#define GPIO_BTN1   22+569
#define GPIO_BTN2   23+569

#define PWM_PERIOD_NS      10000000L   /* 10 ms */
#define SPEED_WINDOW_SEC   10

static dev_t dev_num;
static struct cdev project_cdev;
static struct class *project_class;
static struct device *project_device;

static struct gpio_desc *led1;
static struct gpio_desc *led2;
static struct gpio_desc *btn1;
static struct gpio_desc *btn2;

static int irq_btn1;
static int irq_btn2;

static struct hrtimer pwm_timer;
static struct hrtimer speed_timer;

static int pwm_state = 0;

static int duty_l1 = 10;
static int duty_l2 = 0;

static int press_count = 0;
static int speed = 0;
static int expected_button = 1;

static DEFINE_MUTEX(project_lock);

static enum hrtimer_restart pwm_callback(struct hrtimer *timer)
{
    ktime_t now;
    long on_time_l1;
    long on_time_l2;
    long next_time;

    mutex_lock(&project_lock);

    if (pwm_state == 0) {
        gpiod_set_value(led1, duty_l1 > 0 ? 1 : 0);
        gpiod_set_value(led2, duty_l2 > 0 ? 1 : 0);

        on_time_l1 = (PWM_PERIOD_NS * duty_l1) / 100;
        on_time_l2 = (PWM_PERIOD_NS * duty_l2) / 100;

        next_time = on_time_l1;

        if (on_time_l2 > next_time)
            next_time = on_time_l2;

        if (next_time <= 0)
            next_time = PWM_PERIOD_NS;

        pwm_state = 1;
    } else {
        gpiod_set_value(led1, 0);
        gpiod_set_value(led2, 0);

        next_time = PWM_PERIOD_NS;
        pwm_state = 0;
    }

    mutex_unlock(&project_lock);

    now = ktime_get();
    hrtimer_forward(timer, now, ns_to_ktime(next_time));

    return HRTIMER_RESTART;
}

static enum hrtimer_restart speed_callback(struct hrtimer *timer)
{
    mutex_lock(&project_lock);

    speed = press_count;
    press_count = 0;

    mutex_unlock(&project_lock);

    hrtimer_forward_now(timer, ktime_set(SPEED_WINDOW_SEC, 0));
    return HRTIMER_RESTART;
}

static irqreturn_t button_irq_handler(int irq, void *dev_id)
{
    int button_id = *(int *)dev_id;

    mutex_lock(&project_lock);

    if (button_id == expected_button) {
        press_count++;

        if (expected_button == 1)
            expected_button = 2;
        else
            expected_button = 1;
    }

    mutex_unlock(&project_lock);

    return IRQ_HANDLED;
}

static int btn1_id = 1;
static int btn2_id = 2;

static ssize_t project_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    char kbuf[64];
    int ret;
    int current_speed;

    if (*offset > 0)
        return 0;

    mutex_lock(&project_lock);
    current_speed = speed;
    mutex_unlock(&project_lock);

    ret = snprintf(kbuf, sizeof(kbuf), "speed=%d\n", current_speed);

    if (copy_to_user(buf, kbuf, ret))
        return -EFAULT;

    *offset = ret;
    return ret;
}

static ssize_t project_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    char kbuf[64];
    int l1;
    int l2;

    if (len >= sizeof(kbuf))
        return -EINVAL;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    if (sscanf(kbuf, "L1=%d L2=%d", &l1, &l2) == 2) {
        if (l1 < 0) l1 = 0;
        if (l1 > 100) l1 = 100;

        if (l2 < 0) l2 = 0;
        if (l2 > 100) l2 = 100;

        mutex_lock(&project_lock);
        duty_l1 = l1;
        duty_l2 = l2;
        mutex_unlock(&project_lock);

        pr_info("project: LED duty updated: L1=%d L2=%d\n", l1, l2);
    } else {
        pr_info("project: invalid format. Use: L1=50 L2=20\n");
        return -EINVAL;
    }

    return len;
}

static int project_open(struct inode *inode, struct file *file)
{
    pr_info("project: device opened\n");
    return 0;
}

static int project_release(struct inode *inode, struct file *file)
{
    pr_info("project: device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = project_open,
    .release = project_release,
    .read = project_read,
    .write = project_write,
};

static int __init project_init(void)
{
    int ret;

    pr_info("project: loading module\n");

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&project_cdev, &fops);

    ret = cdev_add(&project_cdev, dev_num, 1);
    if (ret < 0)
        goto unregister_chrdev;

    project_class = class_create(CLASS_NAME);
    if (IS_ERR(project_class)) {
        ret = PTR_ERR(project_class);
        goto delete_cdev;
    }

    project_device = device_create(project_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(project_device)) {
        ret = PTR_ERR(project_device);
        goto destroy_class;
    }

    led1 = gpio_to_desc(GPIO_LED1);
    led2 = gpio_to_desc(GPIO_LED2);
    btn1 = gpio_to_desc(GPIO_BTN1);
    btn2 = gpio_to_desc(GPIO_BTN2);

    if (!led1 || !led2 || !btn1 || !btn2) {
        pr_err("project: failed to get GPIO descriptors\n");
        ret = -ENODEV;
        goto destroy_device;
    }

    ret = gpiod_direction_output(led1, 0);
    if (ret)
        goto destroy_device;

    ret = gpiod_direction_output(led2, 0);
    if (ret)
        goto destroy_device;

    ret = gpiod_direction_input(btn1);
    if (ret)
        goto destroy_device;

    ret = gpiod_direction_input(btn2);
    if (ret)
        goto destroy_device;

    irq_btn1 = gpiod_to_irq(btn1);
    irq_btn2 = gpiod_to_irq(btn2);

    if (irq_btn1 < 0 || irq_btn2 < 0) {
        pr_err("project: failed to get IRQ numbers\n");
        ret = -EINVAL;
        goto destroy_device;
    }

    ret = request_irq(irq_btn1, button_irq_handler,
                      IRQF_TRIGGER_RISING,
                      "project_btn1_irq", &btn1_id);
    if (ret) {
        pr_err("project: failed to request BTN1 IRQ\n");
        goto destroy_device;
    }

    ret = request_irq(irq_btn2, button_irq_handler,
                      IRQF_TRIGGER_RISING,
                      "project_btn2_irq", &btn2_id);
    if (ret) {
        pr_err("project: failed to request BTN2 IRQ\n");
        goto free_btn1_irq;

    }

    hrtimer_init(&pwm_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    pwm_timer.function = pwm_callback;
    hrtimer_start(&pwm_timer, ns_to_ktime(PWM_PERIOD_NS), HRTIMER_MODE_REL);

    hrtimer_init(&speed_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    speed_timer.function = speed_callback;
    hrtimer_start(&speed_timer, ktime_set(SPEED_WINDOW_SEC, 0), HRTIMER_MODE_REL);

    pr_info("project: module loaded successfully\n");
    pr_info("project: device created at /dev/project\n");

    return 0;

free_btn1_irq:
    free_irq(irq_btn1, &btn1_id);

destroy_device:
    device_destroy(project_class, dev_num);

destroy_class:
    class_destroy(project_class);

delete_cdev:
    cdev_del(&project_cdev);

unregister_chrdev:
    unregister_chrdev_region(dev_num, 1);

    return ret;
}

static void __exit project_exit(void)
{
    hrtimer_cancel(&pwm_timer);
    hrtimer_cancel(&speed_timer);

    free_irq(irq_btn1, &btn1_id);
    free_irq(irq_btn2, &btn2_id);

    gpiod_set_value(led1, 0);
    gpiod_set_value(led2, 0);

    device_destroy(project_class, dev_num);
    // class_destroy(project_class);
    cdev_del(&project_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("project: module unloaded\n");
}

module_init(project_init);
module_exit(project_exit);
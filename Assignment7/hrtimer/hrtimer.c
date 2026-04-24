#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/gpio/consumer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Simple High Resolution Timer Example");

#define TIMEOUT_SEC  1
#define TIMEOUT_NSEC 100000000L   /* 0.1 sec */
#define IO_LED 21
#define OFFSET 569

static struct gpio_desc *led;
static struct hrtimer my_hrtimer;
static unsigned int count = 0;

static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
    pr_info("HRTimer fired: %u\n", count++);
    static bool on;
    on = !on;
    gpiod_set_value(led, on);
    hrtimer_forward_now(timer, ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC));
    return HRTIMER_RESTART;   /* periodic timer */
}

static int __init my_hrtimer_init(void)
{
    int status;

    ktime_t ktime = ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC);

    led = gpio_to_desc(IO_LED + OFFSET);
    if(!led){
        printk("gpiocntrl-Error getting pin 4 !!!");
        return -ENODEV;
    }

    status = gpiod_direction_output(led,0);
    if(status){
        printk("gpio- error setting the Led to output !!!");
        return status;
    }

    pr_info("HRTimer module loaded\n");
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_hrtimer.function = timer_callback;
    hrtimer_start(&my_hrtimer, ktime, HRTIMER_MODE_REL);


    return 0;
}

static void __exit my_hrtimer_exit(void)
{
    hrtimer_cancel(&my_hrtimer);
    gpiod_set_value(led,0);
    pr_info("HRTimer module unloaded\n");
}

module_init(my_hrtimer_init);
module_exit(my_hrtimer_exit);
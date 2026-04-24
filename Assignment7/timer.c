#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/gpio/consumer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("Blink LED every 1 second using kernel timer");

static struct timer_list my_timer;
static struct gpio_desc *led;

#define IO_LED 21
#define OFFSET 569

static int led_state = 0;

static void timer_callback(struct timer_list *t)
{
    led_state = !led_state;
    gpiod_set_value(led, led_state);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
    pr_info("Timer callback executed, LED state: %d\n", led_state);
}

static int __init ModuleInit(void)
{
    pr_info("Hello, Kernel!\n");

    led = gpio_to_desc(IO_LED + OFFSET);
    if (!led) {
        pr_err("Error getting GPIO pin\n");
        return -ENODEV;
    }

    if (gpiod_direction_output(led, 0)) {
        pr_err("Cannot set GPIO to output\n");
        return -EINVAL;
    }

    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

    return 0;
}

static void __exit ModuleExit(void)
{
    del_timer_sync(&my_timer);
    gpiod_set_value(led, 0);
    pr_info("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
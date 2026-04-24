

#include <linux/module.h>
// #include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>

#define IO_LED 21
#define Button 20
#define OFFSET 569

static struct gpio_desc *led ,*button;

static int __init my_init(void)
{
    int status;
    led = gpio_to_desc(IO_LED + OFFSET);
    if(!led){
        printk("gpiocntrl-Error getting pin 21 !!!");
        return -ENODEV;
    }
    button = gpio_to_desc(Button + OFFSET);
    if(!button){
        printk("giocntrl-error getting the pin 20 !!");
        return -ENODEV;
    }
    status = gpiod_direction_output(led,0);
    if(status){
        printk("gpio- error setting the Led to output !!!");
        return status;
    }
    status = gpiod_direction_input(button);
    if(status){
        printk("gpio- error setting the button in the input mode !!!");
        return status;
    }
    gpiod_set_value(led,1);


    return 0;
}

static void __exit my_exit(void)
{
    // printk("hello: exit\n");
    gpiod_set_value(led,0);
    printk("LED OFF !!");
}
module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Simple hello kernel module");


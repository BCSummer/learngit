#include <linux/gfp.h>
#include "led_operation.h"

static int board_demo_led_init(int which)
{
    printk("%s %s line %d, ddevice:%s %d\n",__FILE__,__FUNCTION__,__LINE__,which ? "beep":"led",which);
    return 0;
}


static int board_demo_led_ctl(int which,char status)
{
    printk("%s %s line %d, ddevice:%s %d, %s\n",__FILE__,__FUNCTION__,__LINE__, which ? "beep":"led",which, status ? "on" : "off");
    return 0;
}
/*返回状态*/
static int board_demo_led_check(int which)
{
    printk("%s %s line %d, ddevice:%s %d\n",__FILE__,__FUNCTION__,__LINE__, which ? "beep":"led",which);
    return 1;
}

static struct led_operations board_demo_led_operation = {
    .init = board_demo_led_init,
    .ctl =  board_demo_led_ctl,
    .check = board_demo_led_check,
};

struct led_operations *get_board_led_operation(void)
{
    return &board_demo_led_operation;
}
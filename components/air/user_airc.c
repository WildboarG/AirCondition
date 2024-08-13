/**
 * @file wifi6_led.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "user_airc.h"
#include "log.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include "bflb_pwm_v2.h"      //pwm_v2头文件
#include "bflb_clock.h"           //系统时钟头文件
#include "user_state.h"
#define DBG_TAG "LED"


struct bflb_device_s* gpio;
struct bflb_device_s*  timer0;
struct bflb_device_s *pwm;     //创建LHAL外设库结构体，名称为pwm

static xTaskHandle configNet_task;
static xTaskHandle configNet_stop;

bool ir_pin_status = false;

uint16_t rawData[15][139]={  //temp code
    {9024, 4436,  692, 1602,  694, 532,  668, 508,  694, 1604,  692, 1628,  668, 510,  690, 510,  692, 510,  692, 510,  692, 508,  694, 508,  692, 532,  670, 508,  692, 510,  690, 508,  694, 508,  692, 512,  690, 508,  694, 532,  670, 508,  692, 512,  690, 1604,  694, 510,  692, 532,  668, 512,  690, 510,  692, 510,  692, 508,  694, 1604,  692, 508,  692, 1606,  690, 510,  692, 510,  692, 1604,  692, 510,  690, 19928,  690, 510,  692, 508,  692, 532,  668, 508,  694, 508,  694, 510,  692, 510,  692, 508,  692, 510,  692, 510,  692, 510,  690, 510,  692, 508,  692, 1606,  690, 510,  692, 506,  694, 510,  692, 510,  692, 532,  668, 510,  690, 534,  666, 512,  688, 510,  692, 510,  690, 510,  690, 512,  690, 510,  690, 510,  692, 1606,  690, 510,  690, 1608,  690, 510,  692},//16
    {9022, 4436,  690, 1606,  692, 508,  692, 508,  692, 1604,  692, 1604,  692, 508,  692, 510,  690, 508,  692, 1604,  692, 512,  690, 508,  692, 510,  690, 510,  692, 532,  668, 508,  692, 532,  668, 508,  692, 512,  688, 510,  692, 510,  690, 510,  692, 1604,  692, 508,  692, 510,  692, 510,  692, 508,  692, 508,  692, 510,  692, 1604,  692, 510,  692, 1604,  690, 512,  690, 510,  690, 1628,  668, 510,  692, 19926,  692, 510,  692, 508,  694, 510,  690, 508,  692, 510,  690, 508,  692, 512,  688, 508,  692, 532,  668, 510,  692, 510,  692, 510,  692, 508,  694, 1604,  690, 532,  668, 510,  692, 510,  692, 510,  690, 510,  690, 510,  692, 532,  668, 510,  692, 510,  690, 510,  690, 510,  692, 510,  692, 510,  690, 512,  690, 510,  692, 1604,  692, 1604,  692, 512,  690},//17
    {9026, 4434,  692, 1604,  692, 510,  692, 508,  692, 1606,  692, 1604,  692, 508,  692, 508,  692, 508,  694, 508,  692, 1604,  692, 510,  690, 512,  690, 510,  692, 510,  690, 508,  692, 510,  690, 510,  692, 510,  690, 510,  692, 510,  690, 510,  692, 1604,  692, 510,  690, 508,  694, 532,  668, 510,  692, 510,  690, 510,  692, 1604,  692, 512,  688, 1606,  690, 510,  690, 508,  692, 1606,  690, 532,  668, 19930,  690, 510,  692, 508,  692, 510,  690, 510,  690, 510,  692, 510,  690, 510,  692, 508,  692, 510,  692, 510,  690, 532,  670, 510,  692, 508,  692, 1628,  668, 510,  690, 510,  692, 510,  690, 508,  692, 510,  692, 510,  690, 510,  692, 508,  692, 510,  692, 510,  690, 510,  692, 508,  692, 510,  692, 508,  692, 1606,  690, 1604,  692, 1606,  690, 510,  692},//18
    {9026, 4436,  692, 1606,  690, 512,  688, 510,  690, 1606,  692, 1606,  690, 510,  692, 510,  690, 510,  690, 1604,  692, 1604,  692, 510,  692, 508,  692, 510,  690, 510,  692, 510,  692, 510,  692, 510,  690, 510,  690, 508,  694, 532,  668, 510,  692, 1604,  692, 510,  692, 510,  690, 510,  692, 532,  668, 510,  692, 510,  692, 1606,  690, 508,  692, 1628,  668, 510,  690, 510,  692, 1604,  692, 510,  692, 19926,  692, 510,  692, 508,  692, 510,  692, 510,  692, 532,  668, 512,  690, 532,  668, 510,  692, 510,  692, 512,  690, 510,  690, 510,  692, 508,  692, 1604,  692, 512,  690, 534,  668, 510,  692, 508,  692, 532,  668, 510,  692, 508,  692, 510,  692, 508,  692, 510,  692, 510,  690, 510,  692, 510,  690, 512,  688, 510,  692, 510,  692, 510,  690, 1604,  692},//19
    {9024, 4434,  692, 1604,  692, 512,  690, 510,  692, 1604,  692, 1604,  692, 508,  692, 510,  692, 508,  694, 508,  692, 508,  692, 1606,  690, 508,  692, 510,  690, 510,  692, 508,  692, 510,  690, 510,  690, 508,  692, 532,  668, 510,  690, 510,  690, 1604,  692, 510,  692, 510,  692, 508,  692, 512,  690, 510,  690, 512,  690, 1606,  690, 512,  690, 1606,  690, 510,  690, 510,  692, 1606,  690, 510,  690, 19928,  692, 510,  690, 508,  694, 508,  692, 508,  694, 510,  690, 510,  692, 508,  692, 510,  690, 508,  692, 508,  692, 508,  692, 510,  690, 534,  668, 1604,  690, 510,  692, 510,  690, 534,  668, 510,  692, 508,  692, 510,  690, 508,  692, 510,  690, 510,  692, 510,  690, 510,  690, 534,  668, 508,  692, 510,  690, 1606,  690, 510,  692, 508,  692, 1604,  690},//20
    {9024, 4436,  690, 1604,  692, 510,  692, 508,  692, 1606,  690, 1604,  692, 508,  692, 510,  690, 510,  692, 1604,  692, 508,  692, 1628,  668, 510,  692, 510,  690, 510,  690, 510,  692, 510,  692, 508,  692, 510,  692, 512,  690, 532,  668, 510,  690, 1606,  692, 508,  692, 510,  690, 512,  690, 508,  692, 508,  692, 510,  690, 1604,  692, 508,  692, 1606,  690, 510,  692, 510,  690, 1604,  692, 532,  668, 19928,  692, 508,  692, 508,  692, 510,  692, 510,  690, 510,  692, 508,  692, 510,  692, 510,  690, 510,  690, 510,  694, 508,  692, 508,  692, 510,  690, 1606,  690, 508,  694, 508,  692, 508,  694, 508,  692, 512,  690, 510,  690, 510,  692, 510,  690, 510,  692, 510,  692, 510,  690, 510,  690, 510,  690, 510,  690, 512,  690, 1606,  690, 510,  690, 1604,  692},//21
    {9024, 4436,  690, 1628,  668, 510,  692, 510,  690, 1604,  692, 1606,  690, 510,  692, 508,  692, 508,  694, 508,  694, 1602,  692, 1606,  692, 510,  692, 508,  692, 510,  692, 510,  692, 508,  692, 510,  690, 510,  690, 510,  690, 510,  692, 510,  690, 1606,  692, 508,  692, 508,  692, 508,  692, 510,  692, 510,  690, 510,  692, 1604,  692, 510,  692, 1604,  692, 510,  692, 510,  692, 1604,  692, 510,  692, 19928,  690, 508,  692, 508,  692, 510,  690, 510,  692, 508,  692, 510,  692, 510,  692, 510,  690, 508,  694, 508,  694, 508,  692, 510,  690, 510,  690, 1628,  668, 510,  692, 510,  692, 510,  690, 508,  692, 508,  692, 510,  690, 510,  690, 532,  668, 510,  690, 508,  692, 512,  690, 510,  692, 510,  692, 508,  692, 1606,  690, 1604,  692, 508,  692, 1606,  690},//22
    {9024, 4436,  692, 1604,  692, 508,  694, 508,  692, 1604,  692, 1606,  690, 510,  690, 510,  692, 508,  692, 1604,  692, 1604,  692, 1604,  692, 532,  668, 532,  668, 510,  692, 532,  670, 508,  692, 510,  692, 510,  692, 508,  692, 510,  692, 508,  692, 1628,  670, 508,  692, 512,  690, 510,  690, 510,  690, 510,  690, 510,  692, 1628,  668, 510,  690, 1606,  690, 510,  692, 510,  690, 1604,  692, 510,  690, 19950,  668, 508,  692, 510,  690, 508,  692, 508,  692, 508,  692, 510,  692, 510,  692, 510,  692, 512,  690, 510,  690, 510,  692, 532,  668, 510,  690, 1604,  692, 512,  690, 510,  692, 508,  692, 508,  692, 510,  692, 510,  690, 508,  692, 510,  690, 510,  690, 510,  692, 508,  692, 510,  690, 512,  690, 510,  692, 510,  692, 510,  690, 1604,  692, 1628,  668},//23
    {9026, 4434,  694, 1604,  692, 510,  692, 508,  692, 1606,  692, 1604,  692, 510,  690, 512,  688, 510,  692, 510,  690, 510,  692, 508,  692, 1606,  690, 510,  690, 532,  668, 508,  692, 510,  690, 510,  692, 532,  668, 510,  690, 510,  690, 512,  690, 1606,  692, 510,  690, 510,  690, 510,  692, 510,  692, 512,  690, 510,  692, 1604,  692, 510,  690, 1628,  668, 532,  668, 532,  668, 1604,  692, 510,  690, 19928,  692, 510,  692, 510,  690, 510,  692, 508,  692, 510,  690, 512,  690, 510,  690, 508,  692, 510,  692, 532,  668, 510,  690, 508,  692, 510,  690, 1602,  694, 532,  668, 510,  692, 510,  690, 510,  692, 510,  690, 510,  692, 512,  690, 512,  690, 510,  690, 510,  692, 510,  690, 508,  692, 512,  690, 510,  692, 1604,  692, 510,  692, 1604,  692, 1606,  690},//24
    {9026, 4434,  692, 1604,  692, 510,  690, 510,  690, 1606,  690, 1606,  690, 512,  690, 510,  692, 510,  692, 1604,  692, 510,  690, 510,  690, 1628,  668, 510,  692, 510,  692, 510,  692, 510,  690, 534,  668, 510,  690, 512,  690, 510,  690, 508,  692, 1604,  692, 510,  690, 510,  692, 512,  690, 510,  690, 510,  690, 510,  692, 1604,  690, 510,  692, 1606,  690, 510,  690, 510,  692, 1606,  690, 510,  690, 19928,  690, 510,  692, 508,  692, 510,  690, 510,  690, 510,  692, 510,  690, 512,  690, 508,  692, 510,  692, 510,  690, 512,  690, 510,  690, 508,  692, 1606,  690, 510,  692, 510,  692, 510,  690, 510,  692, 510,  690, 510,  692, 510,  690, 510,  692, 510,  692, 508,  692, 510,  690, 510,  692, 510,  690, 510,  692, 508,  692, 1606,  690, 1604,  692, 1604,  692},//25
    {9024, 4434,  694, 1604,  692, 510,  692, 510,  690, 1602,  694, 1606,  690, 532,  668, 510,  690, 532,  668, 508,  692, 1604,  690, 510,  692, 1604,  692, 510,  690, 510,  692, 510,  690, 510,  690, 508,  692, 510,  690, 510,  692, 512,  690, 510,  690, 1604,  692, 512,  690, 510,  690, 508,  692, 510,  692, 510,  690, 510,  692, 1606,  690, 510,  692, 1604,  692, 512,  688, 510,  690, 1604,  692, 510,  690, 19926,  692, 508,  692, 510,  692, 508,  692, 510,  692, 510,  692, 510,  690, 510,  692, 510,  692, 510,  692, 510,  690, 512,  690, 506,  694, 508,  692, 1604,  692, 510,  690, 532,  668, 510,  692, 508,  692, 510,  690, 532,  668, 510,  692, 510,  692, 532,  668, 510,  692, 510,  690, 510,  692, 532,  668, 532,  668, 1604,  692, 1604,  692, 1604,  694, 1604,  692},//26
    {9000, 4460,  668, 1606,  690, 510,  692, 512,  688, 1628,  668, 1602,  692, 512,  690, 510,  690, 510,  692, 1602,  694, 1628,  668, 508,  694, 1604,  692, 508,  694, 508,  694, 510,  690, 508,  692, 508,  692, 510,  692, 510,  690, 510,  692, 510,  690, 1608,  688, 512,  690, 510,  692, 510,  692, 510,  692, 510,  690, 510,  692, 1604,  692, 510,  690, 1604,  692, 512,  690, 510,  690, 1604,  692, 510,  692, 19930,  690, 510,  692, 508,  692, 510,  692, 508,  692, 510,  692, 508,  692, 508,  692, 510,  692, 510,  690, 510,  692, 508,  692, 508,  692, 512,  688, 1628,  668, 510,  690, 532,  668, 534,  668, 510,  692, 510,  690, 510,  692, 508,  692, 510,  690, 512,  692, 512,  690, 508,  692, 510,  690, 532,  668, 510,  692, 510,  690, 510,  692, 510,  690, 512,  690},//27
    {9022, 4436,  690, 1606,  690, 532,  668, 508,  692, 1604,  692, 1604,  692, 508,  692, 512,  690, 512,  690, 510,  692, 508,  692, 1606,  690, 1628,  668, 510,  692, 510,  692, 510,  692, 508,  692, 510,  692, 508,  692, 508,  692, 510,  692, 510,  692, 1604,  692, 510,  692, 510,  690, 510,  690, 510,  692, 510,  690, 534,  668, 1606,  690, 510,  690, 1628,  668, 510,  692, 510,  690, 1604,  694, 510,  690, 19928,  690, 510,  690, 510,  690, 510,  692, 510,  692, 506,  692, 512,  690, 510,  690, 510,  692, 532,  668, 508,  694, 510,  690, 510,  690, 510,  692, 1606,  692, 510,  690, 508,  692, 510,  690, 510,  692, 510,  690, 510,  692, 510,  692, 510,  692, 508,  692, 508,  692, 510,  692, 510,  692, 510,  692, 510,  690, 1604,  692, 510,  690, 512,  690, 534,  666},//28
    {9000, 4434,  694, 1626,  670, 510,  690, 510,  692, 1602,  694, 1604,  690, 510,  692, 510,  690, 510,  692, 1604,  692, 508,  692, 1604,  692, 1628,  668, 512,  690, 510,  692, 510,  690, 532,  670, 508,  692, 510,  692, 510,  692, 508,  692, 510,  692, 1604,  692, 510,  692, 508,  692, 510,  690, 512,  690, 510,  690, 510,  690, 1628,  668, 508,  692, 1604,  692, 510,  690, 510,  690, 1604,  692, 512,  690, 19926,  692, 510,  690, 510,  690, 508,  692, 510,  692, 508,  692, 508,  692, 510,  690, 512,  690, 508,  692, 508,  694, 510,  692, 508,  692, 510,  690, 1606,  690, 534,  668, 510,  692, 510,  692, 510,  690, 510,  690, 510,  692, 510,  690, 512,  690, 510,  690, 510,  690, 510,  690, 508,  692, 508,  692, 510,  692, 510,  690, 1604,  690, 510,  692, 508,  692},//29
    {9024, 4458,  668, 1606,  692, 510,  690, 510,  692, 1604,  692, 1606,  688, 510,  692, 510,  690, 512,  690, 510,  690, 1604,  692, 1604,  692, 1606,  690, 510,  692, 510,  692, 510,  690, 510,  692, 510,  692, 508,  692, 510,  692, 510,  692, 510,  692, 1604,  692, 510,  692, 508,  692, 510,  692, 510,  692, 510,  692, 510,  692, 1604,  690, 510,  692, 1604,  692, 510,  692, 510,  690, 1604,  692, 510,  690, 19928,  692, 510,  690, 508,  692, 510,  690, 510,  692, 508,  692, 510,  692, 512,  690, 508,  692, 506,  694, 510,  690, 510,  690, 510,  690, 512,  688, 1606,  690, 508,  692, 510,  692, 512,  690, 532,  668, 510,  690, 510,  692, 508,  692, 510,  692, 512,  690, 510,  690, 510,  690, 510,  692, 510,  692, 512,  690, 1606,  690, 1604,  692, 532,  668, 510,  690}//30          
};

uint16_t off[139]= {9024, 4436,  688, 1606,  690, 534,  666, 512,  690, 512,  688, 1608,  690, 510,  690, 512,  690, 510,  690, 512,  688, 512,  690, 510,  690, 1608,  690, 512,  688, 512,  690, 512,  688, 510,  690, 512,  688, 514,  688, 510,  692, 512,  686, 512,  688, 1608,  688, 510,  692, 512,  688, 514,  688, 512,  688, 510,  690, 514,  688, 1606,  690, 512,  688, 1608,  688, 514,  688, 534,  668, 1608,  688, 512,  688, 19950,  666, 512,  688, 510,  690, 512,  688, 512,  688, 512,  690, 510,  690, 512,  688, 512,  688, 534,  668, 512,  688, 510,  690, 512,  690, 512,  688, 1606,  690, 510,  690, 512,  688, 512,  690, 514,  688, 512,  688, 512,  688, 534,  668, 512,  688, 512,  690, 514,  686, 512,  690, 512,  690, 512,  688, 512,  690, 1606,  688, 514,  686, 1608,  690, 512,  688};

// void timer0_isr(int irq, void *arg)//timer0中端服务程序
// {
//     bool status = bflb_timer_get_compint_status(timer0, TIMER_COMP_ID_0);
//     if (status) {
//         bflb_timer_compint_clear(timer0, TIMER_COMP_ID_0);
//         ir_pin_status ? bflb_gpio_set(gpio,IR) : bflb_gpio_reset(gpio,IR);
//         ir_pin_status=!ir_pin_status;
//     }
// }

/**
 * @brief  configNET_start
 *
 * @param arg
*/
static void configNET_start(void* arg) // 
{
    while (1) {
        bflb_gpio_set(gpio, LED_CONFIG_NET);
        vTaskDelay(pdMS_TO_TICKS(200));
        bflb_gpio_reset(gpio, LED_CONFIG_NET);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
/**
 * @brief
 *
*/

void my_pwm_gpio_init()        //编写一个选择pwm输出的gpio口初始化函数
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
//选择IO0作为pwm输出，
}

void ac_init(void)
{
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, LED_CONFIG_NET, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, LED_DEV, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IR, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0); //IR 
    bflb_gpio_reset(gpio, LED_CONFIG_NET);
    bflb_gpio_reset(gpio, LED_DEV);
    bflb_gpio_reset(gpio, IR);  

      my_pwm_gpio_init();         //调用函数，里面设置好了pwm输出的gpio口

    pwm = bflb_device_get_by_name("pwm_v2_0");  //给外设接口赋名pwm_v2_0

    /* period = .XCLK / .clk_div / .period = 40MHz / 40 / 1000 = 1KHz */

    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_XCLK,
        .clk_div = 7,
        .period = 150,
    };                //设置PWM的频率，选择时钟，分频，和周期。根据上面的公式算出最终的频率。

    /*初始化PWM输出*/
    bflb_pwm_v2_init(pwm, &cfg);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH0,75, 150); //改变占空比，变量i会不断变化

 

    xTaskCreate(configNET_start, "configNET_task", 1024, NULL, 2, &configNet_task);
    vTaskSuspend(configNet_task);
}
/**
 * @brief
 *
*/
static void led_configNET_start(void)
{
    if (configNet_task==NULL) {
        LOG_E("config NET task not created");
        return;
    }
    vTaskResume(configNet_task);
}

static void configNET_stop(void* arg)
{
    bflb_gpio_reset(gpio, LED_CONFIG_NET);
    bflb_gpio_set(gpio, LED_CONFIG_NET);
    vTaskDelay(pdMS_TO_TICKS(50));
    bflb_gpio_reset(gpio, LED_CONFIG_NET);
    vTaskDelay(pdMS_TO_TICKS(50));
    bflb_gpio_set(gpio, LED_CONFIG_NET);
    vTaskDelay(pdMS_TO_TICKS(50));
    bflb_gpio_reset(gpio, LED_CONFIG_NET);
    vTaskDelete(NULL);
}


/**
 * @brief
 *
*/
static void led_configNET_stop(void)
{
    if (configNet_task==NULL) {
        LOG_E("config NET task not created");
        return;
    }
    vTaskSuspend(configNet_task);
    xTaskCreate(configNET_stop, "configNET_task", 1024, NULL, 4, &configNet_stop);
}

static void configNET_server_CNT(void* arg) //mqtt连接成功
{
    bflb_gpio_reset(gpio, LED_CONFIG_NET);
    bflb_gpio_set(gpio, LED_CONFIG_NET);
    vTaskDelay(pdMS_TO_TICKS(2000));
    bflb_gpio_reset(gpio, LED_CONFIG_NET);
    vTaskDelete(NULL);
}

void led_indicator_change(int led_state)
{
    user_state_t state = led_state;
    switch (state)
    {
        case STATE_WIFI_CONNECTING:
            /* code */
            led_configNET_start();
            break;
        case STATE_WIFI_CONNECTED_OK:
            led_configNET_stop();
            break;
        case STATE_MQTT_SERVER_CONENCT_OK:
        {
            //创建任务，常亮2S 灯
            xTaskCreate(configNET_server_CNT, "server led", 1024, NULL, 4, NULL);
        }
        break;
        default:
            break;
    }
}

void led_dev_open(int* led_state)
{
    bflb_gpio_set(gpio, LED_DEV);
    *led_state = true;
}

void led_dev_cloes(int* led_state)
{
    bflb_gpio_reset(gpio, LED_DEV);
    *led_state = false;
}

void ctlAC(int status){
    printf("temp : %d\r\n",status);
    Send_IR(rawData[status - 16],139);
}


void Send_IR(uint16_t *buf,uint8_t len)
{
   // printf("Sending IR code...\n");
    for (uint16_t i = 0; i < len; i++) {
        if (i%2==0) {
           bflb_pwm_v2_start(pwm);          //将设置好的频率开启pwm输出
            bflb_mtimer_delay_us(buf[i]); // 高电平（脉冲时间）
            bflb_pwm_v2_stop(pwm);          //将设置好的频率关闭pwm输出

        } else {
            //bflb_gpio_reset(gpio, IR);
            bflb_mtimer_delay_us(buf[i]); // 低电平（空闲时间）
        }
    }   
}

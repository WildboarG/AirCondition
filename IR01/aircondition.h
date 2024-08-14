#ifndef AIRCONDITION_H
#define AIRCONDITION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mqtt_client.h>
#include <hosal_timer.h>
#include <bl_gpio.h>
#include "bl_timer.h"
#include "bl_irq.h"
#include <bl602.h>
#include <bl602_gpio.h>
#include <bl602_timer.h>
#include <bl602_glb.h>
#include <bl_pwm.h>
#include "blog.h"
#include "config.h"

#define  GPIO_IR  4
#define mqtt_sub axk_mqtt_client_subscribe
#define mqtt_pub axk_mqtt_client_publish
#define RAWSIZE 139     

static uint16_t rawData[15][RAWSIZE]={  //temp code
    {9000, 4436,  692, 1602,  694, 532,  668, 508,  694, 1604,  692, 1628,  668, 510,  690, 510,  692, 510,  692, 510,  692, 508,  694, 508,  692, 532,  670, 508,  692, 510,  690, 508,  694, 508,  692, 512,  690, 508,  694, 532,  670, 508,  692, 512,  690, 1604,  694, 510,  692, 532,  668, 512,  690, 510,  692, 510,  692, 508,  694, 1604,  692, 508,  692, 1606,  690, 510,  692, 510,  692, 1604,  692, 510,  690, 19928,  690, 510,  692, 508,  692, 532,  668, 508,  694, 508,  694, 510,  692, 510,  692, 508,  692, 510,  692, 510,  692, 510,  690, 510,  692, 508,  692, 1606,  690, 510,  692, 506,  694, 510,  692, 510,  692, 532,  668, 510,  690, 534,  666, 512,  688, 510,  692, 510,  690, 510,  690, 512,  690, 510,  690, 510,  692, 1606,  690, 510,  690, 1608,  690, 510,  692},//16
    {9000, 4436,  690, 1606,  692, 508,  692, 508,  692, 1604,  692, 1604,  692, 508,  692, 510,  690, 508,  692, 1604,  692, 512,  690, 508,  692, 510,  690, 510,  692, 532,  668, 508,  692, 532,  668, 508,  692, 512,  688, 510,  692, 510,  690, 510,  692, 1604,  692, 508,  692, 510,  692, 510,  692, 508,  692, 508,  692, 510,  692, 1604,  692, 510,  692, 1604,  690, 512,  690, 510,  690, 1628,  668, 510,  692, 19926,  692, 510,  692, 508,  694, 510,  690, 508,  692, 510,  690, 508,  692, 512,  688, 508,  692, 532,  668, 510,  692, 510,  692, 510,  692, 508,  694, 1604,  690, 532,  668, 510,  692, 510,  692, 510,  690, 510,  690, 510,  692, 532,  668, 510,  692, 510,  690, 510,  690, 510,  692, 510,  692, 510,  690, 512,  690, 510,  692, 1604,  692, 1604,  692, 512,  690},//17
    {9000, 4436,  692, 1604,  692, 510,  692, 508,  692, 1606,  692, 1604,  692, 508,  692, 508,  692, 508,  694, 508,  692, 1604,  692, 510,  690, 512,  690, 510,  692, 510,  690, 508,  692, 510,  690, 510,  692, 510,  690, 510,  692, 510,  690, 510,  692, 1604,  692, 510,  690, 508,  694, 532,  668, 510,  692, 510,  690, 510,  692, 1604,  692, 512,  688, 1606,  690, 510,  690, 508,  692, 1606,  690, 532,  668, 19930,  690, 510,  692, 508,  692, 510,  690, 510,  690, 510,  692, 510,  690, 510,  692, 508,  692, 510,  692, 510,  690, 532,  670, 510,  692, 508,  692, 1628,  668, 510,  690, 510,  692, 510,  690, 508,  692, 510,  692, 510,  690, 510,  692, 508,  692, 510,  692, 510,  690, 510,  692, 508,  692, 510,  692, 508,  692, 1606,  690, 1604,  692, 1606,  690, 510,  692},//18
    {9000, 4436,  692, 1606,  690, 512,  688, 510,  690, 1606,  692, 1606,  690, 510,  692, 510,  690, 510,  690, 1604,  692, 1604,  692, 510,  692, 508,  692, 510,  690, 510,  692, 510,  692, 510,  692, 510,  690, 510,  690, 508,  694, 532,  668, 510,  692, 1604,  692, 510,  692, 510,  690, 510,  692, 532,  668, 510,  692, 510,  692, 1606,  690, 508,  692, 1628,  668, 510,  690, 510,  692, 1604,  692, 510,  692, 19926,  692, 510,  692, 508,  692, 510,  692, 510,  692, 532,  668, 512,  690, 532,  668, 510,  692, 510,  692, 512,  690, 510,  690, 510,  692, 508,  692, 1604,  692, 512,  690, 534,  668, 510,  692, 508,  692, 532,  668, 510,  692, 508,  692, 510,  692, 508,  692, 510,  692, 510,  690, 510,  692, 510,  690, 512,  688, 510,  692, 510,  692, 510,  690, 1604,  692},//19
    {9000, 4436,  692, 1604,  692, 512,  690, 510,  692, 1604,  692, 1604,  692, 508,  692, 510,  692, 508,  694, 508,  692, 508,  692, 1606,  690, 508,  692, 510,  690, 510,  692, 508,  692, 510,  690, 510,  690, 508,  692, 532,  668, 510,  690, 510,  690, 1604,  692, 510,  692, 510,  692, 508,  692, 512,  690, 510,  690, 512,  690, 1606,  690, 512,  690, 1606,  690, 510,  690, 510,  692, 1606,  690, 510,  690, 19928,  692, 510,  690, 508,  694, 508,  692, 508,  694, 510,  690, 510,  692, 508,  692, 510,  690, 508,  692, 508,  692, 508,  692, 510,  690, 534,  668, 1604,  690, 510,  692, 510,  690, 534,  668, 510,  692, 508,  692, 510,  690, 508,  692, 510,  690, 510,  692, 510,  690, 510,  690, 534,  668, 508,  692, 510,  690, 1606,  690, 510,  692, 508,  692, 1604,  690},//20
    {9000, 4436,  690, 1604,  692, 510,  692, 508,  692, 1606,  690, 1604,  692, 508,  692, 510,  690, 510,  692, 1604,  692, 508,  692, 1628,  668, 510,  692, 510,  690, 510,  690, 510,  692, 510,  692, 508,  692, 510,  692, 512,  690, 532,  668, 510,  690, 1606,  692, 508,  692, 510,  690, 512,  690, 508,  692, 508,  692, 510,  690, 1604,  692, 508,  692, 1606,  690, 510,  692, 510,  690, 1604,  692, 532,  668, 19928,  692, 508,  692, 508,  692, 510,  692, 510,  690, 510,  692, 508,  692, 510,  692, 510,  690, 510,  690, 510,  694, 508,  692, 508,  692, 510,  690, 1606,  690, 508,  694, 508,  692, 508,  694, 508,  692, 512,  690, 510,  690, 510,  692, 510,  690, 510,  692, 510,  692, 510,  690, 510,  690, 510,  690, 510,  690, 512,  690, 1606,  690, 510,  690, 1604,  692},//21
    {9000, 4436,  690, 1628,  668, 510,  692, 510,  690, 1604,  692, 1606,  690, 510,  692, 508,  692, 508,  694, 508,  694, 1602,  692, 1606,  692, 510,  692, 508,  692, 510,  692, 510,  692, 508,  692, 510,  690, 510,  690, 510,  690, 510,  692, 510,  690, 1606,  692, 508,  692, 508,  692, 508,  692, 510,  692, 510,  690, 510,  692, 1604,  692, 510,  692, 1604,  692, 510,  692, 510,  692, 1604,  692, 510,  692, 19928,  690, 508,  692, 508,  692, 510,  690, 510,  692, 508,  692, 510,  692, 510,  692, 510,  690, 508,  694, 508,  694, 508,  692, 510,  690, 510,  690, 1628,  668, 510,  692, 510,  692, 510,  690, 508,  692, 508,  692, 510,  690, 510,  690, 532,  668, 510,  690, 508,  692, 512,  690, 510,  692, 510,  692, 508,  692, 1606,  690, 1604,  692, 508,  692, 1606,  690},//22
    {9000, 4436,  692, 1604,  692, 508,  694, 508,  692, 1604,  692, 1606,  690, 510,  690, 510,  692, 508,  692, 1604,  692, 1604,  692, 1604,  692, 532,  668, 532,  668, 510,  692, 532,  670, 508,  692, 510,  692, 510,  692, 508,  692, 510,  692, 508,  692, 1628,  670, 508,  692, 512,  690, 510,  690, 510,  690, 510,  690, 510,  692, 1628,  668, 510,  690, 1606,  690, 510,  692, 510,  690, 1604,  692, 510,  690, 19950,  668, 508,  692, 510,  690, 508,  692, 508,  692, 508,  692, 510,  692, 510,  692, 510,  692, 512,  690, 510,  690, 510,  692, 532,  668, 510,  690, 1604,  692, 512,  690, 510,  692, 508,  692, 508,  692, 510,  692, 510,  690, 508,  692, 510,  690, 510,  690, 510,  692, 508,  692, 510,  690, 512,  690, 510,  692, 510,  692, 510,  690, 1604,  692, 1628,  668},//23
    {9000, 4436,  694, 1604,  692, 510,  692, 508,  692, 1606,  692, 1604,  692, 510,  690, 512,  688, 510,  692, 510,  690, 510,  692, 508,  692, 1606,  690, 510,  690, 532,  668, 508,  692, 510,  690, 510,  692, 532,  668, 510,  690, 510,  690, 512,  690, 1606,  692, 510,  690, 510,  690, 510,  692, 510,  692, 512,  690, 510,  692, 1604,  692, 510,  690, 1628,  668, 532,  668, 532,  668, 1604,  692, 510,  690, 19928,  692, 510,  692, 510,  690, 510,  692, 508,  692, 510,  690, 512,  690, 510,  690, 508,  692, 510,  692, 532,  668, 510,  690, 508,  692, 510,  690, 1602,  694, 532,  668, 510,  692, 510,  690, 510,  692, 510,  690, 510,  692, 512,  690, 512,  690, 510,  690, 510,  692, 510,  690, 508,  692, 512,  690, 510,  692, 1604,  692, 510,  692, 1604,  692, 1606,  690},//24
    {9000, 4436,  692, 1604,  692, 510,  690, 510,  690, 1606,  690, 1606,  690, 512,  690, 510,  692, 510,  692, 1604,  692, 510,  690, 510,  690, 1628,  668, 510,  692, 510,  692, 510,  692, 510,  690, 534,  668, 510,  690, 512,  690, 510,  690, 508,  692, 1604,  692, 510,  690, 510,  692, 512,  690, 510,  690, 510,  690, 510,  692, 1604,  690, 510,  692, 1606,  690, 510,  690, 510,  692, 1606,  690, 510,  690, 19928,  690, 510,  692, 508,  692, 510,  690, 510,  690, 510,  692, 510,  690, 512,  690, 508,  692, 510,  692, 510,  690, 512,  690, 510,  690, 508,  692, 1606,  690, 510,  692, 510,  692, 510,  690, 510,  692, 510,  690, 510,  692, 510,  690, 510,  692, 510,  692, 508,  692, 510,  690, 510,  692, 510,  690, 510,  692, 508,  692, 1606,  690, 1604,  692, 1604,  692},//25
    {9000, 4436,  694, 1604,  692, 510,  692, 510,  690, 1602,  694, 1606,  690, 532,  668, 510,  690, 532,  668, 508,  692, 1604,  690, 510,  692, 1604,  692, 510,  690, 510,  692, 510,  690, 510,  690, 508,  692, 510,  690, 510,  692, 512,  690, 510,  690, 1604,  692, 512,  690, 510,  690, 508,  692, 510,  692, 510,  690, 510,  692, 1606,  690, 510,  692, 1604,  692, 512,  688, 510,  690, 1604,  692, 510,  690, 19926,  692, 508,  692, 510,  692, 508,  692, 510,  692, 510,  692, 510,  690, 510,  692, 510,  692, 510,  692, 510,  690, 512,  690, 506,  694, 508,  692, 1604,  692, 510,  690, 532,  668, 510,  692, 508,  692, 510,  690, 532,  668, 510,  692, 510,  692, 532,  668, 510,  692, 510,  690, 510,  692, 532,  668, 532,  668, 1604,  692, 1604,  692, 1604,  694, 1604,  692},//26
    {9000, 4436,  668, 1606,  690, 510,  692, 512,  688, 1628,  668, 1602,  692, 512,  690, 510,  690, 510,  692, 1602,  694, 1628,  668, 508,  694, 1604,  692, 508,  694, 508,  694, 510,  690, 508,  692, 508,  692, 510,  692, 510,  690, 510,  692, 510,  690, 1608,  688, 512,  690, 510,  692, 510,  692, 510,  692, 510,  690, 510,  692, 1604,  692, 510,  690, 1604,  692, 512,  690, 510,  690, 1604,  692, 510,  692, 19930,  690, 510,  692, 508,  692, 510,  692, 508,  692, 510,  692, 508,  692, 508,  692, 510,  692, 510,  690, 510,  692, 508,  692, 508,  692, 512,  688, 1628,  668, 510,  690, 532,  668, 534,  668, 510,  692, 510,  690, 510,  692, 508,  692, 510,  690, 512,  692, 512,  690, 508,  692, 510,  690, 532,  668, 510,  692, 510,  690, 510,  692, 510,  690, 512,  690},//27
    {9000, 4436,  690, 1606,  690, 532,  668, 508,  692, 1604,  692, 1604,  692, 508,  692, 512,  690, 512,  690, 510,  692, 508,  692, 1606,  690, 1628,  668, 510,  692, 510,  692, 510,  692, 508,  692, 510,  692, 508,  692, 508,  692, 510,  692, 510,  692, 1604,  692, 510,  692, 510,  690, 510,  690, 510,  692, 510,  690, 534,  668, 1606,  690, 510,  690, 1628,  668, 510,  692, 510,  690, 1604,  694, 510,  690, 19928,  690, 510,  690, 510,  690, 510,  692, 510,  692, 506,  692, 512,  690, 510,  690, 510,  692, 532,  668, 508,  694, 510,  690, 510,  690, 510,  692, 1606,  692, 510,  690, 508,  692, 510,  690, 510,  692, 510,  690, 510,  692, 510,  692, 510,  692, 508,  692, 508,  692, 510,  692, 510,  692, 510,  692, 510,  690, 1604,  692, 510,  690, 512,  690, 534,  666},//28
    {9000, 4436,  694, 1626,  670, 510,  690, 510,  692, 1602,  694, 1604,  690, 510,  692, 510,  690, 510,  692, 1604,  692, 508,  692, 1604,  692, 1628,  668, 512,  690, 510,  692, 510,  690, 532,  670, 508,  692, 510,  692, 510,  692, 508,  692, 510,  692, 1604,  692, 510,  692, 508,  692, 510,  690, 512,  690, 510,  690, 510,  690, 1628,  668, 508,  692, 1604,  692, 510,  690, 510,  690, 1604,  692, 512,  690, 19926,  692, 510,  690, 510,  690, 508,  692, 510,  692, 508,  692, 508,  692, 510,  690, 512,  690, 508,  692, 508,  694, 510,  692, 508,  692, 510,  690, 1606,  690, 534,  668, 510,  692, 510,  692, 510,  690, 510,  690, 510,  692, 510,  690, 512,  690, 510,  690, 510,  690, 510,  690, 508,  692, 508,  692, 510,  692, 510,  690, 1604,  690, 510,  692, 508,  692},//29
    {9000, 4436,  668, 1606,  692, 510,  690, 510,  692, 1604,  692, 1606,  688, 510,  692, 510,  690, 512,  690, 510,  690, 1604,  692, 1604,  692, 1606,  690, 510,  692, 510,  692, 510,  690, 510,  692, 510,  692, 508,  692, 510,  692, 510,  692, 510,  692, 1604,  692, 510,  692, 508,  692, 510,  692, 510,  692, 510,  692, 510,  692, 1604,  690, 510,  692, 1604,  692, 510,  692, 510,  690, 1604,  692, 510,  690, 19928,  692, 510,  690, 508,  692, 510,  690, 510,  692, 508,  692, 510,  692, 512,  690, 508,  692, 506,  694, 510,  690, 510,  690, 510,  690, 512,  688, 1606,  690, 508,  692, 510,  692, 512,  690, 532,  668, 510,  690, 510,  692, 508,  692, 510,  692, 512,  690, 510,  690, 510,  690, 510,  692, 510,  692, 512,  690, 1606,  690, 1604,  692, 532,  668, 510,  690}//30          
};
static uint16_t offData[RAWSIZE]= {9000, 4436,  688, 1606,  690, 534,  666, 512,  690, 512,  688, 1608,  690, 510,  690, 512,  690, 510,  690, 512,  688, 512,  690, 510,  690, 1608,  690, 512,  688, 512,  690, 512,  688, 510,  690, 512,  688, 514,  688, 510,  692, 512,  686, 512,  688, 1608,  688, 510,  692, 512,  688, 514,  688, 512,  688, 510,  690, 514,  688, 1606,  690, 512,  688, 1608,  688, 514,  688, 534,  668, 1608,  688, 512,  688, 19950,  666, 512,  688, 510,  690, 512,  688, 512,  688, 512,  690, 510,  690, 512,  688, 512,  688, 534,  668, 512,  688, 510,  690, 512,  690, 512,  688, 1606,  690, 510,  690, 512,  688, 512,  690, 514,  688, 512,  688, 512,  688, 534,  668, 512,  688, 512,  690, 514,  686, 512,  690, 512,  690, 512,  688, 512,  690, 1606,  688, 514,  686, 1608,  690, 512,  688};

void mqtt_connected_callback();
void mqtt_start(void);
BL_Err_Type PWM_Smart_Configure2(PWM_CH_ID_Type ch, uint16_t clkDiv, uint16_t period, uint16_t threshold2);
static void timer_cb(void *arg);
static void ctlAC(int status);
static void offAC(void);
static void onAC(void);
static void Send_IR(uint16_t *buf,uint8_t len);
#endif

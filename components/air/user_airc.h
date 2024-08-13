/**
 * @file wifi6_led.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
*/
#ifndef USER_AIRC_H
#define USER_AIRC_H

#define LED_CONFIG_NET 14
#define LED_DEV 15
#define IR 0


// void timer0_isr(int irq, void *arg);
void ac_init(void);
void led_indicator_change(int led_state);
void led_dev_open(int* led_state);
void led_dev_cloes(int* led_state);
void ctlAC(int status);
void Wait_High(uint16_t a);
void Wait_Low(uint16_t a);
void my_pwm_gpio_init(void);
void Send_IR(uint16_t *buf,uint8_t len);
#endif
/**
 * @file user_state.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
*/
#ifndef USER_STATE_H
#define USER_STATE_H

#define SSID "CMCC-RS6f"  
#define PASS "password"

#define SSID_KEY "SSID"
#define PASS_KEY "PASS"
#define CHANNEL_KEY "CH"
#define LED_KEY "LED"

#define MQ_USERNAME "test"
#define MQ_DEF_HOST "192.168.1.254"
#define MQ_DEF_PORT 1888
#define MQ_HOST_KEY "MQ_HOST"
#define MQ_SUB_TOPIC "home/ac"
#define MQ_PUB_TOPIC "home/ac"

typedef enum {
    STATE_SYSTEM_START = 0,
    STATE_WIFI_CONNECTING,
    STATE_WIFI_CONNECTED_OK,
    STATE_WIFI_DISCONNECTED,
    STATE_MQTT_SERVER_CONENCT_OK,
    STATE_MQTT_SERVER_CONNECT_ERR,
    STATE_MQTT_SERVER_DISCONNECT,
    STATE_MQTT_MSG_ARRIVES, //7
}user_state_t;

typedef struct {
    char data[512];
    char topic[128];
}mq_msg_t;
extern mq_msg_t mq_msg;

void user_state_start(void);
void user_state_send_notify(user_state_t state, int is_irq);
int get_server_led(char* data);
void going(int massage);
char* led_payload_update(int led_state);
#endif

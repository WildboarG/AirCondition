#include "aircondition.h"


int mqtt_event = 0;
bool ir_pin_status = false;
axk_mqtt_client_handle_t client;
static hosal_timer_dev_t timer0;

extern uint16_t rawData[15][139];
extern uint16_t offData[139];

void mqtt_connected_callback(){
    puts("successfuly connected server!\r\n");
    mqtt_sub(client,MQTT_TOPIC,1);
    //mqtt_pub(client,"air","online",0,1,0);
}


static void mqtt_topic_data(char *topic,char * payload){
    if(strcmp(topic,MQTT_TOPIC)==0){
        if(strcmp(payload,"on")==0){
                onAC();  
                return;
        }
        if(strcmp(payload,"off")==0){
                offAC();
                return;
        }else{
            int temp = atoi(payload);
            if((temp > 15) && (temp < 31)){
                    ctlAC(temp);
                    return;
            }
        }
    }
}
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        blog_error("Last error %s: 0x%x", message, error_code);
    }
}
static axk_err_t event_cb(axk_mqtt_event_handle_t event)
{
    int32_t event_id;
    client = event->client;
    event_id = event->event_id;
    blog_debug("Event dispatched, event_id=%d", event_id);
    int msg_id;
    switch ((axk_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        blog_info("MQTT_EVENT_CONNECTED");
        mqtt_connected_callback();
        mqtt_event = 1;

        break;
    case MQTT_EVENT_DISCONNECTED:
        blog_info("MQTT_EVENT_DISCONNECTED");
        mqtt_event = 0;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        blog_info("MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = axk_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        blog_info("sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        blog_info("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        blog_info("MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        blog_info("MQTT_EVENT_DATA");
        // printf("topic=%.*s\r\n", event->topic_len, event->topic);
        // printf("payload=%.*s\r\n", event->data_len, event->data);

        char mqtt_topic[50] = "";
        char mqtt_payload[20] = "";
        
        for(int i = 0; i < (event->topic_len); i++)
            mqtt_topic[i] = (char)(event->topic)[i];
        printf("mqtt_topic:");
        printf(mqtt_topic);
        printf("\r\n");

        for(int i = 0; i < (event->data_len); i++)
            mqtt_payload[i] = (char)(event->data)[i];
        printf("mqtt_payload:");
        printf(mqtt_payload);
        printf("\r\n");
        mqtt_topic_data(mqtt_topic, mqtt_payload); //处理接收到的数据
        break;

    case MQTT_EVENT_ERROR:
        blog_info("MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from axk-tls", event->error_handle->axk_tls_last_axk_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->axk_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->axk_transport_sock_errno);
            blog_info("Last errno string (%s)", strerror(event->error_handle->axk_transport_sock_errno));
        }
        break;
    default:
        blog_info("Other event id:%d", event->event_id);
        break;
    }
    return AXK_OK;
}


//定时器中断回调函数
static void timer_cb(void *arg)
{
    // static int i = 0;
    
    // if (i % 2) {
    //     bl_gpio_output_set(4, 0);
    // } else {
    //     bl_gpio_output_set(4, 1);
    // }
    // i++;
    ir_pin_status ? bl_gpio_output_set(GPIO_IR,0) : bl_gpio_output_set(GPIO_IR,1);
    ir_pin_status=!ir_pin_status;
}
//控制空调TEMP
static void ctlAC(int status){
    printf("temp : %d\r\n",status);
    Send_IR(rawData[status - 16],RAWSIZE);
    bl_timer_delay_us(100);
    ir_pin_status = false;
}
static void offAC(void){
    printf("trun off \r\n");
    Send_IR(offData,RAWSIZE);
     bl_timer_delay_us(100);
    ir_pin_status = false;
}
static void onAC(void){
    printf("trun on \r\n");
    Send_IR(rawData[8],RAWSIZE);
     bl_timer_delay_us(100);
    ir_pin_status = false;
}
//发送temp码
static void Send_IR(uint16_t *buf,uint8_t len)
{
   // printf("Sending IR code...\n");
    for (uint16_t i = 0; i < len; i++) {
        if (i%2==0) {
             hosal_timer_start(&timer0);
             bl_timer_delay_us(buf[i]);
             hosal_timer_stop(&timer0);

        } else {
                bl_gpio_output_set(GPIO_IR, 0);
                bl_timer_delay_us(buf[i]);
        }
    }
    bl_timer_delay_us(4096);
     bl_gpio_output_set(GPIO_IR, 0);  //发送完成把gpio拉低，省电
}

void mqtt_start(void){
        // 初始化gpio4 low
        bl_gpio_enable_output(GPIO_IR, 0, 0);
        bl_gpio_output_set(GPIO_IR,0);

        // 初始化定时器0
        timer0.config .arg = NULL;
        timer0.config .cb = timer_cb;
        timer0.config .period = 13      ;/// 13us
        timer0.config .reload_mode = TIMER_RELOAD_PERIODIC;
        timer0.port = 0;

        hosal_timer_init(&timer0);


     //连接mqtt
    axk_mqtt_client_config_t mqtt_cfg = {
        .host = MQTT_HOST,
        .port = MQTT_PORT,
        .username = MQTT_USERNAME,
        .password = MQTT_PASSWORD,
        .client_id = MQTT_CLIENT_ID,
        .event_handle = event_cb,
        .lwt_topic = MQTT_TOPIC,
        .lwt_msg = MQTT_MSG,
        .lwt_qos = 1,
        .keepalive = 15,
    };
    axk_mqtt_client_handle_t client = axk_mqtt_client_init(&mqtt_cfg);
    axk_mqtt_client_start(client);
}




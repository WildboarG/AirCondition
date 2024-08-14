#include "aircondition.h"

#define PWM_Get_Channel_Reg(ch)  (PWM_BASE+PWM_CHANNEL_OFFSET+(ch)*0x20)
#define PWM_STOP_TIMEOUT_COUNT   (160*1000)

int mqtt_event = 0;
bool ir_pin_status = false;
axk_mqtt_client_handle_t client;
static hosal_timer_dev_t timer0;

extern uint16_t rawData[15][139];
extern uint16_t offData[139];


BL_Err_Type PWM_Smart_Configure2(PWM_CH_ID_Type ch, uint16_t clkDiv, uint16_t period, uint16_t threshold2)
{
    uint32_t tmpVal;
    uint32_t timeoutCnt = PWM_STOP_TIMEOUT_COUNT;
    /* 获取通道寄存器 */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    if(BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL) != PWM_CLK_BCLK){
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmpVal, PWM_STOP_EN));
        while(!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)){
            timeoutCnt--;
            if(timeoutCnt == 0){
                return TIMEOUT;
            }
        }
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL, PWM_CLK_BCLK);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_OUT_INV, PWM_POL_NORMAL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_STOP_MODE, PWM_STOP_GRACEFUL);
    BL_WR_REG(PWMx, PWM_CONFIG, tmpVal);

    /* 配置 PWM 分频 */
    BL_WR_REG(PWMx, PWM_CLKDIV, clkDiv);

    /* 配置 PWM 周期和占空比 */
    BL_WR_REG(PWMx, PWM_PERIOD, period);
    BL_WR_REG(PWMx, PWM_THRE1, 0);
    BL_WR_REG(PWMx, PWM_THRE2, threshold2);

    return SUCCESS;
}


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
#if 0
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
#endif
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
   //  bl_timer_delay_us(100);
   // ir_pin_status = false;
}
static void onAC(void){
    printf("trun on \r\n");
    Send_IR(rawData[8],RAWSIZE);
   //  bl_timer_delay_us(100);
   // ir_pin_status = false;
}
//发送temp码
static void Send_IR(uint16_t *buf,uint8_t len)
{
   // printf("Sending IR code...\n");
    for (uint16_t i = 0; i < len; i++) {
        if (i%2==0) {
             //hosal_timer_start(&timer0);
             PWM_Channel_Enable(4);
             bl_timer_delay_us(buf[i]);
             PWM_Channel_Disable(4);
             //hosal_timer_stop(&timer0);

        } else {
                bl_gpio_output_set(GPIO_IR, 0);
                bl_timer_delay_us(buf[i]);
        }
    }
    bl_timer_delay_us(10000);
    bl_gpio_output_set(GPIO_IR, 0);  //发送完成把gpio拉低，省电
}

void mqtt_start(void){
        // 初始化gpio4 low
       // bl_gpio_enable_output(GPIO_IR, 0, 0);
       // bl_gpio_output_set(GPIO_IR,0);

        // 初始化定时器0
    #if 0 
        timer0.config .arg = NULL;
        timer0.config .cb = timer_cb;
        timer0.config .period = 13      ;/// 13us
        timer0.config .reload_mode = TIMER_RELOAD_PERIODIC;
        timer0.port = 0;

        hosal_timer_init(&timer0);
    #endif
        //pwm   
        GLB_GPIO_Cfg_Type cfg={
            .drive = 0,
            .smtCtrl = 1,
            .gpioMode = GPIO_MODE_OUTPUT,
            .pullType = GPIO_PULL_DOWN,
            .gpioPin = 4,  /// io4
            .gpioFun = 8,
};
        GLB_GPIO_Init(&cfg);

        PWM_CH_ID_Type ch = 4; // PWM通道2
        PWM_Channel_Disable(ch);

        PWM_Smart_Configure2(4, 14, 150, 75); // 配置PWM通道2, 时钟为BCLK/7, 周期为150us, 占空比为75%

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




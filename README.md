## AirCondition

Make a simple air-conditioning remote control with Ai-M61-32S-kit

---

 ![](https://img.shields.io/badge/License-MIT-reightgreen.svg) 

### 使用

---

1. 连接wifi
2. 订阅MQTT服务
3. 接入HOMEASSISTANT



### 原理

---

- 使用GPIO模拟高低电平控制红外发射管(使用pwm改变电平状态)
- 使用空调遥控器的原始数据发送（如何获取各种遥控器的原始数据，参见arduino  IRremoteESP8266/IRrecvDumpV2.ino）



### 配置

---

- 配置wifi ,mqtt订阅主题 (/state/user_state.h)
- 获取空调遥控器的RawData 以数组的形式存放user_aric.c 目录下

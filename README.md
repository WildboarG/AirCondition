## 说明

*使用BL602-wb2复刻了一遍Aircondition*



## 配置

---

- WIFI
  - 连接wifi使用的是smart config，上电直接用小程序配网就可以了

- MQTT
  - 采用bafa云作为mqtt服务器，配置在`config.h`
  - 由于bafa的mqtt使用的是密钥连接，需要将用户私钥转一下，用`getsecretkey.sh`根据提示，输入私钥，获取的appID 就是用户名，获取的secretkey 就是密码。



## 使用

---

- 切换到wb2分支

```shell
$ git checkout wb2
```

- 填写config.h
- 更改空调遥控码在`aircondition.h`
- 修改Makefile中SDK的地址
- 编译

```shell
$ make -j32
```

- 连接wb2-01s板子
- 烧录

```shell
make flash p=你的串口地址 b=921600
```


/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-24     Tobby       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "signal_led.h"
#include "pin_config.h"

#define DBG_TAG "led"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

//定义信号灯对象句柄

static led_t *tech_alarm_red = RT_NULL;
static led_t *lowB_alarm_red = RT_NULL;
static led_t *leak_alarm_red = RT_NULL;
static led_t *valve_status_green = RT_NULL;
static led_t *power_status_green = RT_NULL;

static led_t *WIFI_B = RT_NULL;
static led_t *WIFI_R = RT_NULL;
static led_t *WIFI_O = RT_NULL;
static led_t *RF_R = RT_NULL;
static led_t *RF_G = RT_NULL;
static led_t *beep = RT_NULL;
static led_t *beep_k = RT_NULL;

extern uint32_t Gateway_ID;

//定义内存操作函数接口
led_mem_opreation_t led_mem_opreation;

static void tech_alarm_red_on(void *param)
{
    ws2812b_red(0, 1);
}

static void tech_alarm_red_off(void *param)
{
    ws2812b_red(0, 0);
}

static void lowB_alarm_red_on(void *param)
{
    ws2812b_red(1, 1);
}

static void lowB_alarm_red_off(void *param)
{
    ws2812b_red(1, 0);
}

static void leak_alarm_red_on(void *param)
{
    ws2812b_red(2, 1);
}

static void leak_alarm_red_off(void *param)
{
    ws2812b_red(2, 0);
}

static void valve_status_green_on(void *param)
{
    ws2812b_green(3, 1);
}

static void valve_status_green_off(void *param)
{
    ws2812b_green(3, 0);
}

static void WIFI_B_on(void *param)
{
    ws2812b_blue(4, 1);
}

static void WIFI_B_off(void *param)
{
    ws2812b_blue(4, 0);
}

static void WIFI_R_on(void *param)
{
    ws2812b_red(4, 1);
}

static void WIFI_R_off(void *param)
{
    ws2812b_red(4, 0);
}

static void WIFI_O_on(void *param)
{
    ws2812b_orange(4, 1);
}

static void WIFI_O_off(void *param)
{
    ws2812b_orange(4, 0);
}

static void RF_R_on(void *param)
{
    ws2812b_red(5, 1);
}

static void RF_R_off(void *param)
{
    ws2812b_red(5, 0);
}

static void RF_G_on(void *param)
{
    ws2812b_green(5, 1);
}

static void RF_G_off(void *param)
{
    ws2812b_green(5, 0);
}

static void power_status_green_on(void *param)
{
    ws2812b_green(6, 1);
}

static void power_status_green_off(void *param)
{
    ws2812b_green(6, 0);
}

static void beep_on(void *param)
{
    rt_pin_mode(BUZZER, PIN_MODE_OUTPUT);
    rt_pin_write(BUZZER, PIN_HIGH);
}

static void beep_close(void *param)
{
    rt_pin_mode(BUZZER, PIN_MODE_OUTPUT);
    rt_pin_write(BUZZER, PIN_LOW);
}

static void led_run(void *parameter)
{
    ws2812b_init();
    while (1)
    {
        rt_thread_mdelay(LED_TICK_TIME);
        led_ticks();
        RGB_SendArray();
    }
}

void power_on(void)
{
    led_start(power_status_green);
}


void valve_led_on(void)
{
    led_start(valve_status_green);
}

void valve_led_off(void)
{
    led_stop(valve_status_green);
}

void valve_led_status(uint8_t state)
{
    if (state == 1)
    {
        valve_led_on();
    }
    else
    {
        valve_led_off();
    }
}



void leak_alarm_led_on(void)
{
    led_start(leak_alarm_red);
}

void leak_alarm_led_off(void)
{
    led_stop(leak_alarm_red);
}

void leak_alarm_led_status(uint8_t state)
{
    if (state == 1)
    {
        leak_alarm_led_on();
        LOG_W("leak_alarm_led_ON");
    }
    else
    {
        leak_alarm_led_off();
        LOG_W("leak_alarm_led_OFF");
    }
}

void tech_alarm_led_status(uint8_t state)
{
    if (state == 1)
    {
        led_start(tech_alarm_red);
        LOG_W("tech_alarm_led_ON");
    }
    else
    {
        led_stop(tech_alarm_red);
        LOG_W("tech_alarm_led_OFF");
    }
}


void lowB_alarm_led_status(uint8_t state)
{
    if (state == 1)
    {
        led_start(lowB_alarm_red);
        LOG_W("lowB_alarm_led_ON");
    }
    else
    {
        led_stop(lowB_alarm_red);
        LOG_W("lowB_alarm_led_OFF");
    }
}

void led_test_all_on(void)
{
    led_start(tech_alarm_red);
    led_start(lowB_alarm_red);
    led_start(leak_alarm_red);
    led_start(valve_status_green);
    led_start(WIFI_R);
    led_start(RF_R);
    led_start(power_status_green);
}
MSH_CMD_EXPORT(led_test_all_on, led_test_all_on);

void led_test_all_off(void)
{
    led_stop(tech_alarm_red);
    led_stop(lowB_alarm_red);
    led_stop(leak_alarm_red);
    led_stop(valve_status_green);
    led_stop(WIFI_R);
    led_stop(RF_R);
    led_stop(power_status_green);
}
MSH_CMD_EXPORT(led_test_all_off, led_test_all_off);

int led_Init(void)
{
    led_mem_opreation.malloc_fn = (void* (*)(size_t)) rt_malloc;
    led_mem_opreation.free_fn = rt_free;
    led_set_mem_operation(&led_mem_opreation);

    //初始化信号灯对象

    tech_alarm_red = led_create(tech_alarm_red_on, tech_alarm_red_off, NULL);
    led_set_mode(tech_alarm_red, LOOP_PERMANENT, "200,0,");

    lowB_alarm_red = led_create(lowB_alarm_red_on, lowB_alarm_red_off, NULL);
    led_set_mode(lowB_alarm_red, LOOP_PERMANENT, "200,0,");

    leak_alarm_red = led_create(leak_alarm_red_on, leak_alarm_red_off, NULL);
    led_set_mode(leak_alarm_red, LOOP_PERMANENT, "200,0,");

    valve_status_green = led_create(valve_status_green_on, valve_status_green_off, NULL);
    led_set_mode(valve_status_green, LOOP_PERMANENT, "200,0,");

    WIFI_B = led_create(WIFI_B_on, WIFI_B_off, NULL);
    led_set_mode(WIFI_B, LOOP_PERMANENT, "200,0,");

    WIFI_R = led_create(WIFI_R_on, WIFI_R_off, NULL);
    led_set_mode(WIFI_R, LOOP_PERMANENT, "200,0,");

    WIFI_O = led_create(WIFI_O_on, WIFI_O_off, NULL);
    led_set_mode(WIFI_O, LOOP_PERMANENT, "200,0,");

    RF_R = led_create(RF_R_on, RF_R_off, NULL);
    led_set_mode(RF_R, LOOP_PERMANENT, "200,0,");

    RF_G = led_create(RF_G_on, RF_G_off, NULL);
    led_set_mode(RF_G, LOOP_PERMANENT, "200,0,");

    power_status_green = led_create(power_status_green_on, power_status_green_off, NULL);
    led_set_mode(power_status_green, LOOP_PERMANENT, "200,0,");

    beep = led_create(beep_on, beep_close, NULL);
    led_set_mode(beep, LOOP_PERMANENT, "200,200,");

    beep_k = led_create(beep_on, beep_close, NULL);
    led_set_mode(beep_k, LOOP_PERMANENT, "100,100,");

    rt_thread_t tid = RT_NULL;
    tid = rt_thread_create("signal_led", led_run,
    RT_NULL, 512,
    RT_THREAD_PRIORITY_MAX / 2, 100);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    return RT_EOK;
}

void beep_start(uint8_t count)
{
    led_stop(beep);
    led_set_mode(beep, count, "200,200,");
    led_start(beep);
}

void beep_start_test(void)
{
    uint8_t count = 3;
    led_stop(beep);
    led_set_mode(beep, count, "200,200,");
    led_start(beep);
}
MSH_CMD_EXPORT(beep_start_test, beep_start_test);

void beep_power(uint8_t count)
{
    led_stop(beep);
    led_set_mode(beep, count, "400,400,");
    led_start(beep);
}

void beep_kick(void)
{
    led_start(beep_k);
}

void wifi_led(uint8_t type)
{
    switch (type)
    {
    case 0: //启动失败
        led_stop(WIFI_O);
        led_stop(WIFI_B);
        break;
    case 1: //AP慢闪
        led_stop(WIFI_B);
        led_set_mode(WIFI_O, LOOP_PERMANENT, "400,400,");
        led_start(WIFI_O);
        break;
    case 2: //已配置未连接路由器
        led_stop(WIFI_B);
        led_set_mode(WIFI_O, LOOP_PERMANENT, "150,150,");
        led_start(WIFI_O);
        break;
    case 3: //已连接路由器未连接互联网
        led_stop(WIFI_O);
        led_set_mode(WIFI_B, LOOP_PERMANENT, "150,150,");
        led_start(WIFI_B);
        break;
    case 4: //已连接互联网
        led_stop(WIFI_O);
        led_set_mode(WIFI_B, LOOP_PERMANENT, "200,0,");
        led_start(WIFI_B);
        break;
    default:
        break;
    }
}

void wifi_led_factory(uint8_t type)
{
    switch (type)
    {
    case 0: //关闭全部
        led_stop(WIFI_R);
        led_stop(WIFI_B);
        break;
    case 1: //wifi搜索失败
        led_stop(WIFI_B);
        led_set_mode(WIFI_R, LOOP_PERMANENT, "200,0,");
        led_start(WIFI_R);
        break;
    case 2: //RSSI异常
        led_set_mode(WIFI_B, LOOP_PERMANENT, "200,200,0,200,");
        led_start(WIFI_B);
        led_set_mode(WIFI_R, LOOP_PERMANENT, "0,200,200,200,");
        led_start(WIFI_R);
        break;
    case 3: //RSSI正常
        led_stop(WIFI_R);
        led_set_mode(WIFI_B, LOOP_PERMANENT, "200,0,");
        led_start(WIFI_B);
        break;
    default:
        break;
    }
}

void rf_led_resume(led_t *led)
{
    led_set_blink_over_callback(RF_G, RT_NULL);
    led_set_mode(RF_G, LOOP_PERMANENT, "200,0,");
    led_start(RF_G);
}

void rf_led(uint8_t type)
{
    switch (type)
    {
    case 0: //AX5043初始化失败
        led_stop(RF_G);
        led_set_mode(RF_R, LOOP_PERMANENT, "200,0,");
        led_start(RF_R);
        break;
    case 1: //AX5043初始化成功
        led_stop(RF_R);
        led_set_mode(RF_G, LOOP_PERMANENT, "200,0,");
        led_start(RF_G);
        break;
    case 2: //AX5043发送
        led_stop(RF_R);
        led_set_mode(RF_G, 1, "10,100,");
        led_set_blink_over_callback(RF_G, rf_led_resume);
        led_start(RF_G);
        break;
    case 3: //AX5043接收
        led_stop(RF_R);
        led_set_mode(RF_G, 1, "10,100,");
        led_set_blink_over_callback(RF_G, rf_led_resume);
        led_start(RF_G);
        break;
    }
}

void rf_led_factory(uint8_t type)
{
    switch (type)
    {
    case 0: //AX5043 产测无应答
        led_stop(RF_G);
        led_set_mode(RF_R, LOOP_PERMANENT, "200,0,");
        led_start(RF_R);
        break;
    case 1: //AX5043 RSSI过小
        led_stop(RF_R);
        led_set_mode(RF_G, LOOP_PERMANENT, "200,0,");
        led_start(RF_G);
        break;
    case 2: //AX5043 RSSI正常
        led_stop(RF_R);
        led_set_mode(RF_G, 1, "10,100,");
        led_set_blink_over_callback(RF_G, rf_led_resume);
        led_start(RF_G);
        break;
    }
}

void learn_success(void)
{
    led_set_mode(beep, 5, "200,200,");
    led_start(beep);
}

void learn_fail(void)
{
    LOG_W("Main is Full\r\n");
    led_set_mode(beep, 3, "50,50,200,200,");
    led_start(beep);
}

void led_test(void)
{
    led_set_mode(RF_R, 1, "0,600,1200,600,");
    led_set_mode(RF_G, 1, "600,600,600,600,");
    led_set_mode(WIFI_R, 1, "0,600,1200,600,");
    led_set_mode(WIFI_B, 1, "600,600,600,600,");
    led_start(RF_R);
    led_start(RF_G);
    led_start(WIFI_R);
    led_start(WIFI_B);
}
MSH_CMD_EXPORT(led_test, led_test);

void beep_test(void)
{
    led_set_mode(beep, 4, "200,400,");
    led_start(beep);
}
MSH_CMD_EXPORT(beep_test, beep_test);


/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-22     Tobby       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "button.h"
#include "device.h"
#include "pin_config.h"
#include "led.h"

#define DBG_TAG "key"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define Output_ON     0
#define Output_OFF    1

rt_thread_t Interface_t = RT_NULL;

void Interface_Init()
{
    rt_pin_mode(Input_Control, PIN_MODE_INPUT);

    rt_pin_mode(Valve_Status_OUT, PIN_MODE_OUTPUT);
    rt_pin_write(Valve_Status_OUT, Output_OFF);

    rt_pin_mode(Leak_Alarm_OUT, PIN_MODE_OUTPUT);
    rt_pin_write(Leak_Alarm_OUT, Output_OFF);

    rt_pin_mode(Low_Battery_OUT, PIN_MODE_OUTPUT);
    rt_pin_write(Low_Battery_OUT, Output_OFF);

    rt_pin_mode(Tech_Alarm_OUT, PIN_MODE_OUTPUT);
    rt_pin_write(Tech_Alarm_OUT, Output_OFF);

    rt_pin_mode(Spare_Alarm_OUT, PIN_MODE_OUTPUT);
    rt_pin_write(Spare_Alarm_OUT, Output_OFF);
}

void Valve_Status_Output(uint8_t state)
{
    static last_valve_state = 0;
    LOG_D("LastValveStatus is %d,NowValveState is %d", last_valve_state, state);
    if (last_valve_state != state)
    {
        if (state == 1)
        {
            valve_led_on();
            rt_pin_write(Valve_Status_OUT, Output_ON);
            LOG_D("Valve_Status_OUT_ON");
        }
        else
        {
            valve_led_off();
            rt_pin_write(Valve_Status_OUT, Output_OFF);
            LOG_D("Valve_Status_OUT_oFF");
        }
    }
    last_valve_state = state;
}

void Leak_Alarm_Output(uint8_t state)
{
    static last_leak_state = 0;
    LOG_D("LastLeakStatus is %d,NowLeakState is %d", last_leak_state, state);
    if (last_leak_state != state)
    {
        if (state == 1)
        {
            leak_alarm_led_on();
            rt_pin_write(Leak_Alarm_OUT, Output_ON);
            LOG_D("Leak_Alarm_OUT_ON");
        }
        else
        {
            leak_alarm_led_off();
            rt_pin_write(Leak_Alarm_OUT, Output_OFF);
            LOG_D("Leak_Alarm_OUT_oFF");
        }
    }
    last_leak_state = state;
}

void Low_Battery_Output(uint8_t state)
{
    static last_LowB_state = 0;
    LOG_D("LastLowBStatus is %d,NowLowBState is %d", last_LowB_state, state);
    if (last_LowB_state != state)
    {
        if (state == 1)
        {
            lowB_alarm_led_on();
            rt_pin_write(Low_Battery_OUT, Output_ON);
            LOG_D("Low_Battery_OUT_ON");
        }
        else
        {
            lowB_alarm_led_off();
            rt_pin_write(Low_Battery_OUT, Output_OFF);
            LOG_D("Low_Battery_OUT_oFF");
        }
    }
    last_LowB_state = state;
}

void Tech_Alarm_selfcheck_Output(uint8_t state)
{
    if (state == 0 || state == 1)
    {
        tech_alarm_led_on();
        rt_pin_write(Tech_Alarm_OUT, Output_OFF);
        LOG_D("Tech_Alarm_OUT_oFF");
    }
    else
    {
        tech_alarm_led_off();
        rt_pin_write(Tech_Alarm_OUT, Output_ON);
        LOG_D("Tech_Alarm_OUT_ON");
    }
}

void Tech_Alarm_Output(uint8_t state)
{
    static last_TechA_state = 0;
    LOG_D("LastTechAStatus is %d,NowTechAState is %d", last_TechA_state, state);
    if (last_TechA_state != state)
    {
        if (state == 1)
        {
            tech_alarm_led_on();
            rt_pin_write(Tech_Alarm_OUT, Output_ON);
            LOG_D("Tech_Alarm_OUT_ON");

        }
        else
        {
            tech_alarm_led_off();
            rt_pin_write(Tech_Alarm_OUT, Output_OFF);
            LOG_D("Tech_Alarm_OUT_oFF");
        }
    }
    last_TechA_state = state;
}

void Tech_Alarm_Output_OFF(void)
{
    rt_pin_write(Tech_Alarm_OUT, Output_OFF);
}

MSH_CMD_EXPORT(Tech_Alarm_Output, Tech_Alarm_Output);

void Spare_Alarm_Output(uint8_t state)
{
    static last_SpareA_state = 0;
    LOG_D("LastSpareAStatus is %d,NowTechAState is %d", last_SpareA_state, state);
    if (last_SpareA_state != state)
    {
        if (state == 1)
        {
            rt_pin_write(Tech_Alarm_OUT, Output_ON);
            LOG_D("Tech_Alarm_OUT_ON");
        }
        else
        {
            rt_pin_write(Tech_Alarm_OUT, Output_OFF);
            LOG_D("Tech_Alarm_OUT_oFF");
        }
    }
    last_SpareA_state = state;
}

void Init_Interface_state(void)
{
    rt_pin_write(Valve_Status_OUT, Output_OFF);
    rt_pin_write(Leak_Alarm_OUT, Output_OFF);
    rt_pin_write(Low_Battery_OUT, Output_OFF);
    rt_pin_write(Tech_Alarm_OUT, Output_OFF);
    rt_pin_write(Spare_Alarm_OUT, Output_OFF);
}

uint8_t Get_Input_Control_Level(void)
{
    return rt_pin_read(Input_Control);
}

void InterfaceScan_Callback(void *parameter)
{
    uint8_t InputNowStatus = 0, InputPastStatus = 0;
    uint32_t Linker_MainID;
    LOG_D("Input_Control Init Success\r\n");
    Linker_MainID = GetMainValidInLinker();
    while (1)
    {
        if (Get_Input_Control_Level() == 0)
        {
            rt_thread_mdelay(50);
            if (Get_Input_Control_Level() == 0)
            {
                InputNowStatus = 1; //输入控制开阀
            }
        }
        else
        {
            InputNowStatus = 0; //输入控制关阀
        }

        if (InputNowStatus != InputPastStatus)
        {
            if (InputNowStatus == 1)
            {
                LOG_D("Input control to remote open valve\r\n");
                InputPastStatus = InputNowStatus;
                Moto_OpenRemote(Linker_MainID);
            }
            else
            {
                LOG_D("Input control to remote 10000001 close valve\r\n ");
                InputPastStatus = InputNowStatus;
                Moto_CloseRemote(Linker_MainID);
            }
        }
        rt_thread_mdelay(1000);
    }
}

void InterfaceScan_Init(void)
{
    Interface_Init();
    Interface_t = rt_thread_create("InterfaceScan", InterfaceScan_Callback, RT_NULL, 1024, 15, 10);
    if (Interface_t != RT_NULL)
    {
        rt_thread_startup(Interface_t);
    }
}

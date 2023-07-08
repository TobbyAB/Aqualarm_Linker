/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-15     RT-Thread    first version
 */

#include <rtthread.h>
#include "main.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    RadioID_Init();
    Storage_Init();
    led_Init();
    power_on();
    Key_Reponse_Init();
    Button_Init();
    RF_Init();
    Sync_Init();
    Heart_Init();
    WiFi_Init();
    InterfaceScan_Init();
    while (1)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

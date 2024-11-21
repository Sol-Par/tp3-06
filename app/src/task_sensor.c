/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : task_sensor.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_sensor_attribute.h"
#include "task_menu_attribute.h"
#include "task_menu_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SEN_CNT_INIT			0ul
#define G_TASK_SEN_TICK_CNT_INI		0ul

#define DEL_BTN_XX_MIN				0ul
#define DEL_BTN_XX_MED				25ul
#define DEL_BTN_XX_MAX				5ul

/********************** internal data declaration ****************************/
const task_sensor_cfg_t task_sensor_cfg_list[] = {
	{ID_BTN_MEN,  BTN_MEN_PORT,  BTN_MEN_PIN,  BTN_MEN_PRESSED, DEL_BTN_XX_MAX,
	 EV_MEN_MEN_IDLE,  EV_MEN_MEN_ACTIVE},
	{ID_BTN_ENT,  BTN_ENT_PORT,  BTN_ENT_PIN,  BTN_ENT_PRESSED, DEL_BTN_XX_MAX,
	 EV_MEN_ENT_IDLE,  EV_MEN_ENT_ACTIVE},
	{ID_BTN_NEX,  BTN_NEX_PORT,  BTN_NEX_PIN,  BTN_NEX_PRESSED, DEL_BTN_XX_MAX,
	 EV_MEN_NEX_IDLE,  EV_MEN_NEX_ACTIVE},
	{ID_BTN_ESC,  BTN_ESC_PORT,  BTN_ESC_PIN,  BTN_ESC_PRESSED, DEL_BTN_XX_MAX,
	 EV_MEN_ESC_IDLE,  EV_MEN_ESC_ACTIVE}
};

#define SENSOR_CFG_QTY	(sizeof(task_sensor_cfg_list)/sizeof(task_sensor_cfg_t))

task_sensor_dta_t task_sensor_dta_list[] = {
	{DEL_BTN_XX_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP},
	{DEL_BTN_XX_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP},
	{DEL_BTN_XX_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP},
	{DEL_BTN_XX_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP}
};

#define SENSOR_DTA_QTY	(sizeof(task_sensor_dta_list)/sizeof(task_sensor_dta_t))

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
const char *p_task_sensor 		= "Task Sensor (Sensor Statechart)";
const char *p_task_sensor_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_sensor_cnt;
volatile uint32_t g_task_sensor_tick_cnt;

/********************** external functions definition ************************/
void task_sensor_init(void *parameters)
{
	uint32_t index;
	task_sensor_dta_t *p_task_sensor_dta;
	task_sensor_st_t state;
	task_sensor_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_sensor_init), p_task_sensor);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_sensor), p_task_sensor_);

	g_task_sensor_cnt = G_TASK_SEN_CNT_INIT;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_sensor_cnt), g_task_sensor_cnt);

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Data Pointer */
		p_task_sensor_dta = &task_sensor_dta_list[index];

		/* Print out: Index & Task execution FSM */
		LOGGER_LOG("   %s = %lu", GET_NAME(index), index);

		state = p_task_sensor_dta->state;
		LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

		event = p_task_sensor_dta->event;
		LOGGER_LOG("   %s = %lu\r\n", GET_NAME(event), (uint32_t)event);
	}
	g_task_sensor_tick_cnt = G_TASK_SEN_TICK_CNT_INI;
}

void task_sensor_update(void *parameters)
{
	uint32_t index;
	const task_sensor_cfg_t *p_task_sensor_cfg;
	task_sensor_dta_t *p_task_sensor_dta;
	bool b_time_update_required = false;

	/* Update Task Sensor Counter */
	g_task_sensor_cnt++;

	/* Protect shared resource (g_task_sensor_tick_cnt) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_SEN_TICK_CNT_INI < g_task_sensor_tick_cnt)
    {
    	g_task_sensor_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_sensor_tick_cnt) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_SEN_TICK_CNT_INI < g_task_sensor_tick_cnt)
		{
			g_task_sensor_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	for (index = 0; SENSOR_DTA_QTY > index; index++)
		{
    		/* Update Task Sensor Configuration & Data Pointer */
			p_task_sensor_cfg = &task_sensor_cfg_list[index];
			p_task_sensor_dta = &task_sensor_dta_list[index];

			if (p_task_sensor_cfg->pressed == HAL_GPIO_ReadPin(p_task_sensor_cfg->gpio_port, p_task_sensor_cfg->pin))
			{
				p_task_sensor_dta->event =	EV_BTN_XX_DOWN;
			}
			else
			{
				p_task_sensor_dta->event =	EV_BTN_XX_UP;
			}

			switch (p_task_sensor_dta->state)
			{
				case ST_BTN_XX_UP:

					if (EV_BTN_XX_DOWN == p_task_sensor_dta->event)
					{
						p_task_sensor_dta->state = ST_BTN_XX_FALLING;
						p_task_sensor_dta->tick = p_task_sensor_cfg->tick_max;
					}

					break;

				case ST_BTN_XX_FALLING:

					if (EV_BTN_XX_UP == p_task_sensor_dta->event && p_task_sensor_dta->tick > 0)
					{
						p_task_sensor_dta->state = ST_BTN_XX_UP;
					}

					if (EV_BTN_XX_DOWN == p_task_sensor_dta->event && p_task_sensor_dta->tick > 0)
					{
						p_task_sensor_dta->tick--;

					}

					if (EV_BTN_XX_DOWN == p_task_sensor_dta->event && p_task_sensor_dta->tick == 0)
					{
						put_event_task_menu(p_task_sensor_cfg->signal_up);
						p_task_sensor_dta->state = ST_BTN_XX_DOWN;

					}

					break;

				case ST_BTN_XX_DOWN:

					if (EV_BTN_XX_UP == p_task_sensor_dta->event)
					{
						p_task_sensor_dta->tick = p_task_sensor_cfg->tick_max;
						p_task_sensor_dta->state = ST_BTN_XX_RISING;
					}

					break;

				case ST_BTN_XX_RISING:

					if (EV_BTN_XX_DOWN == p_task_sensor_dta->event && p_task_sensor_dta->tick > 0)
					{
						p_task_sensor_dta->state = ST_BTN_XX_DOWN;
					}
					if (EV_BTN_XX_UP == p_task_sensor_dta->event && p_task_sensor_dta->tick > 0)
					{
						p_task_sensor_dta->tick--;
					}
					if (EV_BTN_XX_UP == p_task_sensor_dta->event && p_task_sensor_dta->tick == 0)
					{
						p_task_sensor_dta->state = ST_BTN_XX_UP;
					}

					break;

				default:

					p_task_sensor_dta->tick  = DEL_BTN_XX_MIN;
					p_task_sensor_dta->state = ST_BTN_XX_UP;
					p_task_sensor_dta->event = EV_BTN_XX_UP;

					break;
			}
		}
    }
}

/********************** end of file ******************************************/

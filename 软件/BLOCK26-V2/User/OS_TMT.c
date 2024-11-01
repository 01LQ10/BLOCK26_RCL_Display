/*-----------------------------------------------------------------------
|                            FILE DESCRIPTION                           |
-----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
  - File name     : tmt.c
  - Author        : zeweni
  - Update date   : 2020.09.28                  
  -	Copyright(c)  : 2020-2021 zeweni. All rights reserved.
-----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
|                            COPYRIGHT NOTICE                            |
------------------------------------------------------------------------*/
/*
 * Copyright (C) 2021, zeweni (17870070675@163.com)

 * This file is part of task management time slice framework.
 * Abbreviated as TMT.
 
 * Task management time slice framework is free software: you can redist-
 * ribute it and/or modify it under the terms of the Apache-2.0 License.

 * Task management time slice framework is distributed in the hope that it 
 * will be useful,but WITHOUT ANY WARRANTY; without even the implied  
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * Apache-2.0 License for more details.

 * You should have received a copy of the Apache-2.0 License.Task management 
 * time slice framework. If not, see <http://www.apache.org/licenses/>.
**/
/*-----------------------------------------------------------------------
|                               INCLUDES                                |
-----------------------------------------------------------------------*/
#include "tmt.h"
/*-----------------------------------------------------------------------
|                                 DATA                                  |
-----------------------------------------------------------------------*/

/**
 * @brief     ������������꣬�ٷ�װһ�㡣
 * @details   The maximum number of tasks is a macro, 
 *            and then one layer is encapsulated.
**/
#define  TASKS_MAX       TMT_TASK_NUM


/**
 * @brief   ����TMT��ز��������ṹ�塣
 * @details Define the structure of the TMT related operation functions.
**/
tmt_class_public tmt;


/**
 * @brief   TMT���࣬����������˽���������ԡ�
 * @details Class of TMT, containing both pbl and pvt properties.
**/
static struct class
{
	tmt_class_public pbl;  /*!< �������֣����⿪�ŵ����ԣ����嶨����ͷ�ļ���*/
	
	struct pvt   /*!< ˽�в��֣����ޱ��ļ��ڵ��á�*/
	{
		struct
		{
            void (* entry) (void); /*!< ���������ָ�롣*/
            uint16_t tim_cnt;     /*!< �������ʱ���������*/
            uint16_t set_time;      /*!< �������õĵ���ʱ�䡣*/
            uint8_t state;         /*!< ��������״̬��*/
		}comp[TASKS_MAX];
		
		uint8_t num;   /*!< �Ѿ�����������������*/
	}pvt;
	
}object;

/*-----------------------------------------------------------------------
|                               FUNCTION                                |
-----------------------------------------------------------------------*/

/**
 * @brief     TMT�����������tickʱ�䡣
 * @details   TMT update task schedule tick time.
 * @param[in] None.
 * @return    None.
**/
static void tmt_update_tick(void)
{
	uint8_t i;
	for (i = 0; i< object.pvt.num; i++) {
		 /* If time arrives */
        if (object.pvt.comp[i].tim_cnt > 0) {
            object.pvt.comp[i].tim_cnt--;
        }
	}	
}

/**
 * @brief     TMT���к������������������
 * @details   TMT runs the function, which is the task scheduler.
 * @param[in] None.
 * @return    None.
**/
static void tmt_run(void)
{
	uint8_t index;
	for (index = 0; index < object.pvt.num; index++) {
		 /* If time arrives */
		if (object.pvt.comp[index].tim_cnt == 0 && object.pvt.comp[index].state != task_stop) {
			object.pvt.comp[index].tim_cnt = object.pvt.comp[index].set_time;
			
			/* 
				To prevent process conflict, 
				only one process can be executed in the same time period.
			*/
			if (object.pvt.comp[index].entry != (void*)(0)) {
				object.pvt.comp[index].entry();  /* Run task */
			}
		}
	}
}


/**
 * @brief     TMT����һ����������
 * @details   TMT creates a task function.
 * @param[in] entry ���������ָ��.
 *            A pointer function without formal parameters.
 * @param[in] set_time ������ȵ�ʱ�� task run time (ticks)
 * @return    [tmt_ok]�����ɹ�.Create successfully.
 * @return    [tmt_nok]����ʧ��.	Create failed.
**/	
static tmt_fun_t tmt_create(void (*entry) (void),uint16_t set_time)
{	
	if (object.pvt.num < TASKS_MAX) {
		object.pvt.comp[object.pvt.num].entry = entry;
		object.pvt.comp[object.pvt.num].set_time = set_time;
		object.pvt.comp[object.pvt.num].tim_cnt = set_time;
		object.pvt.comp[object.pvt.num].state = task_continue;
		object.pvt.num++;
		return tmt_ok;
	} else {
		return tmt_nok;	
	}		
}


/**
 * @brief     TMTɾ��һ����������
 * @details   TMT creates a task function.
 * @param[in] entry ���������ָ��.
 *            A pointer function without formal parameters.
 * @return    [tmt_ok]ɾ���ɹ�.Delete successfully.
 * @return    [tmt_nok]ɾ��ʧ��.	Delete failed.
**/	
static tmt_fun_t tmt_delete(void (*entry) (void))
{	
    uint8_t index;
    if (object.pvt.num > 0) {
        ENTER_TMT_CRI_AREA();
		
        for(index = 0; index < object.pvt.num; index++) {
            if(object.pvt.comp[index].entry == entry) {
                    object.pvt.num--;
                    object.pvt.comp[index].entry = object.pvt.comp[object.pvt.num].entry;
                    object.pvt.comp[index].set_time = object.pvt.comp[object.pvt.num].set_time;
                    object.pvt.comp[index].tim_cnt = object.pvt.comp[object.pvt.num].tim_cnt;
                    object.pvt.comp[index].state = object.pvt.comp[object.pvt.num].state;
                    EXTI_TMT_CRI_AREA();
                    return tmt_ok;
            }
        }
    }
    EXTI_TMT_CRI_AREA();
    return tmt_nok;
}


/**
 * @brief     TMT����һ����������״̬������
 * @details   TMT controls a task running state function.
 * @param[in] entry ���������ָ��.
 *            A pointer function without formal parameters.
 * @param[in] state ��������״̬.Task running status
 * @return    [tmt_ok]�޸ĳɹ�.The modification was successful.
 * @return    [tmt_nok]�޸�ʧ��.The modification was failed.
**/	
static tmt_fun_t tmt_run_ctrl(void (*entry)(void),task_type state)
{
	uint8_t index;
	for (index = 0; index<object.pvt.num; index++) {
		if (object.pvt.comp[index].entry == entry) {
		    object.pvt.comp[index].state = state;
			return tmt_ok;
		}
	}
	return tmt_nok;
}


/**
 * @brief     TMT����һ������ĵ���ʱ�亯����
 * @details   The TMT controls the scheduling time function of a task.
 * @param[in] *taskFunc (void) ���������ָ��.
 *            A pointer function without formal parameters.
 * @param[in] set_time ������ȵ�ʱ�� task run time (ticks)
 * @return    [tmt_ok]�޸ĳɹ�.The modification was successful.
 * @return    [tmt_nok]�޸�ʧ��.The modification was failed.
**/	
static tmt_fun_t tmt_time_ctrl(void (*entry) (void),uint16_t set_time)
{	
	uint8_t index;
	for(index = 0; index< object.pvt.num; index++) {
		if (object.pvt.comp[index].entry == entry) {
			object.pvt.comp[index].tim_cnt = set_time;
			object.pvt.comp[index].set_time = set_time;
			return tmt_ok;
		}
	}
	return tmt_nok;
}


/**
 * @brief   TMT��ʼ��������ע�����Ԫ�ء�
 * @details The TMT initializes the function 
 *          and registers the associated elements.
 * @param   None.
 * @return  None.
**/
void tmt_init(void)
{
	object.pbl.run = tmt_run;
	object.pbl.tick = tmt_update_tick;
	object.pbl.create = tmt_create;
	object.pbl.delete = tmt_delete;
	object.pbl.time_ctrl = tmt_time_ctrl;
	object.pbl.run_ctrl = tmt_run_ctrl;
	object.pvt.num = 0;
	tmt = object.pbl;
}

/*-----------------------------------------------------------------------
|                   END OF FLIE.  (C) COPYRIGHT zeweni                  |
-----------------------------------------------------------------------*/



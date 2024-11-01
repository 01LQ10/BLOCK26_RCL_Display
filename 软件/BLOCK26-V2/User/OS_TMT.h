/*-----------------------------------------------------------------------
|                            FILE DESCRIPTION                           |
-----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
  - File name     : tmt.h
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
|                              TRANSPLANT                               |
-----------------------------------------------------------------------*/
#ifndef __TMT_H_
#define __TMT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Ӳ��ƽ̨��ص�ͷ�ļ����ṩӲ�����еĻ���������һ���ǼĴ���ͷ�ļ���
 * @details   The header files related to the hardware platform provide 
 *            the basic environment for hardware operation, generally 
 *            register header files..
**/
#include     "tim.h"  //STM32��
//#include     "STC8Ax_REG.h"  //51�汾

/**
 * @brief     ����TMT�ٽ����꺯������Ҫ�ر���ض�ʱ�����жϡ�
 * @details   Enter the TMT critical section macro function.
 *            It is necessary to turn off the interrupt of the related timer.
 * @note      ��Ҫ����Ӳ��ƽ̨����ֲ���ʵĹض�ʱ���жϴ��롣
 *            It is necessary to transplant the appropriate off-timer 
 *            interrupt code according to the hardware platform.
**/
#define       ENTER_TMT_CRI_AREA()    do{ __HAL_TIM_DISABLE_IT(&htim1,TIM_IT_UPDATE); }while(0)  //32�汾
//#define       ENTER_TMT_CRI_AREA()    do{ IT0 = 0; }while(0) //51�汾

/**
 * @brief     �˳�TMT�ٽ����꺯������Ҫ����ض�ʱ�����жϡ�
 * @details   Enter the TMT critical section macro function.
 *            It is necessary to turn on the interrupt of the related timer.
 * @note      ��Ҫ����Ӳ��ƽ̨����ֲ���ʵĿ���ʱ���жϴ��롣
 *            It is necessary to transplant the appropriate on-timer 
 *            interrupt code according to the hardware platform.
**/
#define       EXTI_TMT_CRI_AREA()     do{ __HAL_TIM_ENABLE_IT(&htim1,TIM_IT_UPDATE); }while(0) //32�汾
//#define       EXTI_TMT_CRI_AREA()     do{ IT0 = 1; }while(0) //51�汾


/**
 * @brief     TMT�ı��������ض��壬����г�ͻ��������Ϊ0��
 * @details   The variable type of TMT is redefined. 
 *            If there is a conflict, please configure it to 0..
**/
#if 		  (0)

	typedef  unsigned char   uint8_t; /*!<  8 bits */
	typedef  unsigned int   uint16_t; /*!< 16 bits */

#endif

/*-----------------------------------------------------------------------
|                                CONFIG                                 |
-----------------------------------------------------------------------*/

/**
 * @brief     �����������ƺ꣬����ȷ��TMT��Ҫ���ȵ��������������
 * @details   The number of tasks control macro is used to 
 *            determine the maximum number of tasks that TMT
 *            needs to schedule.
**/
#define 	  TMT_TASK_NUM  	 (8)


/*-----------------------------------------------------------------------
|                                 DATA                                  |
-----------------------------------------------------------------------*/


/**
 * @brief     �����ִ��״̬ö���塣
 * @details   The execution status enumerator of the task.
**/
typedef enum
{
    task_stop = 1,   /*!< ֹͣ����״̬��*/
	task_continue = !task_stop /*!< ��������״̬��*/
}task_type;


/**
  * @brief   TMT�����Լ�ö���壬ָʾ�����Ƿ�����ִ�С�
  *          TMT function self checks enumerator. 
  *          indicating whether the function is normally executed.
***/
typedef enum
{
	tmt_nok= 1,     /* Function self checking failed */
	tmt_ok = !tmt_nok /* Function self checking successfed */ 
}tmt_fun_t; 


/*-----------------------------------------------------------------------
|                             API FUNCTION                              |
-----------------------------------------------------------------------*/

/**
 * @brief   TMT��API���������ṹ�塣
 * @details The TMT API manipulates the function structure.
**/
typedef struct
{
	/**
	 * @brief     TMT�����������tickʱ�䡣
	 * @details   TMT update task schedule tick time.
	 * @param[in] None.
	 * @return    None.
	 * @note      �ڶ�ʱ���жϷ������е��á�
	 *            Called in the timer interrupt service function.
	 * @example   tmt.tick();
	**/
	void (*tick) (void);
	
	
	/**
	 * @brief     TMT���к������������������
	 * @details   TMT runs the function, which is the task scheduler.
	 * @param[in] None.
	 * @return    None.
	 * @note      ��main����������ѭ���е��á�
	 *            Called in the infinite loop of main.
	 * @example   tmt.run();
	**/
	void (*run) (void);
	
	
	/**
	 * @brief     TMT����һ����������
	 * @details   TMT creates a task function.
	 * @param[in] entry ���������ָ��.
	 *            A pointer function without formal parameters.
	 * @param[in] setTime ������ȵ�ʱ�� task run time (ticks)
	 * @return    [tmt_ok]�����ɹ�.Create successfully.
	 * @return    [tmt_nok]����ʧ��.	Create failed.
	 * @note      ��main����������ѭ���е��á�
	 *            Called in the infinite loop of main.
	 * @example   tmt.create(enrty,500); //��ʱentry����Ϊ500ticks����һ��
	**/	
	tmt_fun_t (*create) (void (*entry) (void), uint16_t set_time);
	
	
	/**
	 * @brief     TMTɾ��һ����������
	 * @details   TMT creates a task function.
	 * @param[in] entry ���������ָ��.
	 *            A pointer function without formal parameters.
	 * @return    [tmt_ok]ɾ���ɹ�.Delete successfully.
	 * @return    [tmt_nok]ɾ��ʧ��.	Delete failed.
	 * @example   tmt.delete(enrty); //ɾ��entry����
	**/		
	tmt_fun_t (*delete) (void (*entry) (void));
	
	
	/**
	 * @brief     TMT����һ������ĵ���ʱ�亯����
	 * @details   The TMT controls the scheduling time function of a task.
	 * @param[in] entry ���������ָ��.
	 *            A pointer function without formal parameters.
	 * @param[in] setTime ������ȵ�ʱ�� task run time (ticks)
	 * @return    [tmt_ok]�޸ĳɹ�.The modification was successful.
	 * @return    [tmt_nok]�޸�ʧ��.The modification was failed.
	 * @example   tmt.time_ctrl(enrty,100); //�޸�entry����Ϊ100ticks����һ��
	**/	
	tmt_fun_t (*time_ctrl) (void (*entry) (void), uint16_t set_time);
	
	
	/**
	 * @brief     TMT����һ����������״̬������
	 * @details   TMT controls a task running state function.
	 * @param[in] entry ���������ָ��.
	 *            A pointer function without formal parameters.
	 * @param[in] state ��������״̬.Task running status
	 * @return    [tmt_ok]�޸ĳɹ�.The modification was successful.
	 * @return    [tmt_nok]�޸�ʧ��.The modification was failed.
	 * @example   tmt.run_ctrl(enrty,task_stop); //�޸�entry����Ϊֹͣ����.
	**/		
	tmt_fun_t (*run_ctrl) (void (*entry) (void), task_type state);
}tmt_class_public;


/**
 * @brief   TMT��ʼ��������ע�����Ԫ�ء�
 * @details The TMT initializes the function 
 *          and registers the associated elements.
 * @param   None.
 * @return  None.
**/
extern void tmt_init(void);

/**
 * @brief   ����������TMT���������ṹ�塣
 * @details TMT operation function structure declared externally.
 * @note    ������Ҫ���ⲿ���õĺ���,������������ṹ����.
 *          All functions that need to be called externally 
 *          are included in this structure.
**/
extern tmt_class_public tmt;

#ifdef __cplusplus
}
#endif

#endif
/*-----------------------------------------------------------------------
|          END OF FLIE        (C) COPYRIGHT zeweni                      | 
-----------------------------------------------------------------------*/


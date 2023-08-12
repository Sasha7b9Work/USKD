// 2022/6/10 9:08:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>
#include <systick.h>


#ifdef __cplusplus
extern "C" {
#endif


    /*!
        \brief      this function handles NMI exception
        \param[in]  none
        \param[out] none
        \retval     none
    */
    void NMI_Handler(void)
    {
    }

    /*!
        \brief      this function handles HardFault exception
        \param[in]  none    
        \param[out] none
        \retval     none
    */
    void HardFault_Handler(void)
    {
        /* if Hard Fault exception occurs, go to infinite loop */
        while (1) {
        }
    }

    /*!
        \brief      this function handles MemManage exception
        \param[in]  none
        \param[out] none
        \retval     none
    */
    void MemManage_Handler(void)
    {
        /* if Memory Manage exception occurs, go to infinite loop */
        while (1) {
        }
    }

    /*!
        \brief      this function handles BusFault exception
        \param[in]  none
        \param[out] none
        \retval     none
    */
    void BusFault_Handler(void)
    {
        /* if Bus Fault exception occurs, go to infinite loop */
        while (1) {
        }
    }

    /*!
        \brief      this function handles UsageFault exception
        \param[in]  none
        \param[out] none
        \retval     none
    */
    void UsageFault_Handler(void)
    {
        /* if Usage Fault exception occurs, go to infinite loop */
        while (1) {
        }
    }

    /*!
        \brief      this function handles SVC exception
        \param[in]  none
        \param[out] none
        \retval     none
    */
    void SVC_Handler(void)
    {
    }

    /*!
        \brief      this function handles DebugMon exception
        \param[in]  none
        \param[out] none
        \retval     none
    */
    void DebugMon_Handler(void)
    {
    }

    /*!
        \brief      this function handles PendSV exception
        \param[in]  none
        \param[out] none
        \retval     none
    */
    void PendSV_Handler(void)
    {
    }

    /*!
        \brief      this function handles SysTick exception
        \param[in]  none
        \param[out] none
        \retval     none
    */
    void SysTick_Handler(void)
    {
#ifndef GUI
        extern uint timer_counter;

        timer_counter++;
#endif

        delay_decrement();
    }

    void UART3_IRQHandler(void)
    {
        if (RESET != usart_interrupt_flag_get(USART_GPRS_ADDR, USART_INT_FLAG_RBNE)) {

            HAL_USART_GPRS::CallbackOnReceive((char)usart_data_receive(USART_GPRS_ADDR));
        }
    }

#ifdef __cplusplus
}
#endif

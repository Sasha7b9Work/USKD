// 2022/6/10 9:08:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#ifdef DEVICE
#include "Hardware/Button.h"
#endif
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
        LOG_ERROR("                                                                 HardFault_Handler() %d ms", Timer::TimeMS());

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
        LOG_ERROR("MemManage_Handler()");

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
        LOG_ERROR("MemManage_Handler()");

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
        LOG_ERROR("MemManage_Handler()");

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

#ifdef DEVICE
        Button::Update();
#endif
    }

    void UART3_IRQHandler(void)
    {
        if (RESET != usart_interrupt_flag_get(USART_GPRS_ADDR, USART_INT_FLAG_RBNE))
        {
            HAL_USART_GPRS::CallbackOnReceive((char)usart_data_receive(USART_GPRS_ADDR));
        }
    }

    void USART0_IRQHandler(void)
    {
        if (RESET != usart_interrupt_flag_get(USART_LOG_ADDR, USART_INT_FLAG_RBNE))
        {
        }
    }

    void EXTI5_9_IRQHandler(void)
    {
        if (RESET != exti_interrupt_flag_get(EXTI_9))
        {
            exti_interrupt_flag_clear(EXTI_9);
        }
    }

    void I2C0_ER_IRQHandler(void)
    {
#define I2CX I2C0

        /* no acknowledge received */
        if (i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_AERR))
        {
            i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_AERR);
        }

        /* SMBus alert */
        if (i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_SMBALT))
        {
            i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_SMBALT);
        }

        /* bus timeout in SMBus mode */
        if (i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_SMBTO))
        {
            i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_SMBTO);
        }

        /* over-run or under-run when SCL stretch is disabled */
        if (i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_OUERR))
        {
            i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_OUERR);
        }

        /* arbitration lost */
        if (i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_LOSTARB))
        {
            i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_LOSTARB);
        }

        /* bus error */
        if (i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_BERR))
        {
            i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_BERR);
        }

        /* CRC value doesn't match */
        if (i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_PECERR))
        {
            i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_PECERR);
        }

        /* disable the error interrupt */
        i2c_interrupt_disable(I2CX, I2C_INT_ERR);
        i2c_interrupt_disable(I2CX, I2C_INT_BUF);
        i2c_interrupt_disable(I2CX, I2C_INT_EV);
    }

#ifdef __cplusplus
}
#endif

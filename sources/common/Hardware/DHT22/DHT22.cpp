#include "defines.h"
#include "DHT22.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


#define lineDown()      GPIO_BC(sensor->DHT_Port) = sensor->DHT_Pin
#define lineUp()        GPIO_BOP(sensor->DHT_Port) = sensor->DHT_Pin
#define getLine()       (gpio_input_bit_get(sensor->DHT_Port, sensor->DHT_Pin) != RESET)


static void goToOutput(DHT_sensor *sensor)
{
    lineUp();                   //По умолчанию на линии высокий уровень

    gpio_init(sensor->DHT_Port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, sensor->DHT_Pin);
}

static void goToInput(DHT_sensor *sensor)
{
    gpio_init(sensor->DHT_Port, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, sensor->DHT_Pin);
}

DHT_data DHT_getData(DHT_sensor *sensor)
{
    DHT_data data = { -128.0f, -128.0f };

#if DHT_POLLING_CONTROL == 1
    /* Ограничение по частоте опроса датчика */
    //Определение интервала опроса в зависимости от датчика
    uint16_t pollingInterval;

    if (sensor->type == DHT11)
    {
        pollingInterval = DHT_POLLING_INTERVAL_DHT11;
    }
    else
    {
        pollingInterval = DHT_POLLING_INTERVAL_DHT22;
    }

    //Если интервал маленький, то возврат последнего удачного значения
    if ((Timer::TimeMS() - sensor->lastPollingTime < pollingInterval) && sensor->lastPollingTime != 0)
    {
        data.hum = sensor->lastHum;
        data.temp = sensor->lastTemp;
        return data;
    }
    sensor->lastPollingTime = Timer::TimeMS() + 1;
#endif

    /* Запрос данных у датчика */
    //Перевод пина "на выход"
    goToOutput(sensor);
    //Опускание линии данных на 18 мс
    lineDown();
    Timer::DelayMS(18);
    //Подъём линии, перевод порта "на вход"
    lineUp();
    goToInput(sensor);


#ifdef DHT_IRQ_CONTROL
    //Выключение прерываний, чтобы ничто не мешало обработке данных
    __disable_irq();
#endif
    /* Ожидание ответа от датчика */
    uint16_t timeout = 0;
    //Ожидание спада
    while (getLine())
    {
        timeout++;
        if (timeout > DHT_TIMEOUT)
        {
#ifdef DHT_IRQ_CONTROL
            __enable_irq();
#endif
            //Если датчик не отозвался, значит его точно нет
            //Обнуление последнего удачного значения, чтобы
            //не получать фантомные значения
            sensor->lastHum = -128.0f;
            sensor->lastTemp = -128.0f;

            return data;
        }
    }

    timeout = 0;

    //Ожидание подъёма
    while (!getLine())
    {
        timeout++;
        if (timeout > DHT_TIMEOUT)
        {
#ifdef DHT_IRQ_CONTROL
            __enable_irq();
#endif
            //Если датчик не отозвался, значит его точно нет
            //Обнуление последнего удачного значения, чтобы
            //не получать фантомные значения
            sensor->lastHum = -128.0f;
            sensor->lastTemp = -128.0f;

            return data;
        }
    }

    timeout = 0;

    //Ожидание спада
    while (getLine())
    {
        timeout++;
        if (timeout > DHT_TIMEOUT)
        {
#ifdef DHT_IRQ_CONTROL
            __enable_irq();
#endif
            return data;
        }
    }

    /* Чтение ответа от датчика */
    uint8_t rawData[5] = { 0,0,0,0,0 };
    for (uint8_t a = 0; a < 5; a++)
    {
        for (uint8_t b = 7; b != 255; b--)
        {
            uint16_t hT = 0, lT = 0;
            //Пока линия в низком уровне, инкремент переменной lT
            while (!getLine() && lT != 65535) lT++;
            //Пока линия в высоком уровне, инкремент переменной hT
            timeout = 0;
            while (getLine() && hT != 65535) hT++;
            //Если hT больше lT, то пришла единица
            if (hT > lT) rawData[a] |= (1 << b);
        }
    }

#ifdef DHT_IRQ_CONTROL
    //Включение прерываний после приёма данных
    __enable_irq();
#endif

    /* Проверка целостности данных */
    if ((uint8_t)(rawData[0] + rawData[1] + rawData[2] + rawData[3]) == rawData[4])
    {
        //Если контрольная сумма совпадает, то конвертация и возврат полученных значений
        if (sensor->type == DHT22)
        {
            data.hum = (float)(((uint16_t)rawData[0] << 8) | rawData[1]) * 0.1f;
            //Проверка на отрицательность температуры
            if (!(rawData[2] & (1 << 7)))
            {
                data.temp = (float)(((uint16_t)rawData[2] << 8) | rawData[3]) * 0.1f;
            }
            else
            {
                rawData[2] &= ~(1 << 7);
                data.temp = (float)(((uint16_t)rawData[2] << 8) | rawData[3]) * -0.1f;
            }
        }

        if (sensor->type == DHT11)
        {
            data.hum = (float)rawData[0];
            data.temp = (float)rawData[2];
        }
    }

#if DHT_POLLING_CONTROL == 1
    sensor->lastHum = data.hum;
    sensor->lastTemp = data.temp;
#endif

    return data;
}
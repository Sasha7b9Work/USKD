#include "defines.h"
#include "DHT22.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


#define lineDown()      GPIO_BC(sensor->DHT_Port) = sensor->DHT_Pin
#define lineUp()        GPIO_BOP(sensor->DHT_Port) = sensor->DHT_Pin
#define getLine()       (gpio_input_bit_get(sensor->DHT_Port, sensor->DHT_Pin) != RESET)

// Настройки
#define DHT_TIMEOUT                 10000   // Количество итераций, после которых функция вернёт пустые значения
#define DHT_POLLING_CONTROL         1       // Включение проверки частоты опроса датчика
#define DHT_POLLING_INTERVAL_DHT11  2000    // Интервал опроса DHT11 (0.5 Гц по даташиту). Можно поставить 1500, будет работать
#define DHT_POLLING_INTERVAL_DHT22  1000    // Интервал опроса DHT22 (1 Гц по даташиту)
#define DHT_IRQ_CONTROL                     // Выключать прерывания во время обмена данных с датчиком


// Структура возвращаемых датчиком данных
struct DHT_data
{
    float hum;
    float temp;
};


// Тип используемого датчика
enum DHT_type
{
    DHT11,
    DHT22
};


// Структура объекта датчика
struct DHT_sensor
{
    uint DHT_Port;              // Порт датчика (GPIOA, GPIOB, etc)
    uint DHT_Pin;               // Номер пина датчика (GPIO_PIN_0, GPIO_PIN_1, etc)
    DHT_type type;              // Тип датчика (DHT11 или DHT22)
    uint8 pullUp;               // Нужна ли подтяжка линии данных к питанию (GPIO_NOPULL - нет, GPIO_PULLUP - да)

    //Контроль частоты опроса датчика. Значения не заполнять!
#if DHT_POLLING_CONTROL == 1
    uint lastPollingTime;       // Время последнего опроса датчика
    float lastTemp;             // Последнее значение температуры
    float lastHum;              // Последнее значение влажности
#endif
};


// Прототипы функций
DHT_data DHT_getData(DHT_sensor *sensor); // Получить данные с датчика

static void goToOutput(DHT_sensor *sensor)
{
    lineUp();                   //По умолчанию на линии высокий уровень

    gpio_init(sensor->DHT_Port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, sensor->DHT_Pin);
}


static void goToInput(DHT_sensor *sensor)
{
    gpio_init(sensor->DHT_Port, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, sensor->DHT_Pin);
}


namespace DHT
{
    static float temperature = 0.0f;
    static float humidity = 0.0f;
}


float DHT::GetTemperature()
{
    return temperature;
}


float DHT::GetHumidity()
{
    return humidity;
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
        for (uint8_t b = 7; b != 255; b--) //-V621
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

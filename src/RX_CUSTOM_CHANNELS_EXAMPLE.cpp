/**
 * @file RX_CUSTOM_CHANNELS_EXAMPLE.cpp
 * @brief Пример использования кастомных данных на стороне приемника (RX)
 * 
 * На приемнике можно читать текстовые данные с каналов 10-15
 * и использовать их для любых целей (логирование, идентификация и т.д.)
 */

#include "common.h"
#include "logging.h"

// Глобальный буфер для кэширования значений каналов
static uint32_t lastChannelValues[16] = {0};

/**
 * @brief Функция для декодирования CRSF значения обратно в приблизительный текст
 * @param crsfValue 11-битное значение CRSF (0-2047)
 * @return Примерное ASCII значение, закодированное в это значение
 */
static char decodeCRSFValueToChar(uint32_t crsfValue)
{
    // Обратное преобразование:
    // CRSF_value = NEUTRAL + ((ASCII_value - 79) * 7)
    // ASCII_value = ((CRSF_value - NEUTRAL) / 7) + 79
    
    const int NEUTRAL = 992;
    int asciiValue = ((int)crsfValue - NEUTRAL) / 7 + 79;
    
    // Ограничиваем диапазон печатными символами ASCII
    if (asciiValue < 32)
        asciiValue = 32;
    if (asciiValue > 126)
        asciiValue = 126;
    
    return (char)asciiValue;
}

/**
 * @brief Обработать полученные пользовательские данные с каналов 10-15
 * Эта функция должна вызваться периодически в main loop RX модуля
 */
void handleCustomChannelData()
{
    // Проверяем каналы 10-15 на изменения
    for (int ch = 10; ch <= 15; ch++)
    {
        if (ChannelData[ch] != lastChannelValues[ch])
        {
            // Значение изменилось!
            lastChannelValues[ch] = ChannelData[ch];
            
            // Декодируем значение
            char decodedChar = decodeCRSFValueToChar(ChannelData[ch]);
            
            DBGVLN("[CUSTOM_CH] Канал %d изменился: %lu -> '%c'", 
                    ch, ChannelData[ch], decodedChar);
            
            // Здесь можно добавить любую логику обработки данных
            // Например:
            // - Логирование в EEPROM
            // - Передача на USB/UART для отладки
            // - Использование для управления LED/звука
            // - Отправка на FC в качестве телеметрии
            
            switch (ch)
            {
                case 10:  // "MY"
                    DBGLN("Получено: MY");
                    // Ваша логика здесь
                    break;
                    
                case 11:  // "NAME"
                    DBGLN("Получено: NAME");
                    break;
                    
                case 12:  // "IS"
                    DBGLN("Получено: IS");
                    break;
                    
                case 13:  // "ARTEM"
                    DBGLN("Получено: ARTEM");
                    break;
                    
                case 14:  // "I"
                    DBGLN("Получено: I");
                    break;
                    
                case 15:  // "AM"
                    DBGLN("Получено: AM");
                    break;
                    
                case 16:  // "UKRAINIAN"
                    DBGLN("Получено: UKRAINIAN");
                    break;
            }
        }
    }
}

/**
 * @brief Инициализация системы обработки кастомных данных
 * Должна вызваться один раз при запуске RX
 */
void initCustomChannelDataHandler()
{
    // Инициализируем буфер кэша
    for (int i = 0; i < 16; i++)
    {
        lastChannelValues[i] = 0xFFFFFFFF;  // Никогда не совпадет при первом чтении
    }
    
    DBGLN("[CUSTOM_CH] Обработчик кастомных данных инициализирован");
}

/**
 * @brief Получить строку со всеми текущими кастомными данными
 * @param buffer указатель на буфер для строки
 * @param bufsize размер буфера
 * @return длина строки
 */
int getCustomChannelDataAsString(char* buffer, int bufsize)
{
    int pos = 0;
    
    // Получаем приблизительное текстовое представление
    const char* textLabels[] = {"MY", "NAME", "IS", "ARTEM", "I", "AM", "UKRAINIAN"};
    
    for (int i = 0; i < 7 && pos < bufsize - 10; i++)
    {
        char decodedChar = decodeCRSFValueToChar(ChannelData[10 + i]);
        pos += snprintf(buffer + pos, bufsize - pos, "CH%d(%s)=%lu ", 
                       10 + i, textLabels[i], ChannelData[10 + i]);
    }
    
    return pos;
}

/**
 * @brief Отправить кастомные данные на FC через MSP/CRSF
 * Пример отправки данных на полетный контроллер
 */
void sendCustomDataToFC()
{
    // Можно создать пакет и отправить на FC
    // Например, упаковать значения каналов 10-15 в специальный пакет
    
    static uint32_t lastSendTime = 0;
    uint32_t now = millis();
    
    // Отправляем каждые 10 секунд
    if (now - lastSendTime >= 10000)
    {
        lastSendTime = now;
        
        char buffer[128];
        getCustomChannelDataAsString(buffer, sizeof(buffer));
        
        DBGVLN("[CUSTOM_CH_TO_FC] %s", buffer);
        
        // Здесь можно добавить код отправки на FC
        // Например, через MSP протокол или CRSF
    }
}

/**
 * @brief Пример полной интеграции в RX main loop
 * 
 * Добавьте этот вызов в rx_main.cpp в функцию loop():
 * 
 * void loop() {
 *     // ... существующий код ...
 *     
 *     // Обработать кастомные данные каналов 10-15
 *     handleCustomChannelData();
 *     sendCustomDataToFC();
 *     
 *     // ... остальной код ...
 * }
 */

/**
 * @brief Пример использования значений отдельных каналов
 * 
 * // Получить значение канала 10
 * uint32_t ch10_value = ChannelData[10];
 * 
 * // Если значение != 0, значит данные получены
 * if (ch10_value > 0) {
 *     // Обработать данные
 *     char approximateChar = decodeCRSFValueToChar(ch10_value);
 *     
 *     // Использовать для логики
 *     if (approximateChar == 'M' || approximateChar == 'N') {
 *         // Выполнить какое-то действие
 *     }
 * }
 */

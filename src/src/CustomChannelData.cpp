#include "CustomChannelData.h"
#include "logging.h"

// CRSF константы для значений каналов
// CRSF_CHANNEL_VALUE_MIN = 191 (988 µs)
// CRSF_CHANNEL_VALUE_MAX = 1791 (2012 µs)
// Нейтральное значение = 992 (1500 µs)
#define CRSF_NEUTRAL 992
#define CRSF_MIN 191
#define CRSF_MAX 1791

uint32_t CustomChannelData::lastUpdateTime = 0;
uint8_t CustomChannelData::currentTextIndex = 0;

void CustomChannelData::init()
{
    lastUpdateTime = 0;
    currentTextIndex = 0;
    DBGLN("CustomChannelData инициализирован - отправка текста на каналы 10-15 каждые 5 секунд");
}

bool CustomChannelData::update(uint32_t now)
{
    // Проверяем прошло ли 5 секунд с последнего обновления
    if (now - lastUpdateTime >= INTERVAL_MS)
    {
        lastUpdateTime = now;
        DBGVLN("CustomChannelData: отправка данных (время=%lu)", now);
        return true;
    }
    return false;
}

uint32_t CustomChannelData::charToCRSFValue(char c)
{
    // Кодируем одиночный символ в CRSF значение (0-2047)
    // Берем ASCII код символа и масштабируем его в диапазон CRSF
    uint32_t val = (uint8_t)c;
    
    // Отображаем печатные ASCII символы (32-126) в полный диапазон CRSF
    // Формула: NEUTRAL + ((char - 79) * 7) дает примерно 191-1791
    return CRSF_NEUTRAL + ((val - 79) * 7);
}

uint32_t CustomChannelData::textToCRSFValue(const char* text)
{
    if (!text || *text == '\0')
        return CRSF_NEUTRAL;  // Нейтральное значение для пустого текста
    
    // Кодируем текст в одно 11-битное значение
    // Суммируем ASCII коды всех символов и нормализуем
    uint32_t value = 0;
    int length = 0;
    
    for (const char* p = text; *p != '\0' && length < 8; p++, length++)
    {
        value += (uint8_t)*p;
    }
    
    // Среднее значение символа
    if (length > 0)
    {
        value = value / length;  // Получаем примерное ASCII значение
    }
    
    // Масштабируем в CRSF диапазон
    return charToCRSFValue((char)value);
}

void CustomChannelData::applyToChannels(uint32_t *channelData)
{
    // Данные текста для отправки на каналы 10-15
    const char* textData[] = {
        "MY",
        "NAME",
        "IS",
        "ARTEM",
        "I",
        "AM",
        "UKRAINIAN"
    };
    
    // Устанавливаем значения для каналов 10-16
    // Каждый канал содержит кодированное значение одного текстового поля
    for (int i = 0; i < 7 && (START_CHANNEL + i) < 16; i++)
    {
        // Кодируем текст в CRSF значение
        uint32_t channelValue = textToCRSFValue(textData[i]);
        
        // Убеждаемся, что значение в диапазоне CRSF (0-2047)
        if (channelValue > 2047)
            channelValue = 2047;
        if (channelValue < 0)
            channelValue = 0;
        
        // Применяем к каналу
        channelData[START_CHANNEL + i] = channelValue;
        
        DBGVLN("CH%d='%s' -> value=%lu", START_CHANNEL + i, textData[i], channelValue);
    }
    
    DBGVLN("CustomChannelData::applyToChannels - данные применены");
}

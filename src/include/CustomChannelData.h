#pragma once

#include <cstdint>
#include <cstring>

/**
 * @brief Модуль для отправки пользовательских данных на каналы 10-15
 * 
 * Этот модуль позволяет отправлять текстовые данные на зарезервированные каналы 10-15
 * через CRSF протокол с интервалом 5 секунд.
 * 
 * Текстовые данные кодируются в 11-битные значения каналов CRSF:
 * - Диапазон: 0-2047
 * - Нейтральное: 992 (соответствует 1500 µs)
 * - Минимальное: 191 (соответствует 988 µs)
 * - Максимальное: 1791 (соответствует 2012 µs)
 * 
 * Отправляемые данные:
 * CH10: "MY"
 * CH11: "NAME"
 * CH12: "IS"
 * CH13: "ARTEM"
 * CH14: "I"
 * CH15: "AM"
 * CH16: "UKRAINIAN"
 */
class CustomChannelData {
public:
    /**
     * @brief Инициализация модуля кастомных данных каналов
     * Должна быть вызвана один раз при запуске системы
     */
    static void init();
    
    /**
     * @brief Функция обновления - должна вызываться из основного цикла loop()
     * @param now текущее время в миллисекундах (от millis())
     * @return true если прошло 5 секунд и нужно обновить данные, false в противном случае
     */
    static bool update(uint32_t now);
    
    /**
     * @brief Применить пользовательские данные к массиву каналов
     * @param channelData указатель на массив CRSF_NUM_CHANNELS (16) элементов
     * 
     * Модифицирует каналы 10-15 с кодированными текстовыми данными
     */
    static void applyToChannels(uint32_t *channelData);
    
private:
    // Текстовые строки для отправки на каналы 10-15
    static constexpr const char* customText[] = {
        "MY",
        "NAME", 
        "IS",
        "ARTEM",
        "I",
        "AM",
        "UKRAINIAN"
    };
    
    // Интервал отправки данных в миллисекундах
    static constexpr uint32_t INTERVAL_MS = 5000;
    
    // Параметры каналов
    static constexpr uint8_t NUM_CHANNELS = 7;
    static constexpr uint8_t START_CHANNEL = 10;
    
    // Переменные состояния
    static uint32_t lastUpdateTime;
    static uint8_t currentTextIndex;
    
    /**
     * @brief Кодировать один символ в CRSF значение канала
     * @param c символ ASCII для кодирования
     * @return 11-битное значение CRSF (0-2047)
     */
    static uint32_t charToCRSFValue(char c);
    
    /**
     * @brief Кодировать текстовую строку в CRSF значение канала
     * @param text строка для кодирования
     * @return 11-битное значение CRSF (0-2047), полученное на основе текста
     * 
     * Метод: усредненное ASCII значение символов масштабируется в CRSF диапазон
     */
    static uint32_t textToCRSFValue(const char* text);
};


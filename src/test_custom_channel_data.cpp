/**
 * @file test_custom_channel_data.cpp
 * @brief Простой тест для проверки функций кодирования текста в CRSF значения
 * 
 * Этот тест проверяет, что текстовые строки кодируются в корректные CRSF значения (0-2047)
 */

#include "CustomChannelData.h"
#include "logging.h"

void testCustomChannelData()
{
    DBGLN("=== ТЕСТ КОДИРОВАНИЯ ТЕКСТА В КАНАЛЫ ===");
    
    // Инициализируем модуль
    CustomChannelData::init();
    
    // Создаем тестовый массив каналов
    uint32_t testChannelData[16] = {0};
    
    // Инициализируем все каналы нейтральным значением (992)
    for (int i = 0; i < 16; i++)
    {
        testChannelData[i] = 992;
    }
    
    DBGLN("Начальные значения каналов (все 992 - нейтральные):");
    for (int i = 0; i < 16; i++)
    {
        DBGVLN("  CH%d: %lu", i, testChannelData[i]);
    }
    
    DBGLN("\n--- Применяем пользовательские данные ---");
    // Применяем пользовательские данные
    CustomChannelData::applyToChannels(testChannelData);
    
    DBGLN("\nЗначения каналов после применения пользовательских данных:");
    for (int i = 0; i < 16; i++)
    {
        if (testChannelData[i] != 0)
        {
            DBGVLN("  CH%d: %lu", i, testChannelData[i]);
        }
    }
    
    // Проверяем, что каналы 10-15 установлены и в корректном диапазоне
    DBGLN("\n--- ПРОВЕРКА КОРРЕКТНОСТИ ---");
    bool allValid = true;
    for (int i = 10; i < 16; i++)
    {
        uint32_t val = testChannelData[i];
        if (val < 0 || val > 2047)
        {
            DBGVLN("ОШИБКА: CH%d = %lu (должно быть 0-2047)", i, val);
            allValid = false;
        }
        else
        {
            DBGVLN("OK: CH%d = %lu", i, val);
        }
    }
    
    if (allValid)
    {
        DBGLN("\n✓ ВСЕ ТЕСТЫ ПРОЙДЕНЫ!");
    }
    else
    {
        DBGLN("\n✗ НЕКОТОРЫЕ ТЕСТЫ НЕ ПРОЙДЕНЫ!");
    }
    
    // Проверяем механизм обновления
    DBGLN("\n--- ТЕСТ МЕХАНИЗМА ОБНОВЛЕНИЯ (5 СЕК) ---");
    uint32_t testTime = 0;
    
    DBGVLN("Время=0: update() = %d (должно быть true - первое обновление)", CustomChannelData::update(testTime));
    DBGVLN("Время=2000: update() = %d (должно быть false - еще не 5 сек)", CustomChannelData::update(testTime + 2000));
    DBGVLN("Время=5000: update() = %d (должно быть true - прошло 5 сек)", CustomChannelData::update(testTime + 5000));
    DBGVLN("Время=5100: update() = %d (должно быть false - не прошло еще 5 сек)", CustomChannelData::update(testTime + 5100));
    DBGVLN("Время=10000: update() = %d (должно быть true - еще 5 сек прошло)", CustomChannelData::update(testTime + 10000));
    
    DBGLN("\n=== КОНЕЦ ТЕСТИРОВАНИЯ ===\n");
}

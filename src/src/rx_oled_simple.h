#pragma once

#include <U8g2lib.h>
#include <Wire.h>
#include "rxtx_common.h"

#define TTGO_V2_OLED_SDA_PIN 21
#define TTGO_V2_OLED_SCL_PIN 22

// Forward declaration
struct TxInfo;

class RX_OLED_Display {
private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    bool initialized = false;

public:
    RX_OLED_Display() : u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/ 21) {}

    void init() {
        Wire.begin(TTGO_V2_OLED_SDA_PIN, TTGO_V2_OLED_SCL_PIN);
        u8g2.begin();
        u8g2.setFont(u8g2_font_6x10_tf);
        
        // Заставка
        u8g2.clearBuffer();
        u8g2.drawStr(0, 12, "ExpressLRS RX");
        u8g2.drawStr(0, 24, "Initializing...");
        u8g2.sendBuffer();
        
        initialized = true;
        Serial.println("RX OLED display initialized");
    }

    void update(connectionState_e state, TxInfo* txArray = nullptr, uint8_t currentTx = 0) {
        if (!initialized) return;

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_6x10_tf);
        
        if (state == connected && txArray != nullptr) {
            // Подключен - показываем информацию о всех TX
            char buf[32];
            
            // Строка 1: Статус подключения
            snprintf(buf, sizeof(buf), "Connected to TX%u", currentTx + 1);
            u8g2.drawStr(0, 10, buf);
            
            // Строка 2-4: RSSI для каждого TX
            for (uint8_t i = 0; i < 3; i++) {
                if (txArray[i].active) {
                    snprintf(buf, sizeof(buf), "TX%u: %d dBm", i+1, -(int)txArray[i].rssi_1);
                } else {
                    snprintf(buf, sizeof(buf), "TX%u: ---", i+1);
                }
                u8g2.drawStr(0, 24 + (i * 12), buf);
            }
            
            // Строка 5: LQ и SNR
            snprintf(buf, sizeof(buf), "LQ:%u SNR:%d", linkStats.uplink_Link_quality, linkStats.uplink_SNR);
            u8g2.drawStr(0, 60, buf);
        } else {
            // Не подключен
            char buf[32];
            
            // Строка 1: UID
            const uint8_t* uid = config.GetUID();
            snprintf(buf, sizeof(buf), "UID:%02X%02X%02X%02X%02X%02X", 
                     uid[0], uid[1], uid[2], uid[3], uid[4], uid[5]);
            u8g2.drawStr(0, 12, buf);
            
            // Строка 2
            u8g2.drawStr(0, 30, "Searching...");
            
            // Строка 3: Показываем последние RSSI если есть данные
            if (txArray != nullptr) {
                snprintf(buf, sizeof(buf), "Last TX: %u", currentTx + 1);
                u8g2.drawStr(0, 42, buf);
            }
            
            // Строка 4: Статус бинда
            if (InBindingMode) {
                u8g2.drawStr(0, 54, "BIND: ACTIVE");
            } else {
                u8g2.drawStr(0, 54, "BIND: OFF");
            }
        }
        
        u8g2.sendBuffer();
    }

private:
    const char* getFreqDomainStr() {
        #if defined(Regulatory_Domain_FCC_915)
            return "FCC_915";
        #elif defined(Regulatory_Domain_EU_868)
            return "EU_868";
        #elif defined(Regulatory_Domain_ISM_2400)
            return "2.4GHz";
        #else
            return "Unknown";
        #endif
    }
};

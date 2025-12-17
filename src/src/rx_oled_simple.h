#pragma once

#include <U8g2lib.h>
#include <Wire.h>
#include "rxtx_common.h"

#define TTGO_V2_OLED_SDA_PIN 21
#define TTGO_V2_OLED_SCL_PIN 22

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

    void update(connectionState_e state) {
        if (!initialized) return;

        u8g2.clearBuffer();
        
        if (state == connected) {
            // Подключен - показываем телеметрию
            u8g2.drawStr(0, 12, "Connected");
            
            char buf[32];
            snprintf(buf, sizeof(buf), "RSSI1:%d", -(int)linkStats.uplink_RSSI_1);
            u8g2.drawStr(0, 24, buf);
            
            snprintf(buf, sizeof(buf), "RSSI2:%d", -(int)linkStats.uplink_RSSI_2);
            u8g2.drawStr(0, 36, buf);
            
            snprintf(buf, sizeof(buf), "LQ:%u SNR:%d", linkStats.uplink_Link_quality, linkStats.uplink_SNR);
            u8g2.drawStr(0, 48, buf);
            
            snprintf(buf, sizeof(buf), "Ant:%u Pwr:%u", linkStats.active_antenna, linkStats.uplink_TX_Power);
            u8g2.drawStr(0, 60, buf);
        } else {
            // Не подключен
            u8g2.drawStr(0, 12, "ExpressLRS RX");
            u8g2.drawStr(0, 30, "Searching...");
            
            char buf[32];
            snprintf(buf, sizeof(buf), "Mode: %s", getFreqDomainStr());
            u8g2.drawStr(0, 48, buf);
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

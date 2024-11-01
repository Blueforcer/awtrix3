#include <Arduino.h>
#include <rom/rtc.h>
#include "reset.h"

String ESP32GetResetReason(uint32_t cpu_no) {
        // tools\sdk\include\esp32\rom\rtc.h
        // tools\sdk\esp32\include\esp_rom\include\esp32c3\rom\rtc.h
        // tools\sdk\esp32\include\esp_rom\include\esp32s2\rom\rtc.h
  switch (rtc_get_reset_reason(cpu_no)) {                                   //     ESP32             ESP32-S / ESP32-C
    case 1  : return F("Vbat power on reset");                              // 1   POWERON_RESET     POWERON_RESET
    case 3  : return F("Software reset digital core");                      // 3   SW_RESET          RTC_SW_SYS_RESET
    case 4  : return F("Legacy watch dog reset digital core");              // 4   OWDT_RESET        -
    case 5  : return F("Deep Sleep reset digital core");                    // 5   DEEPSLEEP_RESET   DEEPSLEEP_RESET
    case 6  : return F("Reset by SLC module, reset digital core");          // 6   SDIO_RESET
    case 7  : return F("Timer Group0 Watch dog reset digital core");        // 7   TG0WDT_SYS_RESET
    case 8  : return F("Timer Group1 Watch dog reset digital core");        // 8   TG1WDT_SYS_RESET
    case 9  : return F("RTC Watch dog Reset digital core");                 // 9   RTCWDT_SYS_RESET
    case 10 : return F("Instrusion tested to reset CPU");                   // 10  INTRUSION_RESET
    case 11 : return F("Time Group0 reset CPU");                            // 11  TGWDT_CPU_RESET   TG0WDT_CPU_RESET
    case 12 : return F("Software reset CPU");                               // 12  SW_CPU_RESET      RTC_SW_CPU_RESET
    case 13 : return F("RTC Watch dog Reset CPU");                          // 13  RTCWDT_CPU_RESET
    case 14 : return F("or APP CPU, reseted by PRO CPU");                   // 14  EXT_CPU_RESET     -
    case 15 : return F("Reset when the vdd voltage is not stable");         // 15  RTCWDT_BROWN_OUT_RESET
    case 16 : return F("RTC Watch dog reset digital core and rtc module");  // 16  RTCWDT_RTC_RESET
    case 17 : return F("Time Group1 reset CPU");                            // 17  -                 TG1WDT_CPU_RESET
    case 18 : return F("Super watchdog reset digital core and rtc module"); // 18  -                 SUPER_WDT_RESET
    case 19 : return F("Glitch reset digital core and rtc module");         // 19  -                 GLITCH_RTC_RESET
    case 20 : return F("Efuse reset digital core");                         // 20                    EFUSE_RESET
    case 21 : return F("Usb uart reset digital core");                      // 21                    USB_UART_CHIP_RESET
    case 22 : return F("Usb jtag reset digital core");                      // 22                    USB_JTAG_CHIP_RESET
    case 23 : return F("Power glitch reset digital core and rtc module");   // 23                    POWER_GLITCH_RESET
  }

  return F("No meaning");                                                   // 0 and undefined
}

String ESP_getResetReason(void) {
  return ESP32GetResetReason(0);  // CPU 0
}


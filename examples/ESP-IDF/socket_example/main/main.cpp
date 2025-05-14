#include <stdio.h>
#include <esp_log.h>
#include <KastaarModem.hpp>
extern "C" void app_main(void)
{
    ESP_LOGI("Kastaar","Hello world");
    KastaarModem::getModule()->connect()
}
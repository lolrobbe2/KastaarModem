#include <stdio.h>
#include <esp_log.h>
#include <KastaarModem.hpp>
#include <proto/socket/Socket.hpp>
#include <esp_mac.h>

struct PingPacket
{
    uint8_t macAddress[6];
    uint8_t counterLow = 0;
    uint8_t counterHigh = 0;
};

const char* TAG = "Non volatile example";
using Socket = kastaarModem::socket::Socket;
KastaarModem modem;
Socket socket;
PingPacket packet;
uint16_t counter = 0;
extern "C" void app_main(void)
{
    esp_err_t err = esp_read_mac(packet.macAddress, ESP_MAC_WIFI_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read MAC address! Error code: %d", err);
    } else {
        ESP_LOGI(TAG, "MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
                packet.macAddress[0], packet.macAddress[1], packet.macAddress[2],
                packet.macAddress[3], packet.macAddress[4],
                packet.macAddress[5]);
    }

    modem.init("soracom.io",DEFAULT_CONFIG);

    if(modem.connect() == esp_modem::command_result::FAIL){
        ESP_LOGE(TAG, "could not connect the modem");
    } else {
        ESP_LOGI(TAG, "connected to network succesfully");
    }

    if(socket.config() == esp_modem::command_result::OK) {
        ESP_LOGI(TAG, "configured succesfully");
    } else {
        ESP_LOGE(TAG, "could not configure the socket");
    }

    if (socket.dial("walterdemo.quickspot.io",1999,kastaarModem::socket::UDP) == esp_modem::command_result::OK) {
        ESP_LOGI(TAG, "connected  to demo server");
    } else {
        ESP_LOGE(TAG, "could not conect to the demo server");
    }
    
    for (;;)
    {
        if (socket.sendMinimal(packet) != esp_modem::command_result::OK) {
            ESP_LOGE(TAG, "Failed to send packet. Attempt #%d", counter);
        } else {
            ESP_LOGI(TAG, "Packet sent successfully. Counter: %d", counter);
        }
        packet.counterLow = counter >> 8;
        packet.counterHigh = counter & 0xFF;
        counter++;
        vTaskDelay(pdMS_TO_TICKS(5000)); // Optional delay to avoid spamming
    }

}
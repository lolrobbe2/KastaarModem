#include "esp_timer.h" // Required for esp_timer_get_time
#include <KastaarModem.hpp>
#include <esp_log.h>
#include <esp_mac.h>
#include <proto/socket/Socket.hpp>
#include <stdio.h>

KastaarModem modem;
Socket socket;

extern "C" void app_main(void)
{
    modem.init("soracom.io",DEFAULT_CONFIG);

    if(modem.connect() == esp_modem::command_result::FAIL){
        ESP_LOGE("SocketExample", "could not connect the modem");
    } else {
        ESP_LOGI("SocketExample", "connected to network succesfully");
    }

    if(socket.config() == esp_modem::command_result::OK) {
        ESP_LOGI("SocketExample", "configured succesfully");
    } else {
        ESP_LOGE("SocketExample", "could not configure the socket");
    }

    if (socket.dial("example.com",80,kastaarModem::socket::TCP) == esp_modem::command_result::OK) {
        ESP_LOGI("SocketExample", "connected  to demo server");
    } else {
        ESP_LOGE("SocketExample", "could not conect to the demo server");
    }
    
    for (;;)
    {
        std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
        if (socket.sendMinimal(request) != esp_modem::command_result::OK) {
            ESP_LOGE("SocketExample", "failed to querry example.com");
        } else {
            ESP_LOGI("SocketExample", "succesfully querried example.com");
        }
      
        vTaskDelay(pdMS_TO_TICKS(5000)); /* delay to avoid spamming the server */
        std::array<uint8_t, 3000> dataBuffer;

        while (socket.available())
        {
            
            uint32_t received;
            /*
            int64_t startTime = esp_timer_get_time(); // Time before receive
            */
            socket.receive(dataBuffer,3000,received);
            /*
            int64_t endTime = esp_timer_get_time(); // Time after receive
            int64_t durationMs = (endTime - startTime) / 1000;

            ESP_LOGI("Socket", "Received %" PRId32" bytes in %" PRId64 " ms", received, durationMs);
            */
            ESP_LOGI("Socket", "Line: %.*s", static_cast<int>(received),dataBuffer.data());
        }
    }
}
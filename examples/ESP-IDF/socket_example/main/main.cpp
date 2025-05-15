#include <stdio.h>
#include <esp_log.h>
#include <KastaarModem.hpp>
#include <proto/socket/Socket.hpp>

KastaarModem modem;
kastaarModem::socket::Socket socket;
extern "C" void app_main(void)
{
    ESP_LOGI("Kastaar","Hello world");
    modem.init("soracom.io",DEFAULT_CONFIG);
    if(modem.connect() == esp_modem::command_result::FAIL){
        ESP_LOGE("SocketExample", "could not connect the modem");
    } else {
        ESP_LOGI("SocketExample", "connected to network succesfully");
    }

    if(socket.config() == esp_modem::command_result::OK) {
        ESP_LOGI("SocketExample", "connected config");
    } else {
        ESP_LOGE("SocketExample", "could not configure the socket");
    }
}
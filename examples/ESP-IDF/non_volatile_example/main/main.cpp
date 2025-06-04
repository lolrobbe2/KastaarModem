#include <stdio.h>
#include <esp_log.h>
#include <KastaarModem.hpp>
#include <tls/NonVolatile.hpp>
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

extern "C" void app_main(void)
{
    modem.init("soracom.io",DEFAULT_CONFIG);

    
}
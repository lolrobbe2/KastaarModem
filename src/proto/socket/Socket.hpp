#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <cxx_include/esp_modem_types.hpp>
namespace kastaarModem::socket 
{
    /**
     * @brief this class represents a modem socket
     */
    class Socket
    {
    public:
        Socket();
        
        ~Socket();
        /*
        * @brief this function configures the socket
        */
        esp_modem::command_result config(
            const uint16_t packetSize = 300,
            const uint16_t exchangeTimeout = 90,
            const uint16_t connectionTimeout = 600,
            const uint16_t transmissionDelay = 50
        );

        /**
         * @brief this returns the Socket Id (1-5) (0 is not initalized)
         */
        constexpr uint8_t getSocketId();
    private:
        bool configured = false;
        bool connected = true;
        uint8_t socketId = 0;
    };
}

#endif
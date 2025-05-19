#include <cstdint>
#include <string_view>
#include "Socket.hpp"

namespace kastaarModem::socket 
{
    /**
     * @brief This class manages routing of URC's and managing the socket instances
     */
    class SocketManager
    {
    public:
        /**
         * @brief This is the socket URC handler
         */
        static void urcHandler(std::string_view line);

    protected:
        friend class Socket;
        friend class KastaarModem;
        
        /** */
        static constexpr uint8_t toIndex(uint8_t socketId);
        static uint8_t getFreeSocket(Socket* socket);
        static void freeSocket(uint8_t socketId);
    private:
        static inline Socket* sockets[5] = { nullptr };
    };
    
   
    
}
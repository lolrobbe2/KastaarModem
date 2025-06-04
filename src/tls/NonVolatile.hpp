#include <sdkconfig.h>
#if CONFIG_KASTAAR_ENABLE_NVM
#ifndef _TLS_MANAGER_HPP_
#define _TLS_MANAGER_HPP_
#include <cstdint>
#include <cxx_include/esp_modem_types.hpp>
#include <span>
#include <string_view>
namespace kastaarModem::tls 
{
    constexpr std::string_view NVM_TAG = "NonVolatilememory";
    class NonVolatileMemory
    {
        public:

            enum DataType {
                RAW_STRING,
                CERTIFICATE,
                PRIVATE_KEY
            };
            /**
             * @brief This function writes data to a slot on the modems Non Volatile Memory
             * 
             * @param [in] data pointer to the start of the data buffer
             * @param [in] size the size of the data buffer to write
             * @param [in] slotIdx the index of the NVM slot to write to
             * @param [in] type The type of data to store in the NVM slot
             */
            static esp_modem::command_result write(const uint8_t* data, const size_t size,const uint8_t slotIdx ,const DataType type);

            /**
             * @brief This function writes data to a slot on the modems Non
             * Volatile Memory
             *
             * @param [in] data string_view containing the data to write.
             * @param [in] slotIdx the index of the NVM slot to write to
             * @param [in] type The type of data to store in the NVM slot
             */
            static esp_modem::command_result write(const std::string_view data,const uint8_t slotIdx,const DataType type);

            static esp_modem::command_result remove(const uint8_t slotIx, const DataType type);
        private:
            inline static std::string getDataTypeString(const DataType type);
    };
    
  
    
}
using NonVolatileMemory = kastaarModem::tls::NonVolatileMemory;

#endif
#endif
#include "NonVolatile.hpp"
#include <esp_log.h>
#include <KastaarModem.hpp>
namespace kastaarModem::tls
{
    esp_modem::command_result NonVolatileMemory::write(const uint8_t *data, const size_t size,const uint8_t slotIdx, const DataType type) {
        if ((slotIdx <= 4) || (slotIdx >= 7 && slotIdx <= 10)) {
            ESP_LOGD(NVM_TAG,"Slot index was reserved (0-4), (7-10)!");
            return esp_modem::command_result::FAIL;
        }
        
        std::string command = "AT+SQNSNVW=\"" + getDataTypeString(type) + "\"," + std::to_string(slotIdx) + "," + std::to_string(size) + "\r\n";

        KastaarModem::command(command, {"OK"}, {"ERROR"}, 5000);

        std::string terminator = "";
        std::string payload(reinterpret_cast<const char *>(data), size);
        return KastaarModem::commandPayload(payload, terminator, 20000);
    }

    esp_modem::command_result NonVolatileMemory::write(const std::string_view data, const uint8_t slotIdx, const DataType type)
    {
        return write((const uint8_t)data.data(),data.size(),slotIdx,type);
    }

    std::string NonVolatileMemory::getDataTypeString(const DataType type)
    {
        switch (type)
        {
        case RAW_STRING:
            return "strid";
        case PRIVATE_KEY:
            return "privateKey";
        case CERTIFICATE:
            return "certificate";
        default:
            return "strid";
        }
    }
}
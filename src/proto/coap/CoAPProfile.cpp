#include "CoAPProfile.hpp"
#include "CoAPManager.hpp"
#include <esp_log.h>
#include <KastaarModem.hpp>
#include <sdkconfig.h>

#if CONFIG_KASTAAR_ENABLE_COAP
namespace kastaarModem::CoAP
{
    CoAPProfile::CoAPProfile() 
    {
        profileId = CoAPManager::getFreeCoAPProfile(this);

        if (profileId == 255) {
            ESP_LOGD(COAP_PROFILE_TAG, "could not find a free profile");
        }
    }
    
    CoAPProfile::~CoAPProfile()
    {
        CoAPManager::freeProfile(profileId);
    }
    esp_modem::command_result CoAPProfile::config(const std::string &addr, const uint16_t port, const TLSProfile *p_profile)
    {
      return KastaarModem::commandCommon(
          "AT+SQNCOAPCREATE=" + std::to_string(profileId) + ",\"" + addr +
              "\"," + std::to_string(port) + ",," +
              std::to_string(p_profile != nullptr) + ",20,1" + (p_profile ? 
              ("," + std::to_string(p_profile->native())) : ""),
          20000);
    }
    esp_modem::command_result CoAPProfile::setToken(const uint64_t token)
    {
        char hexToken[17]; // 16 chars + null terminator
        snprintf(hexToken, sizeof(hexToken), "%016llX", token); // uppercase hex

        std::string tokenStr(hexToken);

        return KastaarModem::commandCommon("AT+SQNCOAPHDR=" + std::to_string(profileId) + ",," + tokenStr, 500);
    }
    esp_modem::command_result CoAPProfile::setMessageId(const uint16_t messageId)
    {
      return KastaarModem::commandCommon(
          "AT+SQNCOAPHDR=" + std::to_string(profileId) + "," + std::to_string(messageId) +",\"NO_TOKEN\"", 500);
    }

    #pragma region SET_OPTIONS
    esp_modem::command_result CoAPProfile::setGetOption(const CoAPOptCode option, const std::initializer_list<std::string> optionValues)
    {
        return setOption(GET,option,optionValues);
    }
    esp_modem::command_result CoAPProfile::setPostOption(const CoAPOptCode option, const std::initializer_list<std::string> optionValues)
    {
        return setOption(POST, option, optionValues);
    }
    esp_modem::command_result CoAPProfile::setPutOption(const CoAPOptCode option, const std::initializer_list<std::string> optionValues)
    {
        return setOption(PUT, option, optionValues);
    }
    esp_modem::command_result CoAPProfile::setDeleteOption(const CoAPOptCode option, const std::initializer_list<std::string> optionValues)
    {
        return setOption(DELETE,option, optionValues);
    }
    esp_modem::command_result CoAPProfile::setOption(const Method method, const CoAPOptCode option, const std::initializer_list<std::string> optionValues)
    {
        if(!(option == IF_MATCH || option == ETAG || option == LOCATION_PATH  || option == URI_PATH || option == URI_QUERY || option == LOCATION_QUERRY) && optionValues.size() > 1)
        {
            ESP_LOGD(COAP_PROFILE_TAG,"The current option is not extendable and cannot have more then 1 option value");
        }

        std::string result;
        for (auto it = optionValues.begin(); it != optionValues.end(); ++it)
        {
            result += *it;
            if (std::next(it) != optionValues.end())
            {
                result += ",";
            }
        }
        return KastaarModem::commandCommon("AT+SQNCOAPOPT=" + std::to_string(profileId) + ",0," + std::to_string(option) + "," + result,500);
    }
    #pragma endregion

    #pragma region SEND

    #pragma region GET
    esp_modem::command_result CoAPProfile::get(uint8_t *data, size_t len)
    {
        return sendMethodData(CON, GET,data,len);
    }
    esp_modem::command_result CoAPProfile::getNon(uint8_t *data, size_t len)
    {
        return sendMethodData(NON, GET, data, len);
    }
#pragma endregion

#pragma region POST
    esp_modem::command_result CoAPProfile::post(uint8_t *data, size_t len) {
        return sendMethodData(CON, POST, data, len);
    }
    esp_modem::command_result CoAPProfile::postNon(uint8_t *data, size_t len) {
        return sendMethodData(NON, POST, data, len);
    }
#pragma endregion

#pragma region PUT
    esp_modem::command_result CoAPProfile::put(uint8_t *data, size_t len) {
        return sendMethodData(CON, PUT, data, len);
    }
    esp_modem::command_result CoAPProfile::putNon(uint8_t *data, size_t len) {
        return sendMethodData(NON, PUT, data, len);
    }
#pragma endregion

#pragma region DELETE
    esp_modem::command_result CoAPProfile::Delete(uint8_t *data, size_t len) {
        return sendMethodData(CON, DELETE, data, len);
    }
    esp_modem::command_result CoAPProfile::deleteNon(uint8_t *data, size_t len) {
        return sendMethodData(NON, DELETE, data, len);
    }
#pragma endregion

#pragma endregion
    esp_modem::command_result CoAPProfile::sendMethodData(const Type sendType, const Method method, uint8_t *data, size_t len)
    {
        std::string payload(reinterpret_cast<const char*>(data), len);

        return KastaarModem::commandCommon("AT+SQNCOAPOPT=" + std::to_string(profileId) + "," + std::to_string(method) + "," + std::to_string(len) + "\r" + payload,20000);
    }
}

#endif
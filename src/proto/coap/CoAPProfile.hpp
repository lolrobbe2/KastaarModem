#include <sdkconfig.h>
#if CONFIG_KASTAAR_ENABLE_COAP
#ifndef _COAP_PROFILE_HPP_
#define _COAP_PROFILE_HPP_
#include <cstdint>
#include <cxx_include/esp_modem_types.hpp>
#include <string_view>
#include <tls/TLSProfile.hpp>
namespace kastaarModem::CoAP
{
    constexpr const char* COAP_PROFILE_TAG = "CoAPProfile";

    enum CoAPOptCode {
        IF_MATCH = 1,
        URI_HOST = 3,
        ETAG = 4,
        IF_NONE_MATCH = 5,
        OBSERVE = 6,
        URI_PORT = 7,
        LOCATION_PATH = 8,
        URI_PATH = 11,
        CONTENT_PATH,
        MAX_AGE = 14,
        URI_QUERY = 15,
        ACCEPT = 17,
        TOKEN = 19,
        LOCATION_QUERRY,
        BLOCK2 = 23,
        BLOCK1 = 27,
        SIZE2 = 28
    };
    enum CoAPResponseCode
    {
        CREATED = 201,
        DELETED = 202,
        VALID = 203,
        CHANGED = 204,
        CONTENT = 205,
        BAD_REQUEST = 400,
        UNAUTHERIZED = 401,
        BAD_OPTION = 402,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        PRECONDITION_FAILED = 412,
        REQUEST_ENTITY_TOO_LARGE = 413,
        UNSUPPORTED_MEDIA_TYPE = 415,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504,
        PROXYING_NOT_SUPPORTED = 505
    };

    enum Method { GET = 1, POST = 2, PUT = 3, DELETE = 4 };
    enum Type {
        CON = 0,
        NON = 1,
        ACK = 2,
        RST = 3
    };

    enum CoAPEventType
    {
        CONNECTED,
        RING,
    };

    struct CoAPRingData
    {
        /* data */
    };
    

    union CoAPEventData
    {
        CoAPRingData ringData;
    };
    
    class CoAPProfile
    {
    public:
        CoAPProfile();
        ~CoAPProfile();

        esp_modem::command_result config(const std::string& addr,const uint16_t port,const TLSProfile* p_profile = nullptr);
        /**
         * @brief this functions set the header token.
         */
        esp_modem::command_result setToken(const uint64_t token);
        /**
         * @brief this functions set the header messageId and disables the TOKEN.
         */
        esp_modem::command_result setMessageId(const uint16_t token);

        #pragma region SET_OPTION
        esp_modem::command_result setGetOption(const CoAPOptCode option,const std::initializer_list<std::string> optionValues);
        esp_modem::command_result setPostOption(const CoAPOptCode option,const std::initializer_list<std::string> optionValues);
        esp_modem::command_result setPutOption(const CoAPOptCode option,const std::initializer_list<std::string> optionValues);
        esp_modem::command_result setDeleteOption(const CoAPOptCode option,const std::initializer_list<std::string> optionValues);
        #pragma endregion

        #pragma region SEND
        #pragma region GET
        esp_modem::command_result get(uint8_t* data, size_t len);
        esp_modem::command_result getNon(uint8_t* data, size_t len);

        template<typename T>
        esp_modem::command_result get(const T& obj);
        template <typename T>
        esp_modem::command_result getNon(const T& obj);
        #pragma endregion

        #pragma region POST
        esp_modem::command_result post(uint8_t* data, size_t len);
        esp_modem::command_result postNon(uint8_t* data, size_t len);

        template <typename T>
        esp_modem::command_result post(const T &obj);
        template <typename T>
        esp_modem::command_result postNon(const T &obj);
        #pragma endregion

        #pragma region PUT
        esp_modem::command_result put(uint8_t* data, size_t len);
        esp_modem::command_result putNon(uint8_t* data, size_t len);

        template <typename T>
        esp_modem::command_result put(const T& obj);
        template <typename T>
        esp_modem::command_result putNon(const T& obj);
        #pragma endregion
        
        #pragma region DELETE
        esp_modem::command_result Delete(uint8_t* data, size_t len);
        esp_modem::command_result deleteNon(uint8_t* data, size_t len);

        template <typename T>
        esp_modem::command_result Delete(const T& obj);
        template <typename T> esp_modem::command_result deleteNon(const T &obj);
#pragma endregion
        #pragma endregion
    private:
        esp_modem::command_result setOption(const Method method,const CoAPOptCode option,const std::initializer_list<std::string> optionValues);
        esp_modem::command_result sendMethodData(const Type sendType, const Method method,uint8_t* data, size_t len);
        uint8_t profileId = 0;
    protected:
        friend class CoAPManager;
        std::function<void(CoAPEventType, CoAPEventData)> eventHandler;
    };


    #pragma region SEND
    #pragma region GET
    template <typename T>
    inline esp_modem::command_result CoAPProfile::get(const T &obj)
    {
        return get((uint8_t*)&obj,sizeof(T));
    }
    template <typename T>
    inline esp_modem::command_result CoAPProfile::getNon(const T &obj)
    {
        return getNon((uint8_t *)&obj, sizeof(T));
    }
    #pragma endregion

    #pragma region POST
    template <typename T>
    inline esp_modem::command_result CoAPProfile::post(const T &obj)
    {
        return post((uint8_t *)&obj, sizeof(T));
    }
    template <typename T>
    inline esp_modem::command_result CoAPProfile::postNon(const T &obj)
    {
        return postNon((uint8_t *)&obj, sizeof(T));
    }
    #pragma endregion

    #pragma region PUT
    template <typename T>
    inline esp_modem::command_result CoAPProfile::put(const T &obj)
    {
        return put((uint8_t *)&obj, sizeof(T));
    }
    template <typename T>
    inline esp_modem::command_result CoAPProfile::putNon(const T &obj)
    {
        return putNon((uint8_t *)&obj, sizeof(T));
    }
    #pragma endregion

    #pragma region DELETE
    template <typename T>
    inline esp_modem::command_result CoAPProfile::Delete(const T &obj)
    {
        return Delete((uint8_t *)&obj, sizeof(T));
    }
    template <typename T>
    inline esp_modem::command_result CoAPProfile::deleteNon(const T &obj)
    {
        return deleteNon((uint8_t *)&obj, sizeof(T));
    }
#pragma endregion
#pragma endregion
}
using CoAPProfile = kastaarModem::CoAP::CoAPProfile;
using CoAPResponseCode = kastaarModem::CoAP::CoAPResponseCode;
using CoAPOptCode = kastaarModem::CoAP::CoAPOptCode;
#endif
#endif
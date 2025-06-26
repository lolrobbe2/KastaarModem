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

    class CoAPProfile
    {
    public:
        CoAPProfile();
        ~CoAPProfile();
        esp_modem::command_result config(const std::string& addr,const uint16_t port,const TLSProfile* p_profile);
    private:
        uint8_t profileId = 0;
    };
}
using CoAPProfile = kastaarModem::CoAP::CoAPProfile;

#endif
#endif
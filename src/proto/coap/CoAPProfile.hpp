#include <sdkconfig.h>
#if !CONFIG_KASTAAR_ENABLE_COAP
#ifndef _COAP_MANAGER_HPP_
#define _COAP_MANAGER_HPP_
#include <cstdint>
#include <string_view>
namespace kastaarModem::CoAP
{
    class CoAPProfile
    {
    public:
        CoAPProfile();
        ~CoAPProfile();
    private:
        uint8_t profileId = 0;
    };
}
#endif
#endif
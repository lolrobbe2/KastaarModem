#include <sdkconfig.h>
#if CONFIG_KASTAAR_ENABLE_COAP
#ifndef _COAP_MANAGER_HPP_
#define _COAP_MANAGER_HPP_

#include <cstdint>
#include <string_view>
#include "CoAPProfile.hpp"
namespace kastaarModem::CoAP
{
    constexpr const char* COAP_MANAGER_TAG = "CoAPManager";

    class CoAPManager
    {
    public:
    protected:
        friend class CoAPProfile;
        static uint8_t getFreeCoAPProfile(CoAPProfile *profile);
        static void freeProfile(uint8_t profileId);
    private:
        static inline CoAPProfile* profiles[3];
    };
}
#endif
#endif
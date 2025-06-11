#include <sdkconfig.h>
#if !CONFIG_KASTAAR_ENABLE_COAP
#ifndef _COAP_MANAGER_HPP_
#define _COAP_MANAGER_HPP_

#include <cstdint>
#include <string_view>
#include "CoAPProfile.hpp"
namespace kastaarModem::CoAP
{
    class CoAPManager
    {
    public:
    protected:
        /** */
        static constexpr uint8_t toIndex(uint8_t profileId);
        static uint8_t getFreeCoAPProfile(CoAPProfile *profile);
        static void freeProfile(uint8_t socketId);
    private:
        static inline CoAPProfile* profiles[3];
    };
}
#endif
#endif
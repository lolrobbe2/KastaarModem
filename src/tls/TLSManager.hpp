#include <sdkconfig.h>
#if !CONFIG_KASTAAR_ENABLE_TLS
#ifndef _TLS_MANAGER_HPP_
#define _TLS_MANAGER_HPP_
#include <cstdint>
#include <string_view>
#include "TLSProfile.hpp"
namespace kastaarModem::tls 
{
    constexpr const char* TLS_TAG = "NonVolatilememory";

    class TLSManager
    {
    public:
        static inline TLSProfile *profiles[5] = {nullptr};
    protected:
        friend class TLSProfile;
        static uint8_t getFreeProfile(TLSProfile *profile);
        static void freeProfile(uint8_t profileId);
    private:
        static constexpr uint8_t toIndex(uint8_t profileId);
    };    
}
#endif
#endif
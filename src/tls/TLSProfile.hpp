#include <sdkconfig.h>
#if CONFIG_KASTAAR_ENABLE_TLS
#ifndef _TLS_PROFILE_HPP_
#define _TLS_PROFILE_HPP_
namespace kastaarModem::tls 
{
    class TLSProfile
    {
    public:
        TLSProfile();
        ~TLSProfile();
    private:
        uint8_t profileId = 0;
    };
}
#endif
#endif
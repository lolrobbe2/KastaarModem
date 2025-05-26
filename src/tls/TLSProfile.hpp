#include <sdkconfig.h>
#if !CONFIG_KASTAAR_ENABLE_TLS
#ifndef _TLS_PROFILE_HPP_
#define _TLS_PROFILE_HPP_
namespace kastaarModem::tls 
{
    class TLSProfile
    {
    private:
        /* data */
    public:
        TLSProfile(/* args */);
        ~TLSProfile();
    };
}
#endif
#endif
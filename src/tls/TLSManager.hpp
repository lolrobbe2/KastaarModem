#include <sdkconfig.h>
#if !CONFIG_KASTAAR_ENABLE_TLS
#ifndef _TLS_MANAGER_HPP_
#define _TLS_MANAGER_HPP_
#include "TLSProfile.hpp"
namespace kastaarModem::tls 
{
    class TlsManager
    {
    private:
        /* data */
    public:
        static inline TLSProfile *profiles[5] = {nullptr};
    };    
}
#endif
#endif
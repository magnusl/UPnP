/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "upnp.h"

using namespace std;

namespace upnp
{
    /**
     * UPnP_Context::GetDevicesWithService
     *          returns a list of all devices that implements services with the specified URN.
     */
    bool UPnP_Context::GetDevicesWithService(const std::string & urn, std::list<uuid_t> & devices)
    {
        lock();
        for(list<UPnP_Device *>::iterator it = m_devices.begin(); it != m_devices.end(); it++)
        {
            /* does the device implement the service */
        }
        unlock();

        return false;
    }
}
/* devices.cpp
 *
 *
 * Copyright (c) 2010-2011 Magnus Leksell, all rights reserved.
 */

#include "upnp.h"
#include "ssdp.h"
#include "uuid.h"

#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <sstream>

/*****************************************************************************/
/*                          FORWARD DECLARATIONS                             */
/*****************************************************************************/

using namespace std;

namespace upnp
{

    UPnP_Device::UPnP_Device()
    {
        m_parent = 0;
    }

    /**
     * UPnP_Device::~UPnP_Device
     */
    UPnP_Device::~UPnP_Device()
    {
        for(list<UPnP_Device *>::iterator it = m_embeddedDevices.begin();
            it != m_embeddedDevices.end();
            it++)
        {
            UPnP_Device * dev = *it;
            if (dev) {
                int res = dev->RefDecrease();
                assert(!res);   /* should not happen when a device is still referenced */
                delete dev;
            }
        }
    }

    /**
     * UPnP_Device::Init
     */
    void UPnP_Device::Init(const ssdp_request_t * request)
    {
        unsigned int addr[4];
        unsigned int port;
        char uri[128];

        m_id = request->uuid;

        const char * str = ssdp_field_contents(request, "location");
        if (str)
            m_location = str;
        /* extract address and port */
        int res = sscanf(str, "http://%d.%d.%d.%d:%d%128s", &addr[0], &addr[1], &addr[2], &addr[3], &port, uri);
        if (res == 6) {
            std::stringstream ss;
            ss << addr[0] << "." << addr[1] << "." << addr[2] << "." << addr[3];
            m_descriptionAddr = ss.str();
            m_descriptionPort = port;
            m_descriptionUri = uri;
        }
    }

    /**
     * UPnP_Device::Init
     */
    void UPnP_Device::Init(const std::string & location)
    {
        unsigned int addr[4];
        unsigned int port;
        char uri[128];

        const char * str = location.c_str();
        int res = sscanf(str, "http://%d.%d.%d.%d:%d%128s", &addr[0], &addr[1], &addr[2], &addr[3], &port, uri);
        if (res == 6) {
            std::stringstream ss;
            ss << addr[0] << "." << addr[1] << "." << addr[2] << "." << addr[3];
            m_descriptionAddr = ss.str();
            m_descriptionPort = port;
            m_descriptionUri = uri;
        }
        m_location = location;
    }

    /**
     * UPnP_Device::AddService
     */
    void UPnP_Device::AddService(const UPnP_Service & service)
    {
        m_services.push_back(service);
    }

    /**
     * UPnP_Device::AddEmbeddedDevice
     */
    void UPnP_Device::AddEmbeddedDevice(UPnP_Device * dev)
    {
        lock();
        dev->RefIncrease();
        m_embeddedDevices.push_back(dev);
        unlock();
    }

    /**
     * UPnP_Context::ReleaseDevice
     */
    void UPnP_Context::ReleaseDevice(UPnP_Device * dev)
    {
        lock();
        /* decrease reference count for device */
        if (!dev->RefDecrease()) {
            /* device not referenced */
            if (dev->IsRootDevice()) { /* root device is unreferenced. */

            }
        }
        unlock();
    }


    /**
     * UPnP_Context::GetRootDeviceByUUID
     */
    UPnP_Device * UPnP_Context::GetRootDeviceByUUID(const uuid_t & uuid)
    {
        lock();
        for(std::list<UPnP_Device *>::iterator it = m_devices.begin(); it != m_devices.end(); it++)
        {
            if ((*it)->GetUUID() == uuid) {
                /* increase reference count */
                (*it)->RefIncrease();
                unlock();
                return *it;
            }
        }
        unlock();

        return 0;
    }

    /**
     *
     */
    UPnP_Device * UPnP_Context::GetDeviceByUUID(const uuid_t & uuid)
    {
        UPnP_Device * dev = 0;
        lock();
        for(std::list<UPnP_Device *>::iterator it = m_devices.begin(); it != m_devices.end(); it++)
        {
            UPnP_Device * current = *it;
            if (current->GetUUID() == uuid) {
                dev = current;
                break;
            }

            std::list<UPnP_Device *> & devices = current->GetEmbeddedDevices();
            for(std::list<UPnP_Device *>::iterator cit = devices.begin(); cit != devices.end(); cit++)
            {
                if ((*cit)->GetUUID() == uuid) {
                    /* lock both parent and child device in memory */
                    dev = (*it);
                    break;
                }
            }
        }

        if (dev)
            dev->RefIncrease();

        unlock();
        return dev;
    }


    /**
     * UPnP_Context::GetRootDeviceByLocation
     */
    UPnP_Device * UPnP_Context::GetRootDeviceByLocation(const char * location)
    {
        lock();
        for(std::list<UPnP_Device *>::iterator it = m_devices.begin(); it != m_devices.end(); it++)
        {
            if (!stricmp((*it)->GetLocation().c_str(), location))
            {
                (*it)->RefIncrease();
                unlock();
                return *it;
            }
        }
        unlock();
        return 0;
    }

    /**
     * UPnP_Context::AddRootDevice
     */
    int UPnP_Context::AddRootDevice(const ssdp_request_t * request)
    {
        printf("%s()\r\n", __FUNCTION__);

        UPnP_Device * dev = new UPnP_Device();      
        dev->Init(request);

        lock();
        dev->RefIncrease();
        m_devices.push_back(dev);
        unlock();

        /* notify */
        OnDeviceAvailable(request->uuid);
        return 0;
    }r

    /**
     * UPnP_Context::AddRootDeviceByLocation
     */
    int UPnP_ClientContext::AddRootDeviceByLocation(const std::string & location)
    {
        UPnP_Device * dev = new UPnP_Device();
        dev->Init(location);

        lock();
        dev->RefIncrease();
        m_devices.push_back(dev);
        unlock();

        DescriptionEntry entry;
        entry.type      = DescriptionEntry::DESCRIPTION_BY_LOCATION;
        entry.location  = location;

        /* fetch information about this device */
        descriptionQueue.Put(entry);

        return 0;
    }


    /**
     *
     */
    int UPnP_Context::DeviceAvailable(const ssdp_request_t * request)
    {
        UPnP_Device * dev = 0;
        const char * nt = ssdp_field_contents(request, "NT"), * location = ssdp_field_contents(request, "LOCATION");
        if (!nt || !location) {
            printf("%s() missing fields.\r\n", __FUNCTION__);
            return -1;
        }

        printf("%s() nt: %s\r\n", __FUNCTION__, nt);

        if (!strcmp(nt, "upnp:rootdevice") || (dev = GetRootDeviceByUUID(request->uuid))) {
            /* sspd identifies a root device */
            RootDeviceAvailable(request);
            if (dev) {
                ReleaseDevice(dev);
            }
        } else if(!strncmp(nt, "uuid", 4) && (dev = GetRootDeviceByLocation(location))) {
            printf("EmbeddedDevice().\r\n");
            if (dev)
                ReleaseDevice(dev);
        }

        return 0;
    }

    /**
     * UPnP_Context::DeviceUnavailable
     */
    int UPnP_Context::DeviceUnavailable(const ssdp_request_t * request)
    {
        printf("%s()\r\n", __FUNCTION__);
        return RemoveDeviceByUUID(request->uuid);
    }

    /**
     * UPnP_Context::RootDeviceAvailable
     */
    int UPnP_Context::RootDeviceAvailable(const ssdp_request_t * request)
    {
        printf("%s()\r\n", __FUNCTION__);
        UPnP_Device * dev = GetRootDeviceByUUID(request->uuid);
        if (dev) {
            ReleaseDevice(dev);
        } else {
            return AddRootDevice(request);
        }
        return 0;
    }

    /**
     * UPnP_Context::RemoveDeviceByUUID
     *      Removes a device based on the UUID.
     */
    int UPnP_Context::RemoveDeviceByUUID(const uuid_t & id)
    {
        UPnP_Device * dev = GetRootDeviceByUUID(id);
        if (dev) { /* remove root device */
            printf("Remove root device.\r\n");
            RemoveRootDevice(id);
            /* decrease refcount and (maybe) delete it */
            ReleaseDevice(dev);
        } else {
            printf("Remove embedded device.\r\n");
            /* not a root device */
            RemoveEmbeddedDevice(id);
        }
        return 0;
    }

    /**
     * UPnP_Context::RemoveRootDevice
     *      Removes a root device, this happens when the device becomes unavailable (ssdp:byeye)
     */
    int UPnP_Context::RemoveRootDevice(const uuid_t & id)
    {
        for(std::list<UPnP_Device *>::iterator it = m_devices.begin(); it != m_devices.end(); it++)
        {
            UPnP_Device * dev = *it;
            if (dev->GetUUID() == id) { /* found device */

            }
        }
        return 0;
    }

    /**
     * UPnP_Context::RemoveEmbeddedDevice
     */
    int UPnP_Context::RemoveEmbeddedDevice(const uuid_t & id)
    {
        return 0;
    }

    /*************************************************************************/
    /*                              CLIENT                                   */
    /*************************************************************************/
    void UPnP_ClientContext::OnDeviceAvailable(const uuid_t & id)
    {
        DescriptionEntry entry;
        entry.type = DescriptionEntry::DESCRIPTION_BY_UUID;
        entry.id = id;
        /* fetch information about this device */
        descriptionQueue.Put(entry);
    }

    void UPnP_ClientContext::OnDeviceUnavailable(const uuid_t & id)
    {
    }
}
/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "deviceinfo.h"
#include "upnp.h"
#include "http.h"
#include "UPnP_XMLParser.h"

namespace upnp
{
    /**
     * UPnP_ClientContext::DescriptionTask
     *          Fetches device information.
     */
    void UPnP_ClientContext::DescriptionTask()
    {
        DescriptionEntry entry;
        UPnP_HttpRequest response;
        while(1)
        {
            if (descriptionQueue.Get(entry))
            {
                UPnP_Device * dev;
                if (entry.type == DescriptionEntry::DESCRIPTION_BY_UUID) {
                    /* fetch device information */
                    dev = GetDeviceByUUID(entry.id);
                } else {
                    dev = GetRootDeviceByLocation(entry.location.c_str());
                }
                if (dev) {
                    GetDeviceDescription(dev);
                    /* decrease reference count */
                    ReleaseDevice(dev);
                }
            }
        }
    }

    /**
     * UPnP_ClientContext::GetDeviceDescription
     */
    int UPnP_ClientContext::GetDeviceDescription(UPnP_Device * dev)
    {
        int error, nret;
        UPnP_HttpRequest request;
        upnp::UPnP_HttpRequest * response = request.GetRequest(dev->GetDescriptionAddr().c_str(), dev->GetDescriptionPort(), dev->GetDescriptionUri().c_str(), error);
        if (!response) {
            return -1;
        }
        /* parse device description */
        nret = ParseDeviceDescription(dev, response->GetPayload());
        delete response;

        return nret;
    }

    /**
     * UPnP_ClientContext::ParseDeviceDescription
     */
    int UPnP_ClientContext::ParseDeviceDescription(UPnP_Device * dev, const std::vector<unsigned char> & data)
    {
        if (!UPnP_XMLParser::ParseDeviceDescription(dev, data)) {
            printf("%s() failed to parse device description\r\n", __FUNCTION__);
            return -1;
        }
        /* notify available services */
        NotifyServiceAvailable(dev);
        return 0;
    }

    /**
     * UPnP_ClientContext::NotifyServiceAvailable
     */
    void UPnP_ClientContext::NotifyServiceAvailable(UPnP_Device * dev)
    {
        std::list<UPnP_Service> & services = dev->GetServices();
        for(std::list<UPnP_Service>::const_iterator it = services.begin(); it != services.end(); it++)
        {
            OnServiceAvailable(dev->GetUUID(), it->GetServiceURN(), *it);
        }
        const std::list<UPnP_Device *> & devices = dev->GetEmbeddedDevices();
        for(std::list<UPnP_Device *>::const_iterator it = devices.begin(); it != devices.end(); it++)
        {
            NotifyServiceAvailable(*it);
        }
    }
}
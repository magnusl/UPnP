/**s
 * Copyright (c) 2010 Magnus Leksell, all rights reserved.
 */

#include "upnp.h"
#include "ssdp.h"

#include <string.h>


namespace upnp
{
    /**
     * UPnP_ClientContext::NotifyTask
     */
    void UPnP_ClientContext::NotifyTask()
    {
        while(1) {
            /* read a multicast ssdp */
            ssdp_request_t request;
            if (upnp_read_multicast_ssdp(ssdp_notify_multicast, &request, 1000) == 0) {
                /* handle the SSDP */
                SSDPEvent(&request);
            }
        }
    }

    /**
     * UPnP_ClientContext::SSDPEvent
     */
    void UPnP_ClientContext::SSDPEvent(ssdp_request_t * request)
    {
        /* we only care about notifications, only the server is interested in searches */
        if (!strncmp(request->startLine, "NOTIFY", 6)) {
            SSDPNotify(request);
        } else {
        
        }
    }

    /**
     * UPnP_ClientContext::SSDPNotify
     */
    void UPnP_ClientContext::SSDPNotify(ssdp_request_t * request)
    {
        /* Notification Sub Type */
        const char * nts = ssdp_field_contents(request, "NTS");
        if (!nts) {
        
        }
        if (!strcmp(nts, "ssdp:alive")) {
            DeviceAvailable(request);
        } else if(!strcmp(nts, "ssdp:byebye")) {
            DeviceUnavailable(request);
        } else if(!strcmp(nts, "ssdp:update")) {
            printf("%s() ssdp:update\r\n", __FUNCTION__);
        }
    }
}
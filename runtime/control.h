/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "uuid.h"
#include "upnp.h"
#include "soap.h"

namespace upnp
{
    /**
     * UPnP_ControlPoint
     */
    class UPnP_ControlPoint
    {
    public:
        UPnP_ControlPoint(upnp::UPnP_ClientContext & context, const uuid_t & , const UPnP_Service &);
        ~UPnP_ControlPoint();

    protected:

        int PerformSoapAction(upnp::UPnP_SOAPAction &, upnp::UPnP_SOAPAction & response);

        UPnP_Service    m_service;
        uuid_t          m_deviceId;     /* device that implements this service */
        UPnP_Context &  m_context;
    };
}

#endif
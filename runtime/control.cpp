/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "control.h"
#include "http.h"
#include "UPnP_XMLParser.h"
#include <cstdio>

namespace upnp
{
    UPnP_ControlPoint::UPnP_ControlPoint(
        upnp::UPnP_ClientContext & context, const uuid_t & id, const UPnP_Service & service) :
        m_context(context),
        m_deviceId(id),
        m_service(service)
    {

    }

    UPnP_ControlPoint::~UPnP_ControlPoint()
    {
    }

    /**
     * UPnP_ControlPoint::PerformSoapAction
     */
    int UPnP_ControlPoint::PerformSoapAction(upnp::UPnP_SOAPAction & action, upnp::UPnP_SOAPAction & soap_response)
    {
        int res, error;
        UPnP_HttpRequest request;

        UPnP_Device * dev = m_context.GetDeviceByUUID(this->m_deviceId);
        if (!dev) {
            printf("%s() no such device.\r\n", __FUNCTION__);
            return -1;
        }

        action.setServiceUrn(m_service.GetServiceURN());
        std::string addr = dev->GetDescriptionAddr();
        unsigned short port = dev->GetDescriptionPort();

        m_context.ReleaseDevice(dev);
        action.setHost(addr, port);

        if (!action.serialize(request)) {
            printf("%s() failed to serialize SOAP request.\r\n", __FUNCTION__);
            return -1;
        }
        /* perform POST request */
        upnp::UPnP_HttpRequest * response = request.PostRequest(addr.c_str(), port, m_service.GetControlURL().c_str(), error);
        if (!response) {
            printf("%s() POST request failed.\r\n", __FUNCTION__);
            return -1;
        }

        if(res = upnp::UPnP_XMLParser::ParseSOAPResponse(&soap_response, response->GetPayload())) {
            delete response;
            printf("%s() failed to parse SOAP response.\r\n", __FUNCTION__);
            return res;
        }

        return 0;
    }
}
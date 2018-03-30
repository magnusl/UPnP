/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _SOAP_H_
#define _SOAP_H_

#include <string>
#include <list>
#include "http.h"

namespace upnp
{
    /**
     * UPnP_SOAPAction
     *      SOAP Action request/response using SOAP 1.1 UPnP Profile
     */
    class UPnP_SOAPAction
    {
    public:

        UPnP_SOAPAction & setHost(const std::string & addr, unsigned short port);
        UPnP_SOAPAction & setServiceUrn(const std::string &);
        UPnP_SOAPAction & setActionName(const std::string &);
        UPnP_SOAPAction & addArgument(const std::string &, const std::string &);

        bool serialize(UPnP_HttpRequest &) const;           /* serializes the soap request/response to a HTTP request */
        bool deserialize(const UPnP_HttpRequest &);         /* deserializes the soap request/response from a HTTP request */

        /* used for serailization */
        const std::string getActionName() const {return m_actionName;}
        const std::list<std::pair<std::string, std::string> > & getArguments() const {return m_arguments;}

        bool getArgumentValueByName(const std::string &, std::string &) const;

    protected:
        std::string     m_actionName;
        std::string     m_serviceUrn;
        std::string     m_hostAddr;
        unsigned short  m_hostPort;
        std::list<std::pair<std::string, std::string> > m_arguments;
    };
}

#endif
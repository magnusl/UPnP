/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "soap.h"
#include <sstream>

using namespace std;

namespace upnp
{
    UPnP_SOAPAction & UPnP_SOAPAction::addArgument(const std::string & name, const std::string & value)
    {
        m_arguments.push_back(pair<string, string>(name, value));
        return *this;
    }

    UPnP_SOAPAction & UPnP_SOAPAction::setActionName(const std::string & name)
    {
        m_actionName = name;
        return *this;
    }

    UPnP_SOAPAction & UPnP_SOAPAction::setServiceUrn(const std::string & urn)
    {
        this->m_serviceUrn = urn;
        return *this;
    }

    UPnP_SOAPAction & UPnP_SOAPAction::setHost(const std::string & host, unsigned short port)
    {
        m_hostAddr = host;
        m_hostPort = port;
        return *this;
    }

    /**
     * UPnP_SOAPAction::getArgumentValueByName
     */
    bool UPnP_SOAPAction::getArgumentValueByName(const std::string & name, std::string & value) const
    {
        for(list<pair<string,string> >::const_iterator it = m_arguments.begin(); 
            it != m_arguments.end();
            it++)
        {
            if (it->first == name) {
                value = it->second;
                return true;
            }
        }
        return false;
    }

    /**
     * UPnP_SOAPAction::serialize
     *      Serializes the SOAP request/response to a HTTP request.
     */
    bool UPnP_SOAPAction::serialize(UPnP_HttpRequest & request) const
    {
        std::stringstream ss, urn, port;
        urn << "\"" << this->m_serviceUrn << "#" << m_actionName << "\"";
        port << m_hostPort;

        request.addHeader("CONTENT-TYPE", "text/xml; charset=\"utf-8\"");
        request.addHeader("SOAPACTION", urn.str());
        request.addHeader(m_hostAddr, port.str());
        
        /* set payload */
        ss << "<?xml version=\"1.0\"?>\r\n";
        ss << "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">" << "\r\n";
        ss << "\t" << "<s:Body>" << "\r\n";
        ss << "\t\t" << "<u:" << this->m_actionName << " xmlns:u=\"" << this->m_serviceUrn << "\">\r\n";
        /* serialize arguments */
        for(list<pair<string,string> >::const_iterator it = m_arguments.begin(); it != m_arguments.end(); it++)
        {
            ss << "\t\t\t<" << it->first << ">" << it->second << "</" << it->first << ">\r\n";
        }   
        ss << "\t\t" << "</u:" << m_actionName << ">" << "\r\n";
        ss << "\t" << "</s:Body>" << "\r\n";
        ss << "</s:Envelope>" << "\r\n";
    
        const std::string & str = ss.str();

        request.setPayload((const unsigned char *) str.c_str(), str.size());

        return true;
    }

    /**
     * UPnP_SOAPAction::deserialize
     */
    bool UPnP_SOAPAction::deserialize(const UPnP_HttpRequest & request)
    {
        return false;
    }
}
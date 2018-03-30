/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "UPnP_XMLParser.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

XERCES_CPP_NAMESPACE_USE

void splitString(const std::string & str, char delim, std::vector<std::string> & tokens);

namespace upnp
{
    static bool ParseDeviceDOM(UPnP_Device * dev, DOMDocument * doc);
    static bool ParseDeviceRootNode(UPnP_Device * dev, const DOMNode * node);
    static bool ParseDeviceNode(UPnP_Device * dev, const DOMNode * node);
    static bool ParseDeviceName(UPnP_Device * dev, const DOMNode * node);
    static bool ParseService(UPnP_Device * dev, const DOMNode * node);
    static bool ParseServiceList(UPnP_Device * dev, const DOMNode * node);
    static bool ParseServiceType(const DOMNode * node, UPnP_Service * service);
    static int ParseResponseEnvelope(UPnP_SOAPAction * response, const DOMElement * node);
    static int ParseResponseDocument(UPnP_SOAPAction * response, DOMDocument * doc);
    static bool ParseDeviceUUID(UPnP_Device * dev, const DOMNode * node);
    static bool ParseDeviceList(UPnP_Device * dev, const DOMNode * node);
    static int ParseResponseBody(UPnP_SOAPAction * response, const DOMElement * node);
    static int ParseResponseArguments(UPnP_SOAPAction * response, const DOMElement * node);

    /**
     * UPnP_XMLParser::init
     */
    bool UPnP_XMLParser::init()
    {
        XMLPlatformUtils::Initialize();

        return true;
    }

    /**
     * UPnP_XMLParser::ParseDeviceDescription
     */
    bool UPnP_XMLParser::ParseDeviceDescription(UPnP_Device * dev, const std::vector<unsigned char> & mem)
    {
        bool bret;

        if (mem.empty())
            return false;

        XercesDOMParser* parser = new XercesDOMParser();
        if (!parser)
            return false;

        parser->setValidationScheme(XercesDOMParser::Val_Always);
        parser->setDoNamespaces(true);    // optional

        /* parse in-memory xml */
        MemBufInputSource input(&mem[0], mem.size(), L"devDescription", false);
        
        /* parse the xml file */
        try {
            parser->parse(input);
        } catch(...) {
            delete parser;
            return false;
        }
        /* parsed */
        DOMDocument * doc = parser->getDocument();
        if (!doc) {
            delete parser;
            return false;
        }
        /* parse DOM */
        bret = ParseDeviceDOM(dev, doc);
        delete parser;

        return bret;
    }

    /**
     * ParseDeviceDOM
     */
    static bool ParseDeviceDOM(UPnP_Device * dev, DOMDocument * doc)
    {
        const DOMNodeList * nodes = doc->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                char *name = XMLString::transcode(child->getNodeName());
                if(!strcmp(name, "root")) {
                    XMLString::release(&name);
                    return ParseDeviceRootNode(dev, child);
                }
                XMLString::release(&name);
            }
        }
        return false;
    }

    /**
     * ParseDeviceRootNode
     */
    static bool ParseDeviceRootNode(UPnP_Device * dev, const DOMNode * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                char *name = XMLString::transcode(child->getNodeName());
                if (!strcmp(name, "device")) {
                    ParseDeviceNode(dev, child);
                }
                XMLString::release(&name);
            }
        }

        return true;
    }

    /**
     * ParseDeviceNode
     */
    static bool ParseDeviceNode(UPnP_Device * dev, const DOMNode * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                char *name = XMLString::transcode(child->getNodeName());
                if(!stricmp(name, "friendlyName")) {
                    ParseDeviceName(dev, child);
                } else if(!stricmp(name, "serviceList")) {
                    ParseServiceList(dev, child);
                } else if(!stricmp(name, "UDN")) {
                    ParseDeviceUUID(dev, child);
                } else if(!stricmp(name, "deviceList")) {
                    ParseDeviceList(dev, child);
                }
                XMLString::release(&name);
            }
        }
        return true;
    }

    /**
     * ParseDeviceList
     *      Parses a list of embedded devices.
     */
    static bool ParseDeviceList(UPnP_Device * dev, const DOMNode * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child && (child->getNodeType() == DOMNode::ELEMENT_NODE))
            {
                const DOMElement * element = (DOMElement *) child;
                char *name = XMLString::transcode(element->getNodeName());
                if (!stricmp(name, "device")) {
                    UPnP_Device * embeddedDev = new UPnP_Device;
                
                    embeddedDev->SetDescriptionAddr( dev->GetDescriptionAddr() );
                    embeddedDev->SetDescriptionPort( dev->GetDescriptionPort() );

                    if (!ParseDeviceNode(embeddedDev, child)) {
                        delete embeddedDev;
                        return false;
                    }
                    dev->AddEmbeddedDevice(embeddedDev);
                }
            }
        }
        return true;
    }

    /**
     * ParseDeviceName
     */
    static bool ParseDeviceName(UPnP_Device * dev, const DOMNode * node)
    {
        char * value = XMLString::transcode(node->getTextContent());
        if (value) {
            dev->SetFriendlyName(value);
            printf("%s: device name: %s\r\n", __FUNCTION__, value);
            XMLString::release(&value);
        }
        return true;
    }

    /**
     * 
     */
    static bool ParseDeviceUUID(UPnP_Device * dev, const DOMNode * node)
    {
        char * value = XMLString::transcode(node->getTextContent());
        if (value) {
            uuid_t id;
            if (strncmp(value, "uuid:", 5)) {
                XMLString::release(&value);
                return false;
            }
            if (!id.parse(value + 5)) {
                XMLString::release(&value);
                return false;
            }
            dev->SetUUID(id);
            printf("%s() UDN: %s\r\n", __FUNCTION__, value);
            XMLString::release(&value);
            return true;
        }
        return false;
    }

    /**
     * ParseServiceList
     */
    static bool ParseServiceList(UPnP_Device * dev, const DOMNode * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                char *name = XMLString::transcode(child->getNodeName());
                if (name) {
                    if (!strcmp(name, "service")) {
                        ParseService(dev, child);
                    }
                    XMLString::release(&name);
                }
            }
        }
        return true;
    }

    /**
     * ParseService
     */
    static bool ParseService(UPnP_Device * dev, const DOMNode * node)
    {
        UPnP_Service service;
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                char *name = XMLString::transcode(child->getNodeName());
                if (name) {
                    if (!stricmp(name, "serviceType")) {
                        ParseServiceType(child, &service);
                    } else if (!stricmp(name, "SCPDURL")) {
                        char * value = XMLString::transcode(child->getTextContent());
                        if (value) {
                            service.SetDescriptionURL(value);
                            XMLString::release(&value);
                        }
                    } else if(!stricmp(name, "controlURL")) {
                        char * value = XMLString::transcode(child->getTextContent());
                        if (value) {
                            service.SetControlURL(value);
                            XMLString::release(&value);
                        }
                    } else if(!stricmp(name, "eventSubURL")) {
                        char * value = XMLString::transcode(child->getTextContent());
                        if (value) {
                            service.SetEventURL(value);
                            XMLString::release(&value);
                        }
                    }
                    XMLString::release(&name);
                }
            }
        }
        
        /* add the service to the device */
        dev->AddService(service);
        return true;
    }

    /**
     * ParseServiceType
     */
    static bool ParseServiceType(const DOMNode * node, UPnP_Service * service)
    {
        bool bret = false;
        std::vector<std::string> tokens;
        char *value = XMLString::transcode(node->getTextContent());
        if (value) {
            std::string str = value;
            splitString(str, ':', tokens);
            if (tokens.size() == 5 && (tokens[0] == "urn") && (tokens[2] == "service")) { /* urn:schemas-upnp-org:service:serviceType:v */
                service->SetSchema(tokens[1]);
                service->SetServiceType(tokens[3]);
                service->SetVersion(atoi(tokens[4].c_str()));
                service->SetServiceURN(str);
                bret = true;
            }
            XMLString::release(&value);
        }
        return bret;
    }

    /**
     * UPnP_XMLParser::ParseSOAPResponse
     *      Parses a SOAP response.
     */
    bool UPnP_XMLParser::ParseSOAPResponse(UPnP_SOAPAction * response, const std::vector<unsigned char> & mem)
    {
        bool bret;

        if (mem.empty())
            return false;

        XercesDOMParser* parser = new XercesDOMParser();
        if (!parser)
            return false;

        parser->setValidationScheme(XercesDOMParser::Val_Always);
        parser->setDoNamespaces(true);    // optional

        /* parse in-memory xml */
        MemBufInputSource input(&mem[0], mem.size(), L"devDescription", false);
        
        /* parse the xml file */
        try {
            parser->parse(input);
        } catch(...) {
            delete parser;
            return false;
        }
        /* parsed */
        DOMDocument * doc = parser->getDocument();
        if (!doc) {
            delete parser;
            return false;
        }
        /* parse DOM */
        bret =  ParseResponseDocument(response, doc);
        delete parser;

        return bret;
    }

    /**
     * ParseEnvelopeNode
     */
    static int ParseResponseDocument(UPnP_SOAPAction * response, DOMDocument * doc)
    {
        DOMElement * element = 0;
        const XMLCh * nsUri;

        const DOMNodeList * nodes = doc->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE) {
                    element = (DOMElement *) child;
                    break;
                }
            }
        }
        if (!element || !(nsUri = element->getNamespaceURI()))
            return -1;

        char * nsStr = XMLString::transcode(nsUri);
        if (strcmp(nsStr, "http://schemas.xmlsoap.org/soap/envelope/")) {
            printf("%s() incorrect XML namespace.\r\n", __FUNCTION__);
            XMLString::release(&nsStr);
            return -1;
        }
        XMLString::release(&nsStr);
        char * elementName = XMLString::transcode(element->getLocalName());
        if (stricmp(elementName, "envelope")) {
            printf("%s() expectex a SOAP Envelope element.\r\n", __FUNCTION__);
            XMLString::release(&elementName);
            return -1;
        }
        XMLString::release(&elementName);

        return ParseResponseEnvelope(response, element);
    }

    /**
     * ParseResponseEnvelope
     *          Parses a <Envelope>...</Envelope>
     */
    static int ParseResponseEnvelope(UPnP_SOAPAction * response, const DOMElement * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child && (child->getNodeType() == DOMNode::ELEMENT_NODE)) {
                DOMElement * element = (DOMElement *) child;
                char * elementName = XMLString::transcode(element->getLocalName());
                if (stricmp(elementName, "body")) {
                    XMLString::release(&elementName);
                    return -1;
                }
                XMLString::release(&elementName);
                return ParseResponseBody(response, element);
            }
        }
        return -1;
    }

    /**
     * ParseResponseBody
     *      Parses <s:Body> .. </s:Body>
     */
    static int ParseResponseBody(UPnP_SOAPAction * response, const DOMElement * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child && (child->getNodeType() == DOMNode::ELEMENT_NODE)) {
                DOMElement * element = (DOMElement *) child;
                char * elementName = XMLString::transcode(element->getLocalName());
                char * ptr = strstr(elementName, "Response");
                if (!ptr || (ptr == elementName)) {
                    XMLString::release(&elementName);
                    return -1;
                }
                XMLString::release(&elementName);
                return ParseResponseArguments(response, element);
            }
        }
        return 0;
    }

    /**
     *
     */
    static int ParseResponseArguments(UPnP_SOAPAction * response, const DOMElement * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child && (child->getNodeType() == DOMNode::ELEMENT_NODE)) {
                DOMElement * element = (DOMElement *) child;
                char * elementName = XMLString::transcode(element->getLocalName());
                char * value = XMLString::transcode(element->getTextContent());
                response->addArgument(elementName, value ? value : "");
                XMLString::release(&elementName);
                XMLString::release(&value);
            }
        }
        return 0;
    }
}
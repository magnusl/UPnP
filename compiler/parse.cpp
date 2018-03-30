/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "service.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <cstdio>
#include <cstdlib>

XERCES_CPP_NAMESPACE_USE

namespace upnp
{
#ifndef WIN32
    int stricmp(const char * str, const char * str2)
    {
      return strcasecmp(str, str2);
    }
#endif
    
    /*************************************************************************/
    /*                          FORWARD DECLARATIONS                         */
    /*************************************************************************/
    static int UPnP_ParseService(upnp::UPnP_Service & service, DOMDocument * doc);
    static int ParseServiceNode(upnp::UPnP_Service & service, const DOMElement * node);
    static int ParseActionList(upnp::UPnP_Service & service, const DOMElement * node);
    static int ParseAction(upnp::UPnP_Service & service, const DOMElement * node);
    static int ParseArgumentList(upnp::UPnP_ServiceAction & action, const DOMElement * node);
    static int ParseArgument(upnp::UPnP_MethodArgument & argument, const DOMElement * node);
    static int ParseStateTable(upnp::UPnP_Service & service, const DOMElement * node);
    static int ParseStateVariable(upnp::UPnP_StateVariable & variable, const DOMElement * node);
    static int ParseDataType(upnp::UPnP_StateVariable & variable, const DOMElement * node);

    /*************************************************************************/
    /*                          EXPORTED FUNCTIONS                           */
    /*************************************************************************/

    /**
     * UPnP_ParseServiceDescription
     *      Parses a UPnP service description (scpd) and constructs a intermediate representation of 
     *      the service.
     */
    int UPnP_ParseServiceDescription(upnp::UPnP_Service & service, const char * filename)
    {
        XercesDOMParser* parser = new XercesDOMParser();
        if (!parser)
            return false;

        parser->setValidationScheme(XercesDOMParser::Val_Always);
        parser->setDoNamespaces(true);    // optional
        
        /* parse the xml file */
        try {
            parser->parse(filename);
        } catch(...) {
            delete parser;
            return -1;
        }
        /* parsed */
        DOMDocument * doc = parser->getDocument();
        if (!doc) {
            delete parser;
            return -1;
        }

        int res = UPnP_ParseService(service, doc);
        delete parser;

        return res;
    }

    /**
     * UPnP_ParseService
     */
    static int UPnP_ParseService(upnp::UPnP_Service & service, DOMDocument * doc)
    {
        const DOMNodeList * nodes = doc->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE)
                {
                    DOMElement * element = (DOMElement *) child;
                    char *name = XMLString::transcode(element->getNodeName());
                    if(!strcmp(name, "scpd")) {
                        XMLString::release(&name);
                        return ParseServiceNode(service, element);
                    }
                    XMLString::release(&name);
                }
            }
        }
        return 0;
    }

    /**
     * ParseServiceNode
     */
    static int ParseServiceNode(upnp::UPnP_Service & service, const DOMElement * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE) {
                    DOMElement * element = (DOMElement *) child;
                    char *name = XMLString::transcode(element->getNodeName());
                    if(!stricmp(name, "actionList")) {
                        if (ParseActionList(service, element)) {
                            XMLString::release(&name);
                            return -1;
                        }
                    } else if(!stricmp(name, "serviceStateTable")) {
                        ParseStateTable(service, element);
                    }
                    XMLString::release(&name);
                }
            }
        }
        return 0;
    }

    /**
     * ParseActionList
     */
    static int ParseActionList(upnp::UPnP_Service & service, const DOMElement * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE) {
                    DOMElement * element = (DOMElement *) child;
                    char *name = XMLString::transcode(element->getNodeName());
                    if(!stricmp(name, "action")) {
                        if (ParseAction(service, element)) {
                            printf("%s() failed to parse action.\r\n", __FUNCTION__);
                            XMLString::release(&name);
                            return -1;
                        }
                    }
                    XMLString::release(&name);
                }
            }
        }
        return 0;
    }

    /**
     * ParseAction
     */
    static int ParseAction(upnp::UPnP_Service & service, const DOMElement * node)
    {
        upnp::UPnP_ServiceAction action;
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE) {
                    DOMElement * element = (DOMElement *) child;
                    char *name = XMLString::transcode(element->getNodeName());
                    if (!strcmp(name, "name")) {
                        const char * value = XMLString::transcode(element->getTextContent());
                        if (value) {
                            action.m_name = value;
                        }
                    } else if (!strcmp(name, "argumentList")) {
                        if (ParseArgumentList(action, element)) {
                            XMLString::release(&name);
                            printf("%s() failed to parse argument list\r\n", __FUNCTION__);
                            return -1;
                        }   
                    }
                    XMLString::release(&name);
                }
            }
        }

        service.m_actions.push_back(action);
        return 0;
    }

    /**
     * ParseArgumentList
     */
    static int ParseArgumentList(upnp::UPnP_ServiceAction & action, const DOMElement * node)
    {
        upnp::UPnP_MethodArgument argument;
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE) {
                    DOMElement * element = (DOMElement *) child;
                    char *name = XMLString::transcode(element->getNodeName());
                    if (!strcmp(name, "argument")) {
                        if (ParseArgument(argument, element)) {
                            XMLString::release(&name);
                            printf("%s() failed to parse argument\r\n", __FUNCTION__);
                            return -1;
                        }
                        action.m_arguments.push_back(argument);
                    }
                    XMLString::release(&name);
                }
            }
        }
        return 0;
    }

    /**
     * ParseArgument
     */
    static int ParseArgument(upnp::UPnP_MethodArgument & argument, const DOMElement * node)
    {
        bool parsedDirection = false;
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE) {
                    DOMElement * element = (DOMElement *) child;
                    char *name = XMLString::transcode(element->getNodeName());
                    if (!stricmp(name, "name")) {
                        /**
                        * Argument Name (REQUIRED)
                        */
                        char * value = XMLString::transcode(element->getTextContent());
                        if (!value) {
                            printf("%s() invalid argument name.\r\n", __FUNCTION__);
                            XMLString::release(&name);
                            return -1;
                        }
                        argument.m_name = value;
                        XMLString::release(&value);
                    } else if(!strcmp(name, "direction")) {
                        /**
                        * Argument direction, either in or out (REQUIRED)
                        */
                        char * value = XMLString::transcode(element->getTextContent());
                        if (!strcmp(value, "in")) {
                            argument.direction = upnp::UPnP_MethodArgument::ARG_DIR_IN;
                        } else if (!strcmp(value, "out")) {
                            argument.direction = upnp::UPnP_MethodArgument::ARG_DIR_OUT;
                        } else {
                            printf("%s() invalid argument direction '%s'\r\n", __FUNCTION__, value);
                            XMLString::release(&name);
                            XMLString::release(&value);
                            return -1;
                        }
                        parsedDirection = true;
                        XMLString::release(&value);
                    } else if(!strcmp(name, "retval")) {
                        /**
                        * 'retval' - indicates that the argument is the return value of the function (OPTIONAL)
                        */
                        argument.m_retval = true;
                    } else if(!strcmp(name, "relatedStateVariable")) {
                        /**
                        * Related state variable (REQUIRED)
                        */
                        char * value = XMLString::transcode(element->getTextContent());
                        if (value) {
                            std::string temp = value;
                            size_t delim = temp.find_first_not_of('\n');
                            if (delim != temp.npos)
                                temp = temp.substr(delim);
                            delim = temp.find_last_not_of('\n');
                            if (delim != temp.npos)
                                temp = temp.substr(0, delim+1);

                            argument.m_relatedStateVariable = temp;
                            XMLString::release(&value);
                        } else {
                            printf("%s() failed to parse 'relatedStateVariable'\r\n", __FUNCTION__);
                            XMLString::release(&name);
                            return -1;
                        }
                    } else {
                        printf("%s() unknown XML element '%s'.\r\n", __FUNCTION__, name);
                    }
                    XMLString::release(&name);
                }
            }
        }

        if (argument.m_name.empty() ||
            argument.m_relatedStateVariable.empty() ||
            !parsedDirection) 
        {
            printf("%s() incorrect argument.\r\n", __FUNCTION__);
            return -1;
        }

        return 0;
    }

    /**
     * ParseStateTable
     */
    static int ParseStateTable(upnp::UPnP_Service & service, const DOMElement * node)
    {
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE) {
                    DOMElement * element = (DOMElement *) child;
                    upnp::UPnP_StateVariable variable;
                    char *name = XMLString::transcode(element->getNodeName());
                    if (!strcmp(name, "stateVariable")) {
                        if (ParseStateVariable(variable, element)) {
                            XMLString::release(&name);
                            return -1;
                        }
                        XMLString::release(&name);
                        service.m_stateVariables.push_back(variable);
                    } else {

                    }
                }
            }
        }

        return 0;
    }

    /**
     * ParseStateVariable
     */
    static int ParseStateVariable(upnp::UPnP_StateVariable & variable, const DOMElement * node)
    {
        /* Parse arguments */
        const XMLCh * ptr = node->getAttribute((const XMLCh *) "sendEvents");
        if (ptr && ptr[0]) {
            char * value = XMLString::transcode(ptr);
            if (!stricmp(value, "yes")) {
                variable.m_sendEvents = true;
            } else if(!stricmp(value, "no")) {
                variable.m_sendEvents = false;
            } else {
                printf("%s() incorrect value for attribute %s\r\n", __FUNCTION__, "sendEvents");
                XMLString::release(&value);
                return -1;
            }
            XMLString::release(&value);
        } else {
            variable.m_sendEvents = true;
        }

        ptr = node->getAttribute((const XMLCh *)"multicast");
        if (ptr && ptr[0]) {
            char * value = XMLString::transcode(ptr);
            if (!stricmp(value, "yes")) {
                variable.m_multicast = true;
            } else if(!stricmp(value, "no")) {
                variable.m_multicast = false;
            } else {
                printf("%s() incorrect value for attribute %s\r\n", __FUNCTION__, "multicast");
                XMLString::release(&value);
                return -1;
            }
            XMLString::release(&value);
        } else {
            variable.m_multicast = false;
        }

        /* Parse child elements */
        const DOMNodeList * nodes = node->getChildNodes();
        for(size_t i = 0; i < nodes->getLength(); i++)
        {
            const DOMNode * child = nodes->item(i);
            if (child) {
                if (child->getNodeType() && child->getNodeType() == DOMNode::ELEMENT_NODE) {
                    DOMElement * element = (DOMElement *) child;
                    char *name = XMLString::transcode(element->getNodeName());
                    if (!stricmp(name, "name")) {
                        /* <name>...</name> */
                        char * value = XMLString::transcode(element->getTextContent());
                        if (value) {
                            variable.m_name = value;
                            XMLString::release(&value);
                        }
                    } else if (!stricmp(name, "dataType")) {
                        /* <dataType>...</dataType> */
                        if (ParseDataType(variable, element)) {
                            XMLString::release(&name);
                            return -1;
                        }
                    } else {

                    }
                    XMLString::release(&name);
                }
            }
        }
        return 0;
    }

    /**
     * ParseDataType
     */
    static int ParseDataType(upnp::UPnP_StateVariable & variable, const DOMElement * node)
    {
        variable.m_typeOverride = false;
        
        const XMLCh * str = node->getAttribute( (const XMLCh *)"type");
        if (str && str[0]) {
            /* value of dataType must be string */
            printf("override\r\n");
            variable.m_typeOverride = true;
        }
        char * value = XMLString::transcode(node->getTextContent());
        if (!value) {
            return -1;
        }
        if (variable.m_typeOverride) {
            if (strcmp(value, "string")) {
                printf("%s() value must be 'string' if the 'type' attribute is included.\r\n", __FUNCTION__);
                XMLString::release(&value);
                return -1;
            }
            variable.m_type = upnp::TYPE_STRING;
        } else {
            if ((variable.m_type = UPnP_GetTypeByName(value)) == upnp::TYPE_INVALID) {
                XMLString::release(&value);
                printf("%s() invalid data type.\r\n", __FUNCTION__);
                return -1;
            }
        }

        printf("type: %s\r\n", UPnP_GetNameByType(variable.m_type) ? UPnP_GetNameByType(variable.m_type) : "<null>");
        XMLString::release(&value);
        return 0;
    }
}
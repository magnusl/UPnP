/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "DIDL_Parser.h"
#include "DIDL_Object.h"
#include "DIDL_Container.h"
#include "DIDL_Item.h"
#include "DIDL_Resource.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <wchar.h>

using namespace std;

XERCES_CPP_NAMESPACE_USE

namespace didl
{
    static int ParseDIDLDocument(const DOMDocument * doc, std::list<DIDL_Object *> & objects);
    static int ParseDIDLElement(const DOMElement * element, std::list<DIDL_Object *> & objects);
    static int ParseDIDLContainer(const DOMElement * element, std::list<DIDL_Object *> & objects);
    static int ParseDIDLItem(const DOMElement * element, std::list<DIDL_Object *> & objects);
    static int ParseDIDLMetadata(const DOMElement * element, didl::DIDL_Object * obj);

    int ParseUPnPObjectMetadata(const DOMElement * element, DIDL_Object * obj);
    int ParseDublinCoreMetadata(const DOMElement * element, didl::DIDL_Object * obj);
    int ParseDIDLResource(const DOMElement * element, didl::DIDL_Resource & resource);

    /**
     * DIDL_Parser::ParseMemoryDIDL
     *      Parses a in-memory DIDL-LITE XML file.
     */
    int DIDL_Parser::ParseMemoryDIDL(const std::string & str, std::list<DIDL_Object *> & objects)
    {
        XercesDOMParser* parser = new XercesDOMParser();
        if (!parser)
            return false;

        parser->setValidationScheme(XercesDOMParser::Val_Always);
        parser->setDoNamespaces(true);    // optional

        /* parse in-memory xml */
        MemBufInputSource input((const uint8_t *)str.c_str(), str.size(), L"devDescription", false);
        
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
        ParseDIDLDocument(doc, objects);
        delete parser;

        return 0;
    }

    /**
     * ParseDIDLDocument
     */
    static int ParseDIDLDocument(const DOMDocument * doc, std::list<DIDL_Object *> & objects)
    {
        /* find the <DIDL-LITE> element */
        DOMNodeList * elements = doc->getElementsByTagName(L"DIDL-Lite");
        if (!elements || !elements->getLength()) {
            printf("%s() no DIDL-Lite element found.\r\n", __FUNCTION__);
            return -1;
        }

        DOMNode * node = elements->item(0);
        if (node->getNodeType() != DOMNode::ELEMENT_NODE)
            return -1;

        ParseDIDLElement((DOMElement *) node, objects);
        return 0;
    }

    /**
     *
     */
    static int ParseDIDLElement(const DOMElement * element, std::list<DIDL_Object *> & objects)
    {
        DOMNodeList * nodes = element->getChildNodes();
        if (!nodes)
            return -1;

        for(size_t i = 0, count = nodes->getLength(); i < count; i++)
        {
            DOMNode * node = nodes->item(i);
            if (node->getNodeType() != DOMNode::ELEMENT_NODE) 
                continue;
            DOMElement * item = (DOMElement *) node;
            const wchar_t * name = item->getTagName();

            if (!XMLString::compareIString(L"container", name)) {
                ParseDIDLContainer(item, objects);
            } else if (!XMLString::compareIString(L"item", name)) {
                ParseDIDLItem(item, objects);
            }
        }

        return 0;
    }

    /**
     * ParseObjectAttributes
     */
    static int ParseObjectAttributes(const DOMElement * element, DIDL_Object * obj)
    {
        const wchar_t * str = element->getAttribute(L"id");
        if (!str) {
            return -1;
        }
        obj->SetId(str);

        str = element->getAttribute(L"parentID");
        if (!str) {
            return -1;
        }
        obj->SetParentId(str);

        str = element->getAttribute(L"restricted");
        if (!str) {
            return -1;
        }

        if (!XMLString::compareIString(str, L"1")) {
            obj->SetRestricted(true);
        } else {
            obj->SetRestricted(false);
        }

        if (obj->IsContainer()) {
            /* Parse attributes specific to a container */
            didl::DIDL_Container * container = (didl::DIDL_Container *) obj;
            str = element->getAttribute(L"searchable");
            if (str && XMLString::compareIString(str, L"1")) {
                container->SetSearchable(true);
            } else {
                container->SetSearchable(false);
            }
        }
        return 0;
    }


    /**
     * ParseDIDLContainer
     *      Parses a DIDL container.
     */
    static int ParseDIDLContainer(const DOMElement * element, std::list<DIDL_Object *> & objects)
    {
        DOMNodeList * nodes = element->getChildNodes();
        if (!nodes)
            return -1;

        didl::DIDL_Container * obj = new didl::DIDL_Container();

        if (ParseObjectAttributes(element, obj)) {
            printf("%s() failed to parse object attributes.\r\n", __FUNCTION__);
            delete obj;
            return -1;
        }

        for(size_t i = 0, count = nodes->getLength(); i < count; i++)
        {
            DOMNode * node = nodes->item(i);
            if (node->getNodeType() != DOMNode::ELEMENT_NODE) 
                continue;
            DOMElement * item = (DOMElement *) node;
            const wchar_t * name = item->getTagName();
            const wchar_t * ns = item->getNamespaceURI();

            if (!XMLString::compareIString(L"urn:schemas-upnp-org:metadata-1-0/upnp/", ns)) { /* UPnP Metadata for ContentDirectory */
                ParseUPnPObjectMetadata(item, obj);
            } else if (!XMLString::compareIString(L"http://purl.org/dc/elements/1.1/", ns)) { /* Dublin Core */
                ParseDublinCoreMetadata(item, obj);
            } else {
                char * name = XMLString::transcode(ns);
                printf("%s() unknown namespace %s\r\n", __FUNCTION__, name);
            }
        }

        /* add container to list */
        objects.push_back(obj);
        return 0;
    }

    /**
     *
     */
    static int ParseDIDLItem(const DOMElement * element, std::list<DIDL_Object *> & objects)
    {
        DOMNodeList * nodes = element->getChildNodes();
        if (!nodes)
            return -1;

        didl::DIDL_Item * didl_item = new didl::DIDL_Item();

        if (ParseObjectAttributes(element, didl_item)) {
            printf("%s() failed to parse object attributes.\r\n", __FUNCTION__);
            delete didl_item;
            return -1;
        }

        for(size_t i = 0, count = nodes->getLength(); i < count; i++)
        {
            DOMNode * node = nodes->item(i);
            if (node->getNodeType() != DOMNode::ELEMENT_NODE) 
                continue;
            DOMElement * item = (DOMElement *) node;
            const wchar_t * name = item->getTagName();
            const wchar_t * ns = item->getNamespaceURI();

            if (!XMLString::compareIString(L"urn:schemas-upnp-org:metadata-1-0/upnp/", ns)) { /* UPnP Metadata for ContentDirectory */
                ParseUPnPObjectMetadata(item, didl_item);
            } else if (!XMLString::compareIString(L"http://purl.org/dc/elements/1.1/", ns)) { /* Dublin Core */
                ParseDublinCoreMetadata(item, didl_item);
            } else if(!XMLString::compareIString(L"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/", ns)) { /* DIDL */
                ParseDIDLMetadata(item, didl_item);
            } 
        }

        objects.push_back(didl_item);
        return 0;
    }

    /**
     * ParseDIDLMetadata
     */
    static int ParseDIDLMetadata(const DOMElement * element, didl::DIDL_Object * obj)
    {
        const wchar_t * name = element->getLocalName();
        if (!XMLString::compareIString(name, L"res") && !obj->IsContainer()) {
            DIDL_Resource resource;
            if (ParseDIDLResource(element, resource)) {
                return -1;
            }
            ((DIDL_Item *) obj)->AddResource(resource);
        } 
        return 0;
    }
}
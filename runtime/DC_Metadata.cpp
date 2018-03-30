/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "DIDL_Parser.h"
#include "DIDL_Object.h"
#include "DIDL_Container.h"

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
    /**
     * ParseDublinCoreMetadata
     */
    int ParseDublinCoreMetadata(const DOMElement * element, didl::DIDL_Object * obj)
    {
        const wchar_t * name = element->getLocalName(), * value;
        if (!XMLString::compareIString(name, L"title")) {   /* dc:title */
            if (!(value = element->getTextContent())) {
                return -1;
            }
            obj->SetName(value);
        } else if (!XMLString::compareIString(name, L"creator")) {  /* dc:creator */

        }
        return 0;
    }
}
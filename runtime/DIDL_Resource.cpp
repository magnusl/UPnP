/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "DIDL_Resource.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <wchar.h>

XERCES_CPP_NAMESPACE_USE

namespace didl
{
    /**
     * ParseDIDLResource
     *      Parses a DIDL resource element <res>..</res>
     */
    int ParseDIDLResource(const DOMElement * element, didl::DIDL_Resource & resource)
    {
        const wchar_t * str = element->getAttribute(L"protocolInfo");
        if (!str) {
            return -1;
        }
        char * value = XMLString::transcode(str);
        resource.protocolInfo = value;
        XMLString::release(&value);

        str = element->getTextContent();
        if (str) {
            value = XMLString::transcode(str);
            resource.addr = value;
            XMLString::release(&value);
        }

        return 0;
    }
}
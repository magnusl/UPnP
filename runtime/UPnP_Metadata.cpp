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
    int ParseObjectClass(DIDL_Object * obj, const wchar_t * name);

    /**
     * ParseDIDLObjectMetadata
     */
    int ParseUPnPObjectMetadata(const DOMElement * element, DIDL_Object * obj)
    {
        const wchar_t * name = element->getLocalName();
        if (!XMLString::compareIString(L"class", name)) {
            ParseObjectClass(obj, name);
        }

        return 0;
    }

    int ParseObjectClass(DIDL_Object * obj, const wchar_t * name)
    {
        return 0;
    }
}
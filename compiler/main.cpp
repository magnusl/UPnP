/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "service.h"
#include <xercesc/util/PlatformUtils.hpp>
#include "codegen.h"
#include <fstream>
#include "ServiceImpl.h"

namespace upnp
{
    int UPnP_ParseServiceDescription(upnp::UPnP_Service & service, const char * filename);
}

XERCES_CPP_NAMESPACE_USE

int main()
{
    XMLPlatformUtils::Initialize();

    upnp::UPnP_Service service;
    if (upnp::UPnP_ParseServiceDescription(service, "C:\\Users\\Magnus\\Desktop\\avtransport.xml")) {
        std::cerr << "Failed to parse service description." << std::endl;
        return -1;
    }

    std::ofstream header("j:\\avTransport.h");
    upnp::ServiceImpl serviceImpl("avTransport", service);
    serviceImpl.generateServiceDecl( header );

    /**
    upnp::UPnP_Service service;
    upnp::UPnP_ParseServiceDescription(service, "d:\\service3.xml");

    upnp::UPnP_ControlCodegen codegen("ContentDirectory1", service);

    std::ofstream output_impl("d:\\ContentDirectory1.cpp");
    std::ofstream output_header("d:\\ContentDirectory1.h");

    codegen.GenerateClientImpl(output_impl);
    codegen.GenerateClientDecl(output_header);
    */
    return 0;
}
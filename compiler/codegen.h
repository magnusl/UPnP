/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include <iostream>
#include "service.h"

namespace upnp
{
    /**
     * UPnP_ControlCodegen
     *      Implements codegeneration for UPnP Control Points.
     */
    class UPnP_ControlCodegen
    {
    public:
        UPnP_ControlCodegen(const std::string & name, upnp::UPnP_Service & service) : m_service(service), m_name(name)
        {
        }

        /**
         * GenerateClientDecl
         */
        int GenerateClientDecl(std::ostream &);

        /**
         * GenerateClientImpl
         *      Generates the C++ code for the client implementation.
         */
        int GenerateClientImpl(std::ostream &);

    protected:

        int GenerateConstructor(std::ostream &);
        int GenerateMethodDecl(const upnp::UPnP_ServiceAction & action, std::ostream &);
        int GenerateArgumentList(const upnp::UPnP_ServiceAction & action, std::ostream &);
        int GenerateArgument(const upnp::UPnP_MethodArgument &, std::ostream &);
        int GenerateMethodBody(const upnp::UPnP_ServiceAction & action, std::ostream &);
        int GenerateArgumentSerialization(const upnp::UPnP_ServiceAction & action, std::ostream &);
        int GenerateResponseDeserialization(const upnp::UPnP_ServiceAction & action, std::ostream &);

        upnp::UPnP_Service &    m_service;
        std::string             m_name;
    };
}

#endif
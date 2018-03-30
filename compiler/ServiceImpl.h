/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _SERVICEIMPL_H_
#define _SERVICEIMPL_H_

#include <iostream>
#include <string>

#include "service.h"

namespace upnp
{
    class ServiceImpl
    {
    public:
        ServiceImpl(const std::string &, const UPnP_Service &);
        ~ServiceImpl();

        /**
         * generateServiceDecl
         *      Generates the service declaration header
         */
        bool generateServiceDecl(std::ostream &) const;
    private:
        /**
         * Each service method is wrapped by a function which deserializes the arguments and
         * serializes the return values.
         */
        bool generateWrapDecls(std::ostream &) const;
        bool generateWrapImpls(std::ostream &) const;
        bool generateWrapImpl(const UPnP_ServiceAction &, std::ostream &) const;
        /**
         * The actual actions are called by the wrapper functions.
         */
        bool generateActionDecls(std::ostream &) const;

        std::string     m_name;
        UPnP_Service    m_service;
    };
}
#endif
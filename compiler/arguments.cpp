/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "service.h"
#include <iostream>

#include <list>
#include "trim.h"

namespace upnp
{
    using namespace std;

    /**
     * GenerateArgument
     */
    bool GenerateArgument(const upnp::UPnP_MethodArgument & argument, 
        const UPnP_Service & service,
        std::ostream & os)
    {
        UPnP_StateVariable var;
        /* map the type to the actual implementation type */
        std::string name = Util::Trim( argument.m_relatedStateVariable );
        if (!service.GetStateVariableByName(name, var))
            return -1;

        const char * nativeType = UPnP_GetImplType(var.m_type);
        if (!nativeType) {
            return -1;
        }

        os << nativeType << ((argument.direction == UPnP_MethodArgument::ARG_DIR_IN) ? " " : " & ") << argument.m_name;
        return 0;
    }

    /**
     * GenerateArgumentList
     */
    int GenerateArgumentList(const upnp::UPnP_ServiceAction & action, 
        const UPnP_Service & service,
        std::ostream & os)
    {
        os << "(";
        for(list<UPnP_MethodArgument>::const_iterator it = action.m_arguments.begin();
            it != action.m_arguments.end();
            it++)
        {
            if (it != action.m_arguments.begin())
                os << ", ";
            GenerateArgument(*it, service, os);
        }
        os << ")";
        return 0;
    }
}
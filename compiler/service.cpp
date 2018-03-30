/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "service.h"
#include <iostream>

using namespace std;

namespace upnp
{
    /**
     *
     */
    bool UPnP_Service::GetStateVariableByName(const std::string & name, upnp::UPnP_StateVariable & variable) const
    {
        for(list<UPnP_StateVariable>::const_iterator it = this->m_stateVariables.begin();
            it != m_stateVariables.end();
            it++)
        {
            if (it->m_name == name) {
                variable = *it;
                return true;
            }
        }
        return false;
    }
}
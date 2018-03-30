/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <string>
#include <list>
#include "types.h"

namespace upnp
{
    /**
     *
     */
    struct UPnP_MethodArgument
    {
        enum {
            ARG_DIR_IN,
            ARG_DIR_OUT
        } direction;

        std::string     m_name;     /* argument name */
        std::string     m_relatedStateVariable;
        bool            m_retval;
    };

    /**
     *
     */
    struct UPnP_ServiceAction
    {
        std::string                     m_name;         /* Action name */
        std::list<UPnP_MethodArgument>  m_arguments;    /* Action arguments */
    };

    /**
     *
     */
    struct UPnP_StateVariable
    {
        std::string         m_name;
        std::string         m_customType;
        bool                m_typeOverride;
        bool                m_sendEvents;
        bool                m_multicast;
        UPnP_DataType       m_type;
    };

    /**
     * Service
     */
    class UPnP_Service
    {
    public:
        std::list<UPnP_ServiceAction>           m_actions;      /* actions defined by service */
        std::list<UPnP_StateVariable>   m_stateVariables;

        bool GetStateVariableByName(const std::string & name, upnp::UPnP_StateVariable & variable) const;
    };
}

#endif
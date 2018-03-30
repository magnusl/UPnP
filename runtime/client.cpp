/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "upnp.h"
#include "UPnP_XMLParser.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>
#endif

namespace upnp
{

    /**
     * UPnP_ClientContext::Init
     */
    bool UPnP_ClientContext::Init()
    {
        UPnP_XMLParser::init();

#ifdef WIN32
        _beginthread(UPnP_ClientContext::_NotifyTask, 0, this);
        _beginthread(UPnP_ClientContext::_DescriptionTask, 0, this);
        _beginthread(UPnP_ClientContext::_SearchTask, 0, this);
#endif
        return true;
    }


#ifdef WIN32
    /**
     * UPnP_ClientContext::_NotifyTask
     */
    void UPnP_ClientContext::_NotifyTask(void * arg)
    {
        UPnP_ClientContext * context = (UPnP_ClientContext *) arg;
        context->NotifyTask();
    }

    /**
     * UPnP_ClientContext::_DescriptionTask
     */
    void UPnP_ClientContext::_DescriptionTask(void * arg)
    {
        UPnP_ClientContext * context = (UPnP_ClientContext *) arg;
        context->DescriptionTask();
    }

    /**
     * UPnP_ClientContext::_SearchTask
     */
    void UPnP_ClientContext::_SearchTask(void * arg)
    {
        UPnP_ClientContext * context = (UPnP_ClientContext *) arg;
        context->SearchTask();
    }
#endif

}
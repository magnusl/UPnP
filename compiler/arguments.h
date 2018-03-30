/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _ARGUMENTS_H_
#define _ARGUMENTS_H_

namespace upnp
{
    int GenerateArgumentList(const upnp::UPnP_ServiceAction & action, const UPnP_Service & service, std::ostream & os);
}

#endif
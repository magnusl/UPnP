/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _DIDL_ITEM_H_
#define _DIDL_ITEM_H_

#include "DIDL_Object.h"
#include "DIDL_Resource.h"

#include <list>

namespace didl
{
    /**
     *
     */
    class DIDL_Item : public didl::DIDL_Object
    {
    public:
        DIDL_ObjectType GetType()   {return DIDL_Object::DIDL_ITEM;}
        bool IsContainer()          {return false;}

        void AddResource(const didl::DIDL_Resource & resource) {m_resources.push_back(resource);}

    protected:
        std::list<didl::DIDL_Resource> m_resources;
    };
}

#endif
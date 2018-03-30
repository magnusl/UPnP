/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _DIDL_CONTAINER_H_
#define _DIDL_CONTAINER_H_

#include <list>
#include "DIDL_Object.h"

namespace didl
{
    /**
     * DIDL_Container
     *
     */
    class DIDL_Container : public didl::DIDL_Object
    {
    public:
        DIDL_ObjectType GetType()   {return DIDL_Object::DIDL_CONTAINER;}
        bool IsContainer()          {return true;}

        void SetSearchable(bool searchable)     {m_searchable = searchable;}

    protected:
        std::list<DIDL_Object *>    m_objects;
        bool                        m_searchable;
    };
}

#endif
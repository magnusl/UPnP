/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _DIDL_OBJECT_H_
#define _DIDL_OBJECT_H_

#include <string>

namespace didl
{
    /**
     * DIDL_Object
     */
    class DIDL_Object
    {
    public:
        typedef enum {
            DIDL_ITEM,
            DIDL_CONTAINER,
            DIDL_IMAGEITEM,
            DIDL_PHOTOITEM,
            DIDL_AUDIOITEM,
            DIDL_MUSICTRACK,
            DIDL_AUDIOBROADCAST,
            DIDL_AUDIOBOOK,

        } DIDL_ObjectType;

        virtual DIDL_ObjectType GetType()   = 0;
        virtual bool IsContainer()          = 0;

        void SetId(const wchar_t * id)          {m_id = id;}
        void SetParentId(const wchar_t * id)    {m_parentId = id;}
        void SetRestricted(bool value)          {m_restricted = value;}
        void SetName(const wchar_t * name)      {m_name = name;}

        const std::wstring GetId()              {return m_id;}
        const std::wstring GetParentId()        {return m_parentId;}
        const std::wstring GetName()            {return m_name;}

    protected:
        /* DIDL namespace */
        std::wstring    m_id, m_parentId;
        bool            m_restricted;

        /* Dublin Core namespace */
        std::wstring    m_name;
    };
}

#endif
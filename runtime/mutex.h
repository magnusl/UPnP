/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <assert.h>

namespace upnp
{
    class Mutex
    {
    public:
        Mutex()
        {
            m_hMutex = CreateMutex(NULL, FALSE, NULL);
            assert(m_hMutex);
        }

        ~Mutex()
        {
            CloseHandle(m_hMutex);
        }

        void Lock()
        {
            WaitForSingleObject(m_hMutex, INFINITE);
        }

        void Unlock()
        {
            ReleaseMutex(m_hMutex);
        }
    protected:
#ifdef WIN32
        HANDLE m_hMutex;
#endif
    };
}

#endif
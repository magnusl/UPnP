/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <assert.h>

namespace upnp
{
    /**
     *
     */
    class Semaphore
    {
    public:

        /**
         *
         */
        Semaphore()
        {
            m_hSem = CreateSemaphore(NULL, 0, 100, NULL);
            assert(m_hSem);
        }

        /**
         *
         */
        ~Semaphore()
        {
            CloseHandle(m_hSem);
        }

        /**
         * V()
         *      Increment semaphore count
         */ 
        void V()
        {
#ifdef WIN32
            ReleaseSemaphore(m_hSem, 1, NULL);
#endif
        }

        /**
         * P()
         *      Decrement semaphore count
         */
        bool P(int timeout)
        {
#ifdef WIN32
            return (WaitForSingleObject(m_hSem, timeout == -1 ? INFINITE : timeout) == WAIT_OBJECT_0);
#endif
        }

    protected:
#ifdef WIN32
        HANDLE m_hSem;
#endif
    };
}

#endif
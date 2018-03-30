/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <list>
#include "semaphore.h"
#include "mutex.h"
#include <assert.h>

namespace upnp
{
    template<class T>
    class MessageQueue
    {
    public:

        enum {
            INFINITE_TIMEOUT = -1,
            NO_TIMEOUT = 0, 
        };

        MessageQueue(int maxItems = 100)
        {

        }

        /**
         * Get
         */
        bool Get(T & item, int timeout = INFINITE_TIMEOUT)
        {
            if (!m_sem.P(timeout)) {
                return false;
            }

            m_mutex.Lock();
            assert( !m_items.empty() );
            item = m_items.front(); m_items.pop_front();
            m_mutex.Unlock();
            return true;
        }

        /**
         * Put
         */
        bool Put(const T & item)
        {
            m_mutex.Lock();
            m_sem.V();
            m_items.push_back(item);
            m_mutex.Unlock();
            return true;
        }

        bool Empty()
        {
            return m_items.empty();
        }

    protected:
        std::list<T>    m_items;
        Semaphore       m_sem;
        Mutex           m_mutex;
    };
}
#endif
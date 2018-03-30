/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "upnp.h"
#include "socket.h"
#include <sstream>

int send_multicast_udp_packet(SOCKET sock, const char * dst, unsigned short port, const unsigned char * buf, int buflen);

namespace upnp
{

    /**
     * SetSearchTarget
     */
    void UPnP_ClientContext::SetSearchTarget(const std::string & target, unsigned int interval)
    {
        SearchEntry se;
        se.nextSearch = GetTickCount();
        se.searchTarget = target;
        se.searchInterval = interval;
        searchLock.Lock();
        searchQueue.push_back(se);
        searchLock.Unlock();
    }

    /**
     * UPnP_ClientContext::SearchTask
     *      
     */
    void UPnP_ClientContext::SearchTask()
    {
        std::string st;
        ssdp_request_t response;
        int searchSock = multicast_socket_bind("192.168.0.101", "239.255.255.250", 47361);      /* create multicast socket for broadcasting searches */
        unsigned int ts;
        while(1) {
#ifdef WIN32
            ts = GetTickCount();
#else
#error "Implementation missing"
#endif
            /* check if we should perform a search */
            if (CheckSearchStatus(st, ts)) {
                PerformSearch(searchSock, st);
            }
            /* Read unicast search response */
            if (!upnp_read_search_response(searchSock, &response, 1000)) {
                const char * location = ssdp_field_contents(&response, "location");
                upnp::UPnP_Device * dev = GetRootDeviceByLocation(location);
                if (dev) {
                    ReleaseDevice(dev);
                } else {
                    AddRootDeviceByLocation(location);
                }
            }
        }
    }

    /**
     * UPnP_ClientContext::CheckSearchStatus
     */
    bool UPnP_ClientContext::CheckSearchStatus(std::string & st, unsigned int ts)
    {
        bool bret = false;
        searchLock.Lock();
        for(std::list<SearchEntry>::iterator it = searchQueue.begin(); it != searchQueue.end(); it++)
        {
            if (it->nextSearch <= ts) {
                st = it->searchTarget;
                it->nextSearch = ts + it->searchInterval;
                bret = true;
                break;
            }
        }
        searchLock.Unlock();
        return bret;
    }

    /**
     * UPnP_ClientContext::PerformSearch
     *      Performs a search by sending a M-SEARCH SSDP to the multicast group with
     *      the search target.
     */
    void UPnP_ClientContext::PerformSearch(int sock, const std::string & st)
    {
        std::stringstream ss;
        ss << "M-SEARCH * HTTP/1.1\r\n";
        ss << "HOST: 239.255.255.250:1900\r\n";
        ss << "MAN: ssdp:discover\r\n";
        ss << "ST: " << st << "\r\n";
        ss << "MX: 3\r\n";
        ss << "\r\n";

        const std::string & str = ss.str();
        send_multicast_udp_packet(sock, "239.255.255.250", 1900, (const unsigned char *) str.c_str(), str.length());
    }
}
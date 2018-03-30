/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _UPNP_H_
#define _UPNP_H_

#include "uuid.h"
#include "ssdp.h"
#include "message_queue.h"
#include "ssdp.h"
#include "mutex.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#endif

#include <list>
#include <vector>

namespace upnp
{

    struct SearchEntry
    {
        std::string     searchTarget;
        unsigned int    nextSearch;
        unsigned int    searchInterval;
    };

    struct DescriptionEntry
    {
        enum {
            DESCRIPTION_BY_UUID,
            DESCRIPTION_BY_LOCATION
        } type;
        uuid_t          id;
        std::string     location;
    };
    
    /**
    * UPnP_Service
    *       A UPnP service.
    */
    class UPnP_Service
    {
    public:
        const std::string & GetDescriptionURL() const   {return m_descriptionURL;}
        const std::string & GetControlURL() const       {return m_controlURL;}
        const std::string & GetEventURL() const         {return m_eventSubURL;}

        void SetDescriptionURL(const std::string & str)     {m_descriptionURL = str;}
        void SetControlURL(const std::string & str)         {m_controlURL = str;}
        void SetEventURL(const std::string & str)           {m_eventSubURL = str;}

        void SetDescriptionURL(const char * str)        {m_descriptionURL = str;}
        void SetControlURL(const char * str)            {m_controlURL = str;}
        void SetEventURL(const char * str)              {m_eventSubURL = str;}

        void SetSchema(const std::string & str)         {m_schema = str;}
        void SetServiceType(const std::string & str)    {m_serviceType = str;}
        void SetVersion(int version)                    {m_version = version;}
        void SetServiceURN(const std::string & str)     {m_urn = str;}

        const std::string & GetServiceType() const      {return m_serviceType;}
        int GetVersion() const                          {return m_version;}
        const std::string & GetSchema() const           {return m_schema;}
        const std::string & GetServiceURN() const       {return m_urn;}

    protected:
        std::string     m_descriptionURL;
        std::string     m_controlURL;
        std::string     m_eventSubURL;
        std::string     m_schema;
        std::string     m_serviceType;
        std::string     m_urn;

        int         m_version;
    };

    /**
    * UPnP_Device
    *       A UPnP Device.
    */
    class UPnP_Device
    {
    public:

        UPnP_Device();
        ~UPnP_Device();

        void Init(const ssdp_request_t *);
        void Init(const std::string &);

        bool IsRootDevice()                                 {return !m_parent;}

        void SetParent(UPnP_Device * parent)                {m_parent = parent;}
        void SetFriendlyName(const char * name)             {m_friendlyName = name;}
        void SetDescriptionAddr(const std::string & addr)   {m_descriptionAddr = addr;}
        void SetDescriptionPort(unsigned short port)        {m_descriptionPort = port;}
        void SetUUID(const uuid_t & id)                     {m_id = id;}

        const std::string & GetFriendlyName()       {return m_friendlyName;}
        const std::string & GetLocation()           {return m_location;}
        const uuid_t & GetUUID()                    {return m_id;}
        const std::string & GetDescriptionAddr()    {return m_descriptionAddr;}
        unsigned short GetDescriptionPort()         {return m_descriptionPort;}
        const std::string & GetDescriptionUri()     {return m_descriptionUri;}

        bool GetServiceByServiceId(const std::string &, UPnP_Service &);

        int RefIncrease()       {int temp; lock(); temp = ++refCount; unlock(); return temp;} 
        int RefDecrease()       {int temp; lock(); temp = --refCount; unlock(); return temp;}

        std::list<UPnP_Device *> & GetEmbeddedDevices() {return m_embeddedDevices;}
        std::list<UPnP_Service> & GetServices()         {return m_services;}

        void AddService(const UPnP_Service &);
        void AddEmbeddedDevice(UPnP_Device *);

    protected:

        void lock()     {m_lock.Lock();}
        void unlock()   {m_lock.Unlock();}

        UPnP_Device *               m_parent;

        std::string                 m_descriptionAddr;
        std::string                 m_friendlyName;
        std::string                 m_location;
        std::string                 m_descriptionUri;
        uuid_t                      m_id;
        unsigned short              m_descriptionPort;

        std::list<UPnP_Service>     m_services;
        std::list<UPnP_Device *>    m_embeddedDevices;

        int                         refCount;       /* reference count */
        upnp::Mutex                 m_lock;
    };

    /**
    * UPnP_Context
    */
    class UPnP_Context
    {
    public:

        UPnP_Device * GetRootDeviceByUUID(const uuid_t &);
        UPnP_Device * GetRootDeviceByLocation(const char *);
        UPnP_Device * GetDeviceByUUID(const uuid_t &);

        void ReleaseDevice(UPnP_Device *);

        bool GetDevicesWithService(const std::string & urn, std::list<uuid_t> & devices);

    protected:

        void lock()     {m_lock.Lock();}
        void unlock()   {m_lock.Unlock();}

        int AddRootDevice(const ssdp_request_t * request);

        int UPnP_Context::RemoveDeviceByUUID(const uuid_t & id);
        int RemoveRootDevice(const uuid_t & id);
        int RemoveEmbeddedDevice(const uuid_t & id);

        int DeviceAvailable(const ssdp_request_t * request);
        int RootDeviceAvailable(const ssdp_request_t * request);
        int DeviceUnavailable(const ssdp_request_t * request);

        virtual void OnDeviceAvailable(const uuid_t &)      {}
        virtual void OnDeviceUnavailable(const uuid_t &)    {}

        std::list<UPnP_Device *>    m_devices;
        upnp::Mutex                 m_lock;
    };

    /**
     * UPnP_ClientContext
     */
    class UPnP_ClientContext : public UPnP_Context
    {
    public:

        bool Init();

        void SetSearchTarget(const std::string &, unsigned int);

        void NotifyTask();          /* handles multicast SSDPs */
        void DescriptionTask();     /* Retrives device/service descriptions using HTTP requests */
        void SearchTask();

        int ssdp_notify_multicast;

#ifdef WIN32
        static void _NotifyTask(void *);
        static void _DescriptionTask(void *);
        static void _SearchTask(void *);
#endif

    protected:

        int UPnP_ClientContext::AddRootDeviceByLocation(const std::string & location);

        /* invoked when a new service is available */
        virtual void OnServiceAvailable(const uuid_t & uuid, const std::string & urn, const UPnP_Service &) {}

        int UPnP_ClientContext::GetDeviceDescription(UPnP_Device * dev);
        int ParseDeviceDescription(UPnP_Device * dev, const std::vector<unsigned char> &);
        void NotifyServiceAvailable(UPnP_Device * dev);

        virtual void OnDeviceAvailable(const uuid_t &);
        virtual void OnDeviceUnavailable(const uuid_t &);

        bool CheckSearchStatus(std::string &, unsigned int);
        void PerformSearch(int sock, const std::string & st);

        upnp::MessageQueue<DescriptionEntry>    descriptionQueue;
        std::list<SearchEntry>                  searchQueue;
        upnp::Mutex                             searchLock;

        void UPnP_ClientContext::SSDPNotify(ssdp_request_t * request);
        void UPnP_ClientContext::SSDPEvent(ssdp_request_t * request);               
    };
}

/**
 *
 */
int upnp_read_multicast_ssdp(int socket, ssdp_request_t * request, int timeout = -1);
int upnp_read_search_response(int socket, ssdp_request_t * request, int timeout);
int read_multicast_udp_packet(SOCKET sock, unsigned char * buf, int buflen, int timeout);
SOCKET multicast_socket_bind(const char * local_addr, const char * multicast_addr, unsigned short port);
int unicast_socket_bind(const char * local_addr, unsigned short port);
int UPnP_SocketAccept(int sock, int timeout);
int UPnP_SocketClose(int sock);
int UPnP_SocketListen(int sock);

#endif
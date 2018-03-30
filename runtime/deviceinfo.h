/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _DEVICEINFO_H_
#define _DEVICEINFO_H_

/**
 * service_info
 */
typedef struct _service_info {
    const char * serviceType;
    const char * serviceId;
    const char * SCPDURL;
    const char * controlURL;
    const char * eventSubURL;

    struct _service_info * next;
} service_info;

/**
 * device_info
 */
typedef struct {
    char friendlyName[65];
    char manufacturer[65];
    char modelName[33];
    char modelNumber[33];
    char serialNumber[65];
    
    const char * manufacturerURL;
    const char * modelDescription;
    const char * udn;
} device_info;

/**
 * device_info_cleanup
 *      Deallocates the memory associated with a device_info.
 */
void device_info_cleanup(device_info *);

#endif
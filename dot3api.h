#ifndef __DOT3API_H__
#define __DOT3API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rpc.h>

typedef enum _DOT3_INTERFACE_STATE {
  dot3_interface_state_unknown = 0
} DOT3_INTERFACE_STATE, *PDOT3_INTERFACE_STATE;

typedef struct _DOT3_INTERFACE_INFO {
  GUID                 InterfaceGuid;
  WCHAR                strInterfaceDescription[256];
  DOT3_INTERFACE_STATE isState;
} DOT3_INTERFACE_INFO, *PDOT3_INTERFACE_INFO;

typedef struct _DOT3_INTERFACE_INFO_LIST {
  DWORD               dwNumberOfItems;
  DWORD               dwIndex;
  DOT3_INTERFACE_INFO InterfaceInfo[];
} DOT3_INTERFACE_INFO_LIST, *PDOT3_INTERFACE_INFO_LIST;

#define DOT3_API_VERSION_1_0 0x00000001

#define DOT3_SET_EAPHOST_DATA_ALL_USERS 0x00000001 /* Set EAP host data for all users of this profile. */

DWORD WINAPI Dot3CloseHandle(HANDLE hClientHandle, PVOID pReserved);
DWORD WINAPI Dot3DeleteProfile(HANDLE hClientHandle, const GUID *pInterfaceGuid);
DWORD WINAPI Dot3EnumInterfaces(HANDLE hClientHandle, PVOID pReserved, PDOT3_INTERFACE_INFO_LIST *ppInterfaceList);
VOID WINAPI Dot3FreeMemory(PVOID pMemory);

/*
Dot3GetCurrentProfile@16
Dot3GetInterfaceState@16
*/

DWORD WINAPI Dot3GetProfile(HANDLE hClientHandle, const GUID *pInterfaceGuid, PVOID pReserved, LPWSTR *pstrProfileXml);
DWORD WINAPI Dot3OpenHandle(DWORD dwClientVersion, PVOID pReserved, PDWORD pdwNegotiatedVersion, PHANDLE phClientHandle);

/*
Dot3QueryGUIDNCSState@8
Dot3QueryUIRequest@12
Dot3ReConnect@12
Dot3RegisterNotification@32
Dot3SetInterface@24
*/

DWORD WINAPI Dot3SetProfile(HANDLE hClientHandle, const GUID *pInterfaceGuid, LPCWSTR strProfileXml, BOOL bOverride);
DWORD WINAPI Dot3SetProfileEapUserData(HANDLE hClientHandle, const GUID *pInterfaceGuid, BYTE eapMethodType, DWORD dwEapMethodVendorId, DWORD dwEapMethodVendorType, DWORD dwEapMethodAuthorId, DWORD dwFlags, DWORD dwEapUserDataSize, const LPBYTE pbEapUserData);

/*
Dot3UIResponse@16
QueryNetconStatus@8
*/

#ifdef __cplusplus
}
#endif

#endif /* __DOT3API_H__ */

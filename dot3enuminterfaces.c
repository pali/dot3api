#include <stdio.h>

#include "dot3api.h"

int main() {

	PDOT3_INTERFACE_INFO_LIST list;
	WCHAR guid[40];
	HANDLE handle;
	DWORD version;
	DWORD res;
	DWORD i;

	res = Dot3OpenHandle(DOT3_API_VERSION_1_0, NULL, &version, &handle);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Dot3OpenHandle failed: %lu\n", res);
		return 1;
	}

	res = Dot3EnumInterfaces(handle, NULL, &list);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Dot3EnumInterfaces failed: %lu\n", res);
		Dot3CloseHandle(handle, NULL);
		return 1;
	}

	printf("Numer of items: %lu\n", list->dwNumberOfItems);
	printf("Current index: %lu\n", list->dwIndex);

	for ( i = 0; i < list->dwNumberOfItems; ++i ) {
		res = StringFromGUID2(&list->InterfaceInfo[i].InterfaceGuid, (LPOLESTR)&guid, 39);
		if (res == 0)
			guid[0] = guid[1] = 0;
		printf("Interface index: %lu\n", i);
		printf("  GUID: %ls\n", guid);
		printf("  Description: %ls\n", list->InterfaceInfo[i].strInterfaceDescription);
		printf("  State: %u\n", list->InterfaceInfo[i].isState);
	}

	Dot3FreeMemory(list);

	Dot3CloseHandle(handle, NULL);

	return 0;

}

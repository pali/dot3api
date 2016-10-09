#include <stdio.h>

#include "dot3api.h"

int main(int argc, char *argv[]) {

	GUID guid;
	WCHAR wguid[40];
	HANDLE handle;
	DWORD version;
	DWORD res;
	LPWSTR xml;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s guid\n", argv[0]);
		return 1;
	}

	if (mbstowcs(wguid, argv[1], sizeof(wguid)/sizeof(*wguid)) == (size_t)-1) {
		fprintf(stderr, "Invalid guid: %s\n", argv[1]);
		return 1;
	}

	if (CLSIDFromString(wguid, &guid) != NOERROR) {
		fprintf(stderr, "Invalid guid: %s\n", argv[1]);
		return 1;
	}

	res = Dot3OpenHandle(DOT3_API_VERSION_1_0, NULL, &version, &handle);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Dot3OpenHandle failed: %lu\n", res);
		return 1;
	}

	res = Dot3GetProfile(handle, &guid, NULL, &xml);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Dot3GetProfile failed: %lu\n", res);
		Dot3CloseHandle(handle, NULL);
		return 1;
	}

	printf("%ls\n", xml);

	Dot3FreeMemory(xml);

	Dot3CloseHandle(handle, NULL);

	return 0;

}

#include <stdio.h>

#include "dot3api.h"

int main(int argc, char *argv[]) {

	GUID guid;
	WCHAR wguid[40];
	FILE *file;
	char *buf;
	WCHAR *xml;
	HANDLE handle;
	DWORD version;
	DWORD res;
	int size;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s guid file\n", argv[0]);
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

	file = fopen(argv[2], "rb");
	if (!file) {
		fprintf(stderr, "Invalid file: %s\n", argv[2]);
		return 1;
	}

	size = fseek(file, 0, SEEK_END);
	if (size != 0) {
		fprintf(stderr, "Invalid file: %s\n", argv[2]);
		fclose(file);
		return 1;
	}

	size = ftell(file);
	if (size == 0) {
		fprintf(stderr, "Invalid file: %s\n", argv[2]);
		fclose(file);
		return 1;
	}

	rewind(file);

	buf = malloc(size);
	if (!buf) {
		fprintf(stderr, "malloc failed\n");
		fclose(file);
		return 1;
	}

	if (fread(buf, size, 1, file) != 1) {
		fprintf(stderr, "Invalid file: %s\n", argv[2]);
		fclose(file);
		free(buf);
		return 1;
	}

	fclose(file);

	xml = malloc(size * sizeof(WCHAR));
	if (!xml) {
		fprintf(stderr, "malloc failed\n");
		free(buf);
		return 1;
	}

	if (mbstowcs(xml, buf, size) == (size_t)-1) {
		fprintf(stderr, "Invalid file: %s\n", argv[2]);
		free(buf);
		free(xml);
		return 1;
	}

	free(buf);

	res = Dot3OpenHandle(DOT3_API_VERSION_1_0, NULL, &version, &handle);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Dot3OpenHandle failed: %lu\n", res);
		free(xml);
		return 1;
	}

	Dot3DeleteProfile(handle, &guid);

	res = Dot3SetProfile(handle, &guid, xml, TRUE);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Dot3SetProfile failed: %lu\n", res);
		Dot3CloseHandle(handle, NULL);
		free(xml);
		return 1;
	}

	free(xml);

	printf("Profile file %s was successfuly loaded for network guid %ls\n", argv[2], wguid);

	Dot3CloseHandle(handle, NULL);

	return 0;

}

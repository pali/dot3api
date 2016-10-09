#define _WIN32_WINNT 0x0600
#define COBJMACROS

#include <stdio.h>
#include <msxml2.h>
#include <eaptypes.h>
#include <eaphostpeerconfigapis.h>

#include "dot3api.h"

static const CLSID xCLSID_DOMDocument60 = {0x88d96a05,0xf192,0x11d4,{0xa6,0x5f,0x00,0x40,0x96,0x32,0x51,0xe5}};
static const IID xIID_IXMLDOMDocument2 = {0x2933BF95,0x7B36,0x11d2,{0xB2,0x0E,0x00,0xC0,0x4F,0x98,0x3E,0x60}};

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
	DWORD flags;
	IXMLDOMDocument2 *document;
	IXMLDOMElement *element;
	HRESULT hres;
	short sres;
	EAP_METHOD_TYPE eap;
	DWORD blobsize;
	LPBYTE blob;
	EAP_ERROR *error;

	if (argc != 3 && argc != 4) {
		fprintf(stderr, "Usage: %s guid userdata [allusers]\n", argv[0]);
		return 1;
	}

	if (argc == 4 && strcmp(argv[3], "allusers") == 0)
		flags = DOT3_SET_EAPHOST_DATA_ALL_USERS;
	else
		flags = 0;

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

	hres = CoInitialize(NULL);
	if (FAILED(hres)) {
		fprintf(stderr, "CoInitialize failed: %lu\n", HRESULT_CODE(hres));
		free(xml);
		return 1;
	}

	document = NULL;
	hres = CoCreateInstance(&xCLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, &xIID_IXMLDOMDocument2, (LPVOID *)&document);
	if (FAILED(hres) || !document) {
		fprintf(stderr, "CoCreateInstance failed: %lu\n", HRESULT_CODE(hres));
		CoUninitialize();
		free(xml);
		return 1;
	}

	hres = IXMLDOMDocument2_loadXML(document, xml, &sres);
	if (FAILED(hres) || sres != VARIANT_TRUE) {
		fprintf(stderr, "loadXML failed: %lu\n", HRESULT_CODE(hres));
		IXMLDOMDocument2_Release(document);
		CoUninitialize();
		free(xml);
		return 1;
	}

	hres = IXMLDOMDocument2_get_documentElement(document, &element);
	if (FAILED(hres)) {
		fprintf(stderr, "get_documentElement failed: %lu\n", HRESULT_CODE(hres));
		IXMLDOMDocument2_Release(document);
		CoUninitialize();
		free(xml);
		return 1;
	}

	res = EapHostPeerCredentialsXml2Blob(0, (IXMLDOMNode *)element, 0, NULL, &blobsize, &blob, &eap, &error);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "EapHostPeerCredentialsXml2Blob failed: %lu (%ls)\n", res, error->pRootCauseString);
		EapHostPeerFreeErrorMemory(error);
		free(xml);
		return 1;
	}

	free(xml);

	res = Dot3OpenHandle(DOT3_API_VERSION_1_0, NULL, &version, &handle);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Dot3OpenHandle failed: %lu\n", res);
		EapHostPeerFreeMemory(blob);
		return 1;
	}

	res = Dot3SetProfileEapUserData(handle, &guid, eap.eapType.type, eap.eapType.dwVendorId, eap.eapType.dwVendorType, eap.dwAuthorId, flags, blobsize, blob);
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Dot3SetProfileEapUserData failed: %lu\n", res);
		Dot3CloseHandle(handle, NULL);
		EapHostPeerFreeMemory(blob);
		return 1;
	}

	printf("Profile eap user data file %s was successfuly loaded for network guid %ls%s\n", argv[2], wguid, (flags ? " to all users" : ""));

	EapHostPeerFreeMemory(blob);
	IXMLDOMDocument2_Release(document);
	CoUninitialize();

	printf("Issuing reconnect\n");
	Dot3ReConnect(handle, &guid, NULL);

	Dot3CloseHandle(handle, NULL);

	return 0;

}

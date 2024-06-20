#ifndef WINDOWS_JSON_H
#define WINDOWS_JSON_H

#include <windows.h>
#include <windows.data.json.h>
#include <stdint.h>

// interface

typedef __x_ABI_CWindows_CData_CJson_CIJsonObject json_object;
typedef __x_ABI_CWindows_CData_CJson_CIJsonArray  json_array;

#define JsonCSTR(name) (HSTRING) &(json_hstring){1, sizeof(name) - 1, 0, 0, L ## name}
#define JsonRelease(obj) do {if (obj) IUnknown_Release((IUnknown *) obj);} while (0)

// NOTE: make sure you have called RoInitialize() before using any of functions

// if zero terminated, can set Length to -1
static json_object	*	JsonObjectParse(char *string, int32_t length);
static json_object	*	JsonObjectParseW(wchar_t *string, int32_t length);
static json_object	*	JsonObjectParseStr(HSTRING string);

static json_array	*	JsonArrayParse(char *string, int32_t length);
static json_array	*	JsonArrayParseW(wchar_t *string, int32_t length);
static json_array	*	JsonArrayParseStr(HSTRING string);

static json_object	*	JsonObjectGetObject(json_object *object, HSTRING name);
static json_array	*	JsonObjectGetArray(json_object *object, HSTRING name);
static HSTRING			JsonObjectGetString(json_object *object, HSTRING name);
static double			JsonObjectGetNumber(json_object *object, HSTRING name);
static boolean			JsonObjectGetBoolean(json_object *object, HSTRING name);

static uint32_t			JsonArrayGetLength(json_array *array);
static json_object	*	JsonArrayGetObject(json_array *array, uint32_t index);
static json_array	*	JsonArrayGetArray(json_array *array, uint32_t index);
static HSTRING			JsonArrayGetString(json_array *array, uint32_t index);
static double			JsonArrayGetNumber(json_array *array, uint32_t index);
static boolean			JsonArrayGetBoolean(json_array *array, uint32_t index);

// implementation

#ifndef Assert
#define Assert(Cond) ((void)(Cond))
#endif

#ifndef HR
#define HR(hr) ((void)(hr))
#endif

typedef struct {
	DWORD flags;
	DWORD length;
	DWORD padding1;
	DWORD padding2;
	wchar_t *ptr;
} json_hstring;

DEFINE_GUID(IID_IJsonObject,
			0x2289f159, 0x54de, 0x45d8, 0xab, 0xcc, 0x22, 0x60, 0x3f, 0xa0, 0x66, 0xa0);
DEFINE_GUID(IID_IJsonArray,
			0xdb1434a9, 0xe164, 0x499f, 0x93, 0xe2, 0x8a, 0x8f, 0x49, 0xbb, 0x90, 0xba);
DEFINE_GUID(IID_IVector_IJsonValue,
			0xd44662bc, 0xdce3, 0x59a8, 0x92, 0x72, 0x4b, 0x21, 0x0f, 0x33, 0x90, 0x8b);

json_object * JsonObjectParse(char *string, int32_t length) {
	if (length < 0) length = lstrlenA(string);
	int32_t wideLength = MultiByteToWideChar(CP_UTF8, 0, string, length, 0, 0);
	Assert(wideLength != 0);

	wchar_t *wide = LocalAlloc(0, wideLength * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, string, length, wide, wideLength);

	json_object *result = JsonObjectParseW(wide, wideLength);

	LocalFree(wide);
	return result;
}

json_object * JsonObjectParseW(wchar_t *string, int32_t length) {
	json_hstring wideString = {1, length < 0 ? lstrlenW(string) : length, 0, 0, string};
	return JsonObjectParseStr((HSTRING) &wideString);
}

json_object * JsonObjectParseStr(HSTRING string) {
	__x_ABI_CWindows_CData_CJson_CIJsonObjectStatics *objectStatics;
	HR(RoGetActivationFactory(JsonCSTR("Windows.Data.Json.JsonObject"),
							  &IID_IJsonObject, &objectStatics));
	
	json_object *object;
	HRESULT hr;
	if (FAILED(hr = __x_ABI_CWindows_CData_CJson_CIJsonObjectStatics_Parse(objectStatics, string,
																		   &object))) {
		object = 0;
	}
	__x_ABI_CWindows_CData_CJson_CIJsonObjectStatics_Release(objectStatics);
	
	return object;
}

json_array * JsonArrayParse(char *string, int32_t length) {
	if (length < 0) length = lstrlenA(string);
	int32_t wideLength = MultiByteToWideChar(CP_UTF8, 0, string, length, 0, 0);
	Assert(wideLength != 0);

	wchar_t *wide = LocalAlloc(0, wideLength * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, string, length, wide, wideLength);

	json_array *result = JsonArrayParseW(wide, wideLength);

	LocalFree(wide);
	return result;
}

json_array * JsonArrayParseW(wchar_t *string, int32_t length) {
	json_hstring wideString = {1, length < 0 ? lstrlenW(string) : length, 0, 0, string};
	return JsonArrayParseStr((HSTRING) &wideString);
}

json_array * JsonArrayParseStr(HSTRING string) {
	__x_ABI_CWindows_CData_CJson_CIJsonArrayStatics *arrayStatics;
	HR(RoGetActivationFactory(JsonCSTR("Windows.Data.Json.JsonArray"), &IID_IJsonArray, &arrayStatics));
	
	json_array *object;
	HRESULT hr;
	if (FAILED(hr = __x_ABI_CWindows_CData_CJson_CIJsonArrayStatics_Parse(arrayStatics, string,
																		  &object))) {
		object = 0;
	}
	__x_ABI_CWindows_CData_CJson_CIJsonArrayStatics_Release(arrayStatics);
	
	return object;
}

json_object * JsonObjectGetObject(json_object *object, HSTRING name) {
	json_object *result;
	if (!object || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonObject_GetNamedObject(object, name,
																				   &result))) {
		result = 0;
	}
	
	return result;
}

json_array * JsonObjectGetArray(json_object *object, HSTRING name) {
	json_array *result;
	if (!object || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonObject_GetNamedArray(object, name,
																				  &result))) {
		result = 0;
	}
	
	return result;
}

HSTRING JsonObjectGetString(json_object *object, HSTRING name) {
	HSTRING result;
	if (!object || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonObject_GetNamedString(object, name,
																				   &result))) {
		result = 0;
	}
	
	return result;
}

double JsonObjectGetNumber(json_object *object, HSTRING name) {
	double result;
	if (!object || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonObject_GetNamedNumber(object, name,
																				   &result))) {
		result = 0;
	}
	
	return result;
}

boolean JsonObjectGetBoolean(json_object *object, HSTRING name) {
	boolean result;
	if (!object || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonObject_GetNamedBoolean(object, name,
																					&result))) {
		result = 0;
	}
	
	return result;
}

uint32_t JsonArrayGetLength(json_array *array) {
	if (!array) return 0;
	
	uint32_t length;
	
	__FIVector_1_Windows__CData__CJson__CIJsonValue *vector;
	HR(__x_ABI_CWindows_CData_CJson_CIJsonArray_QueryInterface(array, &IID_IVector_IJsonValue,
															   &vector));
	
	if (FAILED(__FIVector_1_Windows__CData__CJson__CIJsonValue_get_Size(vector, &length))) {
		length = 0;
	}
	__FIVector_1_Windows__CData__CJson__CIJsonValue_Release(vector);

	return length;
}

json_object * JsonArrayGetObject(json_array *array, uint32_t index) {
	json_object *result;
	if (!array || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonArray_GetObjectAt(array, index, &result))) {
		result = 0;
	}
	
	return result;
}

json_array * JsonArrayGetArray(json_array *array, uint32_t index) {
	json_array *result;
	if (!array || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonArray_GetArrayAt(array, index, &result))) {
		result = 0;
	}
	
	return result;
}

HSTRING JsonArrayGetString(json_array *array, uint32_t index) {
	HSTRING result;
	if (!array || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonArray_GetStringAt(array, index, &result))) {
		result = 0;
	}
	
	return result;
}

double JsonArrayGetNumber(json_array *array, uint32_t index) {
	double result;
	if (!array || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonArray_GetNumberAt(array, index, &result))) {
		result = 0;
	}
	
	return result;
}

boolean JsonArrayGetBoolean(json_array *array, uint32_t index) {
	boolean result;
	if (!array || FAILED(__x_ABI_CWindows_CData_CJson_CIJsonArray_GetBooleanAt(array, index,
																			   &result))) {
		result = false;
	}
	
	return result;
}

#endif //WINDOWS_JSON_H

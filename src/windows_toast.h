/*
#ifndef WINDOWS_TOAST_H
#define WINDOWS_TOAST_H

#include <windows.h>
#include <windows.ui.notifications.h>
#include <propkey.h>
#include <stdint.h>
#include <stdbool.h>

// interface
typedef struct windows_toast windows_toast;
typedef __x_ABI_CWindows_CUI_CNotifications_CToastDismissalReason windows_toast_dismiss_reason;

typedef void WindowsToastOnActivated(windows_toast *toast, void *item, wchar_t *action);
typedef void WindowsToastOnDismissed(windows_toast *toast, void *item,
									  windows_toast_dismiss_reason reason);

struct windows_toast {
	// public (change these before Show() is called
	WindowsToastOnActivated *OnActivatedCallback;
	WindowsToastOnDismissed *OnDismissedCallback;

	// private
	__x_ABI_CWindows_CUI_CNotifications_CINotificationDataFactory *dataFactory;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationFactory *notificationFactory;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics *managerStatics;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier *notifier;

	__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable onActivated;
	__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs onDismissed;
};

// NOTE: make sure you have called RoInitialize() before using any of functions

// AppId is in form of "CompanyName.ProductName"
static void WindowsToastInit(windows_toast *toast, wchar_t *appName, wchar_t *appId);
static void WindowsToastDone(windows_toast *toast);
static void WindowsToastHideAll(windows_toast *toast, wchar_t *appId);

// toast xml editor/visualizer
// https://docs.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/notifications-visualizer
// xml examples & schema
// https://docs.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/adaptive-interactive-toasts
// https://docs.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/toast-schema
// https://docs.microsoft.com/en-us/uwp/schemas/tiles/toastschema/schema-root

// pass -1 for XmlLength if Xml string is 0 terminated
static void WindowsToastShowSimple(windows_toast *toast, wchar_t *xml, int32_t xmlLength,
								   wchar_t *(*data)[2], uint32_t count);

static void * WindowsToastCreate(windows_toast *toast, wchar_t *xml, int32_t xmlLength,
								wchar_t *(*data)[2], uint32_t count);

static void WindowsToastShow(windows_toast *toast, void *item);
static void WindowsToastHide(windows_toast *toast, void *item);
static bool WindowsToastUpdate(windows_toast *toast, void *item, wchar_t *(*data)[2], uint32_t count);
static void WindowsToastRelease(windows_toast *toast, void *item);

typedef struct {
	DWORD flags;
	DWORD length;
	DWORD padding1;
	DWORD padding2;
	wchar_t *ptr;
} windows_toast_hstring;

#define WindowsToastCSTR(str) (HSTRING) &(windows_toast_hstring){1, ARRAYSIZE(str) - 1, 0, 0, L ## str}

DEFINE_GUID(IID_IToastNotificationManagerStatics,
			0x50ac103f, 0xd235, 0x4598, 0xbb, 0xef, 0x98, 0xfe, 0x4d, 0x1a, 0x3a, 0xd4);
DEFINE_GUID(IID_IToastNotificationManagerStatics2,
			0x7ab93c52, 0x0e48, 0x4750, 0xba, 0x9d, 0x1a, 0x41, 0x13, 0x98, 0x18, 0x47);
DEFINE_GUID(IID_IToastNotificationFactory,
			0x04124b20, 0x82c6, 0x4229, 0xb1, 0x09, 0xfd, 0x9e, 0xd4, 0x66, 0x2b, 0x53);
DEFINE_GUID(IID_INotificationDataFactory,
			0x23c1e33a, 0x1c10, 0x46fb, 0x80, 0x40, 0xde, 0xc3, 0x84, 0x62, 0x1c, 0xf8);
DEFINE_GUID(IID_IToastNotification2,
			0x9dfb9fd1, 0x143a, 0x490e, 0x90, 0xbf, 0xb9, 0xfb, 0xa7, 0x13, 0x2d, 0xe7);
DEFINE_GUID(IID_IToastNotification4,
			0x15154935, 0x28ea, 0x4727, 0x88, 0xe9, 0xc5, 0x86, 0x80, 0xe2, 0xd1, 0x18);
DEFINE_GUID(IID_IToastNotifier2,
			0x354389c6, 0x7c01, 0x4bd5, 0x9c, 0x20, 0x60, 0x43, 0x40, 0xcd, 0x2b, 0x74);
DEFINE_GUID(IID_IToastActivatedEventArgs,
			0xe3bf92f3, 0xc197, 0x436f, 0x82, 0x65, 0x06, 0x25, 0x82, 0x4f, 0x8d, 0xac);
DEFINE_GUID(IID_IToastActivatedEventHandler,
			0xab54de2d, 0x97d9, 0x5528, 0xb6, 0xad, 0x10, 0x5a, 0xfe, 0x15, 0x65, 0x30);
DEFINE_GUID(IID_IToastDismissedEventHandler,
			0x61c2402f, 0x0ed0, 0x5a18, 0xab, 0x69, 0x59, 0xf4, 0xaa, 0x99, 0xa3, 0x68);
DEFINE_GUID(IID_IXmlDocumentIO,
			0x6cd0e74e, 0xee65, 0x4489, 0x9e, 0xbf, 0xca, 0x43, 0xe8, 0x7b, 0xa6, 0x37);

static HRESULT STDMETHODCALLTYPE WindowsToastOnActivatedQueryInterface(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable *this, REFIID riid, void **object) {
	if (object == 0) return E_POINTER;
	
	if (IsEqualGUID(riid, &IID_IToastActivatedEventHandler) ||
		IsEqualGUID(riid, &IID_IAgileObject) ||
		IsEqualGUID(riid, &IID_IUnknown)) {
		*object = this;
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

static ULONG WINAPI STDMETHODCALLTYPE WindowsToastOnActivatedAddRef(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable *this) {
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToastOnActivatedRelease(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable *this) {
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToastOnActivatedInvoke(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable *this, __x_ABI_CWindows_CUI_CNotifications_CIToastNotification *sender, IInspectable *args) {
	windows_toast *toast = CONTAINING_RECORD(this, windows_toast, onActivated);
	if (toast->OnActivatedCallback) {
		__x_ABI_CWindows_CUI_CNotifications_CIToastActivatedEventArgs *eventArgs;
		HR(IInspectable_QueryInterface(args, &IID_IToastActivatedEventArgs, &eventArgs));

		HSTRING argString;
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastActivatedEventArgs_get_Arguments(eventArgs,
																					   &argString));
		toast->OnActivatedCallback(toast, sender, (wchar_t *) WindowsGetStringRawBuffer(argString, 0));
		WindowsDeleteString(argString);

		__x_ABI_CWindows_CUI_CNotifications_CIToastActivatedEventArgs_Release(eventArgs);
	}
	
	return S_OK;
}

static __FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectableVtbl
windowsToastOnActivatedVtbl = {
	.QueryInterface = &WindowsToastOnActivatedQueryInterface,
	.AddRef         = &WindowsToastOnActivatedAddRef,
	.Release        = &WindowsToastOnActivatedRelease,
	.Invoke         = &WindowsToastOnActivatedInvoke,
};

static HRESULT STDMETHODCALLTYPE WindowsToastOnDismissedQueryInterface(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs *this, REFIID riid, void **object) {
	if (object == 0) return E_POINTER;
	
	if (IsEqualGUID(riid, &IID_IToastDismissedEventHandler) ||
		IsEqualGUID(riid, &IID_IAgileObject) ||
		IsEqualGUID(riid, &IID_IUnknown)) {
		*object = this;
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE WindowsToastOnDismissedAddRef(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs *this) {
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToastOnDismissedRelease(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs *this) {
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToastOnDismissedInvoke(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs *this, __x_ABI_CWindows_CUI_CNotifications_CIToastNotification *sender, __x_ABI_CWindows_CUI_CNotifications_CIToastDismissedEventArgs *args) {
	windows_toast *toast = CONTAINING_RECORD(this, windows_toast, onDismissed);
	if (toast->OnDismissedCallback) {
		__x_ABI_CWindows_CUI_CNotifications_CToastDismissalReason reason;
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastDismissedEventArgs_get_Reason(args, &reason));
		toast->OnDismissedCallback(toast, sender, reason);
	}
	
	return S_OK;
}

static __FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgsVtbl windowsToastOnDismissedVtbl = {
	.QueryInterface = &WindowsToastOnDismissedQueryInterface,
	.AddRef         = &WindowsToastOnDismissedAddRef,
	.Release        = &WindowsToastOnDismissedRelease,
	.Invoke         = &WindowsToastOnDismissedInvoke
};

// NOTE: minimal implementation to support only one active Iterator over Iterable
typedef struct {
	__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING iterable;
	__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING iterator;
	__FIKeyValuePair_2_HSTRING_HSTRING pair;
	wchar_t *(*pairs)[2];
	uint32_t count;
	uint32_t current;
	windows_toast_hstring keyString;
	windows_toast_hstring valueString;
} windows_toast_data;

static HRESULT STDMETHODCALLTYPE WindowsToastIterableQueryInterface(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING *this, REFIID riid, void **object) {
	return E_NOTIMPL;
}

static ULONG STDMETHODCALLTYPE WindowsToastIterableAddRef(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING *this) {
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToastIterableRelease(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING *this) {
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIterableGetIids(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING *this, ULONG *iidCount, IID **iids) {
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIterableGetRuntimeClassName(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING *this, HSTRING *className) {
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WindowsToastIterableGetTrustLevel(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING *this, TrustLevel *trustLevel) {
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIterableFirst(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING *this, __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING **result) {
	windows_toast_data *data = CONTAINING_RECORD(this, windows_toast_data, iterable);
	data->current = 0;
	*result = &data->iterator;
	return S_OK;
}

static __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl windowsToastIterableVtbl = {
	.QueryInterface      = &WindowsToastIterableQueryInterface,
	.AddRef              = &WindowsToastIterableAddRef,
	.Release             = &WindowsToastIterableRelease,
	.GetIids             = &WindowsToastIterableGetIids,
	.GetRuntimeClassName = &WindowsToastIterableGetRuntimeClassName,
	.GetTrustLevel       = &WindowsToastIterableGetTrustLevel,
	.First               = &WindowsToastIterableFirst
};

static HRESULT STDMETHODCALLTYPE WindowsToastIteratorQueryInterface(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this, REFIID riid, void **object) {
	return E_NOTIMPL;
}

static ULONG STDMETHODCALLTYPE WindowsToastIteratorAddRef(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this) {
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToastIteratorRelease(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this) {
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIteratorGetIids(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this, ULONG *iidCount, IID **iids) {
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIteratorGetRuntimeClassName(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this, HSTRING *className) {
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIteratorGetTrustLevel(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this, TrustLevel *trustLevel) {
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIteratorGetCurrent(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this, __FIKeyValuePair_2_HSTRING_HSTRING **result) {
	windows_toast_data* data = CONTAINING_RECORD(this, windows_toast_data, iterator);
	*result = &data->pair;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIteratorGetHasCurrent(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this, boolean *result) {
	windows_toast_data *data = CONTAINING_RECORD(this, windows_toast_data, iterator);
	*result = data->current < data->count;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIteratorMoveNext(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this, boolean *result) {
	windows_toast_data *data = CONTAINING_RECORD(this, windows_toast_data, iterator);
	*result = ++data->current < data->count;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE WindowsToastIteratorGetMany(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING *this, uint32_t itemsLength, __FIKeyValuePair_2_HSTRING_HSTRING **items, uint32_t *result) {
	return E_NOTIMPL;
}

static __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl windowsToastIteratorVtbl = {
	.QueryInterface      = &WindowsToastIteratorQueryInterface,
	.AddRef              = &WindowsToastIteratorAddRef,
	.Release             = &WindowsToastIteratorRelease,
	.GetIids             = &WindowsToastIteratorGetIids,
	.GetRuntimeClassName = &WindowsToastIteratorGetRuntimeClassName,
	.GetTrustLevel       = &WindowsToastIteratorGetTrustLevel,
	.get_Current         = &WindowsToastIteratorGetCurrent,
	.get_HasCurrent      = &WindowsToastIteratorGetHasCurrent,
	.MoveNext            = &WindowsToastIteratorMoveNext,
	.GetMany             = &WindowsToastIteratorGetMany
};

static HRESULT STDMETHODCALLTYPE WindowsToastPairQueryInterface(__FIKeyValuePair_2_HSTRING_HSTRING *this, REFIID riid, void **object) {
	return E_NOTIMPL;
}

static ULONG STDMETHODCALLTYPE WindowsToastPairAddRef(__FIKeyValuePair_2_HSTRING_HSTRING *this) {
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToastPairRelease(__FIKeyValuePair_2_HSTRING_HSTRING *this) {
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToastPairGetIids(__FIKeyValuePair_2_HSTRING_HSTRING *this, ULONG *iidCount, IID **iids) {
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToastPairGetRuntimeClassName(__FIKeyValuePair_2_HSTRING_HSTRING *this, HSTRING *className) {
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToastPairGetTrustLevel(__FIKeyValuePair_2_HSTRING_HSTRING *this, TrustLevel *trustLevel) {
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToastPairGetKey(__FIKeyValuePair_2_HSTRING_HSTRING *this, HSTRING *result) {
	windows_toast_data *data = CONTAINING_RECORD(this, windows_toast_data, pair);
	windows_toast_hstring *keyString = &data->keyString;
	keyString->length = lstrlenW(data->pairs[data->current][0]);
	keyString->ptr = data->pairs[data->current][0];
	*result = (HSTRING) keyString;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE WindowsToastPairGetValue(__FIKeyValuePair_2_HSTRING_HSTRING *this, HSTRING *result) {
	windows_toast_data *data = CONTAINING_RECORD(this, windows_toast_data, pair);
	windows_toast_hstring *valueString = &data->valueString;
	valueString->length = lstrlenW(data->pairs[data->current][1]);
	valueString->ptr = data->pairs[data->current][1];
	*result = (HSTRING) valueString;
	return S_OK;
}

static __FIKeyValuePair_2_HSTRING_HSTRINGVtbl windowsToastPairVtbl = {
	.QueryInterface      = &WindowsToastPairQueryInterface,
	.AddRef              = &WindowsToastPairAddRef,
	.Release             = &WindowsToastPairRelease,
	.GetIids             = &WindowsToastPairGetIids,
	.GetRuntimeClassName = &WindowsToastPairGetRuntimeClassName,
	.GetTrustLevel       = &WindowsToastPairGetTrustLevel,
	.get_Key             = &WindowsToastPairGetKey,
	.get_Value           = &WindowsToastPairGetValue
};

static __x_ABI_CWindows_CUI_CNotifications_CINotificationData * WindowsToastCreateData(windows_toast *toast, windows_toast_data *data, wchar_t *(*pairs)[2], uint32_t count) {
	data->iterable.lpVtbl = &windowsToastIterableVtbl;
	data->iterator.lpVtbl = &windowsToastIteratorVtbl;
	data->pair.lpVtbl = &windowsToastPairVtbl;
	data->pairs = pairs;
	data->count = count;
	data->keyString.flags = 1;
	data->valueString.flags = 1;

	__x_ABI_CWindows_CUI_CNotifications_CINotificationData *notificationData;
	HR(__x_ABI_CWindows_CUI_CNotifications_CINotificationDataFactory_CreateNotificationDataWithValues(
		   toast->dataFactory, &data->iterable, &notificationData));
	return notificationData;
}

static void WindowsToastInit(windows_toast *toast, wchar_t *appName, wchar_t *appId) {
	HR(SetCurrentProcessExplicitAppUserModelID(appId));

	// create Toast objects
	{
		windows_toast_hstring appIdString = {1, lstrlenW(appId), 0, 0, appId};

		HR(RoGetActivationFactory(
			   WindowsToastCSTR("Windows.UI.Notifications.NotificationData"),
			   &IID_INotificationDataFactory, &toast->dataFactory));
		HR(RoGetActivationFactory(
			   WindowsToastCSTR("Windows.UI.Notifications.ToastNotification"),
			   &IID_IToastNotificationFactory, &toast->notificationFactory));
		HR(SUCCEEDED(RoGetActivationFactory(
						 WindowsToastCSTR("Windows.UI.Notifications.ToastNotificationManager"),
						 &IID_IToastNotificationManagerStatics, &toast->managerStatics)));
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics_CreateToastNotifierWithId(toast->managerStatics, (HSTRING) &appIdString, &toast->notifier));
	}

	// setup shortcut with proper properties
	// https://docs.microsoft.com/en-us/windows/win32/shell/enable-desktop-toast-with-appusermodelid
	{
		wchar_t linkPath[MAX_PATH];
		ASSERT(0 != GetEnvironmentVariableW(L"APPDATA", linkPath, ARRAYSIZE(linkPath)));
		PathAppendW(linkPath, L"Microsoft\\Windows\\Start Menu\\Programs");
		PathAppendW(linkPath, appName);
		StrCatW(linkPath, L".lnk");

		IShellLinkW *shellLink;
		HR(CoCreateInstance(&CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER, &IID_IShellLinkW, &shellLink));

		IPersistFile *persistFile;
		HR(IShellLinkW_QueryInterface(shellLink, &IID_IPersistFile, &persistFile));

		IPropertyStore *propertyStore;
		HR(IShellLinkW_QueryInterface(shellLink, &IID_IPropertyStore, &propertyStore));

		if (GetFileAttributesW(linkPath) != INVALID_FILE_ATTRIBUTES) {
			HR(IPersistFile_Load(persistFile, linkPath, STGM_READWRITE));

			PROPVARIANT var;
			if (FAILED(IPropertyStore_GetValue(propertyStore, &PKEY_AppUserModel_ID, &var))) {
				var.vt = VT_LPWSTR;
				var.pwszVal = (wchar_t *) appId;

				HR(IPropertyStore_SetValue(propertyStore, &PKEY_AppUserModel_ID, &var));
				HR(IPropertyStore_Commit(propertyStore));
				if (IPersistFile_IsDirty(persistFile) == S_OK) {
					HR(IPersistFile_Save(persistFile, linkPath, true));
				}
			}
			else {
				PropVariantClear(&var);
			}
		} else {
			wchar_t exePath[MAX_PATH];
			GetModuleFileNameW(0, exePath, ARRAYSIZE(exePath));
			HR(IShellLinkW_SetPath(shellLink, exePath));

			HR(IShellLinkW_SetArguments(shellLink, L""));

			PathRemoveFileSpecW(exePath);
			HR(IShellLinkW_SetWorkingDirectory(shellLink, exePath));

			PROPVARIANT var = {
				.vt = VT_LPWSTR,
				.pwszVal = (wchar_t *) appId,
			};

			HR(IPropertyStore_SetValue(propertyStore, &PKEY_AppUserModel_ID, &var));
			HR(IPropertyStore_Commit(propertyStore));
			HR(IPersistFile_Save(persistFile, linkPath, true));
		}

		IPropertyStore_Release(propertyStore);
		IPersistFile_Release(persistFile);
		IShellLinkW_Release(shellLink);
	}

	toast->onActivated.lpVtbl = &windowsToastOnActivatedVtbl;
	toast->onDismissed.lpVtbl = &windowsToastOnDismissedVtbl;
}

static void WindowsToastHideAll(windows_toast *toast, wchar_t *appId) {
	windows_toast_hstring appIdString = {1, lstrlenW(appId), 0, 0, appId};

	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics2 *managerStatics2;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationHistory *notificationHistory;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics_QueryInterface(
		   toast->managerStatics, &IID_IToastNotificationManagerStatics2, &managerStatics2));
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics2_get_History(
		   managerStatics2, &notificationHistory));
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationHistory_ClearWithId(
		   notificationHistory, (HSTRING) &appIdString));
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationHistory_Release(notificationHistory);
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics2_Release(managerStatics2);
}

static void WindowsToastDone(windows_toast *toast) {
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier_Release(toast->notifier);
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics_Release(
		toast->managerStatics);
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationFactory_Release(toast->notificationFactory);
	__x_ABI_CWindows_CUI_CNotifications_CINotificationDataFactory_Release(toast->dataFactory);
}

static void WindowsToastShowSimple(windows_toast *toast, wchar_t *xml, int32_t xmlLength, wchar_t *(*data)[2], uint32_t count) {
	void *item = WindowsToastCreate(toast, xml, xmlLength, data, count);
	WindowsToastShow(toast, item);
	WindowsToastRelease(toast, item);
}

static void * WindowsToastCreate(windows_toast *toast, wchar_t *xml, int32_t xmlLength, wchar_t *(*data)[2], uint32_t count) {
	__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument *xmlDocument;
	HR(RoActivateInstance(WindowsToastCSTR("Windows.Data.Xml.Dom.XmlDocument"),
						  (IInspectable **) &xmlDocument));

	__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocumentIO *xmlIO;
	HR(__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument_QueryInterface(xmlDocument,
																	 &IID_IXmlDocumentIO, &xmlIO));

	windows_toast_hstring xmlString = {1, xmlLength < 0 ? lstrlenW(xml) : xmlLength, 0, 0, xml};
	HR(__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocumentIO_LoadXml(xmlIO, (HSTRING) &xmlString));
	
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification *notification;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationFactory_CreateToastNotification(
		   toast->notificationFactory, xmlDocument, &notification));

	EventRegistrationToken activatedToken;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_add_Activated(
		   notification, &toast->onActivated, &activatedToken));

	EventRegistrationToken dismissedToken;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_add_Dismissed(
		   notification, &toast->onDismissed, &dismissedToken));

	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification2 *notification2;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_QueryInterface(
		   notification, &IID_IToastNotification2, &notification2));

	wchar_t tag[16 + 1];
	wsprintfW(tag, L"%016I64x", (uint64_t) notification);
	windows_toast_hstring tagString = {1, ARRAYSIZE(tag) - 1, 0, 0, tag};

	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification2_put_Tag(notification2,
																		(HSTRING) &tagString));

	if (count) {
		__x_ABI_CWindows_CUI_CNotifications_CIToastNotification4 *notification4;
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_QueryInterface(
			   notification, &IID_IToastNotification4, &notification4));

		windows_toast_data toastData;
		__x_ABI_CWindows_CUI_CNotifications_CINotificationData *notificationData =
			WindowsToastCreateData(toast, &toastData, data, count);
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification4_put_Data(
			   notification4, notificationData));

		__x_ABI_CWindows_CUI_CNotifications_CINotificationData_Release(notificationData);
		__x_ABI_CWindows_CUI_CNotifications_CIToastNotification4_Release(notification4);
	}

	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification2_Release(notification2);
	__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocumentIO_Release(xmlIO);
	__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument_Release(xmlDocument);

	return notification;
}
static void WindowsToastShow(windows_toast *toast, void *item) {
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification *notification = item;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier_Show(toast->notifier, notification));
}

static void WindowsToastHide(windows_toast *toast, void *item) {
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification *notification = item;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier_Hide(toast->notifier, notification));
}

static bool WindowsToastUpdate(windows_toast *toast, void *item, wchar_t *(*data)[2], uint32_t count) {
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier2* notifier2;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier_QueryInterface(
		   toast->notifier, &IID_IToastNotifier2, &notifier2));

	wchar_t tag[16 + 1];
	wsprintfW(tag, L"%016I64x", (uint64_t) item);
	windows_toast_hstring tagString = {1, ARRAYSIZE(tag) - 1, 0, 0, tag};

	windows_toast_data toastData;
	__x_ABI_CWindows_CUI_CNotifications_CINotificationData *notificationData =
		WindowsToastCreateData(toast, &toastData, data, count);

	enum __x_ABI_CWindows_CUI_CNotifications_CNotificationUpdateResult result;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier2_UpdateWithTag(
		   notifier2, notificationData, (HSTRING) &tagString, &result));

	__x_ABI_CWindows_CUI_CNotifications_CINotificationData_Release(notificationData);
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier2_Release(notifier2);

	return result == NotificationUpdateResult_Succeeded;
}

static void WindowsToastRelease(windows_toast *toast, void *item) {
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification *notification = item;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_Release(notification);
}

#endif //WINDOWS_TOAST_H
*/

#pragma once

#include <windows.h>
#include <windows.ui.notifications.h>

// interface

typedef struct WindowsToast WindowsToast;
typedef __x_ABI_CWindows_CUI_CNotifications_CToastDismissalReason WindowsToast_DismissReason;

typedef void WindowsToast_OnActivated(WindowsToast* Toast, void* Item, LPCWSTR Action);
typedef void WindowsToast_OnDismissed(WindowsToast* Toast, void* Item, WindowsToast_DismissReason Reason);

struct WindowsToast
{
	// public (change these before Show() is called
	WindowsToast_OnActivated* OnActivatedCallback;
	WindowsToast_OnDismissed* OnDismissedCallback;

	// private
	__x_ABI_CWindows_CUI_CNotifications_CINotificationDataFactory* DataFactory;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationFactory* NotificationFactory;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics* ManagerStatics;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier* Notifier;

	__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable OnActivated;
	__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs OnDismissed;
};

// NOTE: make sure you have called RoInitialize() before using any of functions

// AppId is in form of "CompanyName.ProductName"
static void WindowsToast_Init(WindowsToast* Toast, LPCWSTR AppName, LPCWSTR AppId);
static void WindowsToast_Done(WindowsToast* Toast);
static void WindowsToast_HideAll(WindowsToast* Toast, LPCWSTR AppId);

// toast xml editor/visualizer
// https://docs.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/notifications-visualizer
// xml examples & schema
// https://docs.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/adaptive-interactive-toasts
// https://docs.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/toast-schema
// https://docs.microsoft.com/en-us/uwp/schemas/tiles/toastschema/schema-root

// pass -1 for XmlLength if Xml string is 0 terminated
static void WindowsToast_ShowSimple(WindowsToast* Toast, LPCWSTR Xml, int XmlLength, LPCWSTR(*Data)[2], UINT32 Count);

static void* WindowsToast_Create(WindowsToast* Toast, LPCWSTR Xml, int XmlLength, LPCWSTR (*Data)[2], UINT32 Count);
static void WindowsToast_Show(WindowsToast* Toast, void* Item);
static void WindowsToast_Hide(WindowsToast* Toast, void* Item);
static BOOL WindowsToast_Update(WindowsToast* Toast, void* Item, LPCWSTR (*Data)[2], UINT32 Count);
static void WindowsToast_Release(WindowsToast* Toast, void* Item);

// implementation

#include <shlobj.h>
#include <propkey.h>
#include <propvarutil.h>
#include <shlwapi.h>
#include <roapi.h>

#pragma comment (lib, "shlwapi.lib")
#pragma comment (lib, "ole32.lib")
#pragma comment (lib, "runtimeobject.lib")

#ifndef Assert
#define Assert(Cond) ((void)(Cond))
#endif

#ifndef HR
#define HR(hr) ((void)(hr))
#endif

typedef struct {
	DWORD Flags;
	DWORD Length;
	DWORD Padding1;
	DWORD Padding2;
	LPCWSTR Ptr;
} WindowsToastHSTRING;

#define WindowsToastCSTR(Str) (HSTRING)&(WindowsToastHSTRING){ 1, ARRAYSIZE(Str)-1, 0, 0, L ## Str }

DEFINE_GUID(IID_IToastNotificationManagerStatics,  0x50ac103f, 0xd235, 0x4598, 0xbb, 0xef, 0x98, 0xfe, 0x4d, 0x1a, 0x3a, 0xd4);
DEFINE_GUID(IID_IToastNotificationManagerStatics2, 0x7ab93c52, 0x0e48, 0x4750, 0xba, 0x9d, 0x1a, 0x41, 0x13, 0x98, 0x18, 0x47);
DEFINE_GUID(IID_IToastNotificationFactory,         0x04124b20, 0x82c6, 0x4229, 0xb1, 0x09, 0xfd, 0x9e, 0xd4, 0x66, 0x2b, 0x53);
DEFINE_GUID(IID_INotificationDataFactory,          0x23c1e33a, 0x1c10, 0x46fb, 0x80, 0x40, 0xde, 0xc3, 0x84, 0x62, 0x1c, 0xf8);
DEFINE_GUID(IID_IToastNotification2,               0x9dfb9fd1, 0x143a, 0x490e, 0x90, 0xbf, 0xb9, 0xfb, 0xa7, 0x13, 0x2d, 0xe7);
DEFINE_GUID(IID_IToastNotification4,               0x15154935, 0x28ea, 0x4727, 0x88, 0xe9, 0xc5, 0x86, 0x80, 0xe2, 0xd1, 0x18);
DEFINE_GUID(IID_IToastNotifier2,                   0x354389c6, 0x7c01, 0x4bd5, 0x9c, 0x20, 0x60, 0x43, 0x40, 0xcd, 0x2b, 0x74);
DEFINE_GUID(IID_IToastActivatedEventArgs,          0xe3bf92f3, 0xc197, 0x436f, 0x82, 0x65, 0x06, 0x25, 0x82, 0x4f, 0x8d, 0xac);
DEFINE_GUID(IID_IToastActivatedEventHandler,       0xab54de2d, 0x97d9, 0x5528, 0xb6, 0xad, 0x10, 0x5a, 0xfe, 0x15, 0x65, 0x30);
DEFINE_GUID(IID_IToastDismissedEventHandler,       0x61c2402f, 0x0ed0, 0x5a18, 0xab, 0x69, 0x59, 0xf4, 0xaa, 0x99, 0xa3, 0x68);
DEFINE_GUID(IID_IXmlDocumentIO,                    0x6cd0e74e, 0xee65, 0x4489, 0x9e, 0xbf, 0xca, 0x43, 0xe8, 0x7b, 0xa6, 0x37);

static HRESULT STDMETHODCALLTYPE WindowsToast__OnActivated_QueryInterface(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable* This, REFIID Riid, void** Object)
{
	if (Object == NULL)
	{
		return E_POINTER;
	}
	if (IsEqualGUID(Riid, &IID_IToastActivatedEventHandler) ||
		IsEqualGUID(Riid, &IID_IAgileObject) ||
		IsEqualGUID(Riid, &IID_IUnknown))
	{
		*Object = This;
		return S_OK;
	}
	return E_NOINTERFACE;
}

static ULONG WINAPI STDMETHODCALLTYPE WindowsToast__OnActivated_AddRef(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable* This)
{
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToast__OnActivated_Release(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable* This)
{
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__OnActivated_Invoke(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectable* This, __x_ABI_CWindows_CUI_CNotifications_CIToastNotification* Sender, IInspectable* Args)
{
	WindowsToast* Toast = CONTAINING_RECORD(This, WindowsToast, OnActivated);
	if (Toast->OnActivatedCallback)
	{
		__x_ABI_CWindows_CUI_CNotifications_CIToastActivatedEventArgs* EventArgs;
		HR(IInspectable_QueryInterface(Args, &IID_IToastActivatedEventArgs, &EventArgs));

		HSTRING ArgString;
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastActivatedEventArgs_get_Arguments(EventArgs, &ArgString));
		Toast->OnActivatedCallback(Toast, Sender, WindowsGetStringRawBuffer(ArgString, NULL));
		WindowsDeleteString(ArgString);

		__x_ABI_CWindows_CUI_CNotifications_CIToastActivatedEventArgs_Release(EventArgs);
	}
	return S_OK;
}

static __FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_IInspectableVtbl WindowsToast__OnActivatedVtbl =
{
	.QueryInterface = &WindowsToast__OnActivated_QueryInterface,
	.AddRef         = &WindowsToast__OnActivated_AddRef,
	.Release        = &WindowsToast__OnActivated_Release,
	.Invoke         = &WindowsToast__OnActivated_Invoke,
};

static HRESULT STDMETHODCALLTYPE WindowsToast__OnDismissed_QueryInterface(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs* This, REFIID Riid, void** Object)
{
	if (Object == NULL)
	{
		return E_POINTER;
	}
	if (IsEqualGUID(Riid, &IID_IToastDismissedEventHandler) ||
		IsEqualGUID(Riid, &IID_IAgileObject) ||
		IsEqualGUID(Riid, &IID_IUnknown))
	{
		*Object = This;
		return S_OK;
	}
	return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE WindowsToast__OnDismissed_AddRef(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs* This)
{
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToast__OnDismissed_Release(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs* This)
{
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__OnDismissed_Invoke(__FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgs* This, __x_ABI_CWindows_CUI_CNotifications_CIToastNotification* Sender, __x_ABI_CWindows_CUI_CNotifications_CIToastDismissedEventArgs* Args)
{
	WindowsToast* Toast = CONTAINING_RECORD(This, WindowsToast, OnDismissed);
	if (Toast->OnDismissedCallback)
	{
		__x_ABI_CWindows_CUI_CNotifications_CToastDismissalReason Reason;
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastDismissedEventArgs_get_Reason(Args, &Reason));
		Toast->OnDismissedCallback(Toast, Sender, Reason);
	}
	return S_OK;
}

static __FITypedEventHandler_2_Windows__CUI__CNotifications__CToastNotification_Windows__CUI__CNotifications__CToastDismissedEventArgsVtbl WindowsToast__OnDismissedVtbl =
{
	.QueryInterface = &WindowsToast__OnDismissed_QueryInterface,
	.AddRef         = &WindowsToast__OnDismissed_AddRef,
	.Release        = &WindowsToast__OnDismissed_Release,
	.Invoke         = &WindowsToast__OnDismissed_Invoke,
};

// NOTE: minimal implementation to support only one active Iterator over Iterable
typedef struct {
	__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING Iterable;
	__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING Iterator;
	__FIKeyValuePair_2_HSTRING_HSTRING Pair;
	LPCWSTR (*Pairs)[2];
	UINT32 Count;
	UINT32 Current;
	WindowsToastHSTRING KeyString;
	WindowsToastHSTRING ValueString;
} WindowsToast__Data;

static HRESULT STDMETHODCALLTYPE WindowsToast__IterableQueryInterface(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING* This, REFIID Riid, void** Object)
{
	return E_NOTIMPL;
}

static ULONG STDMETHODCALLTYPE WindowsToast__IterableAddRef(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING* This)
{
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToast__IterableRelease(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING* This)
{
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__IterableGetIids(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING* This, ULONG* IidCount, IID** Iids)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__IterableGetRuntimeClassName(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING* This, HSTRING* ClassName)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WindowsToast__IterableGetTrustLevel(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING* This, TrustLevel* TrustLevel)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__IterableFirst(__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING* This, __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING** Result)
{
	WindowsToast__Data* Data = CONTAINING_RECORD(This, WindowsToast__Data, Iterable);
	Data->Current = 0;
	*Result = &Data->Iterator;
	return S_OK;
}

static __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl WindowsToast__IterableVtbl =
{
	.QueryInterface      = &WindowsToast__IterableQueryInterface,
	.AddRef              = &WindowsToast__IterableAddRef,
	.Release             = &WindowsToast__IterableRelease,
	.GetIids             = &WindowsToast__IterableGetIids,
	.GetRuntimeClassName = &WindowsToast__IterableGetRuntimeClassName,
	.GetTrustLevel       = &WindowsToast__IterableGetTrustLevel,
	.First               = &WindowsToast__IterableFirst,
};

static HRESULT STDMETHODCALLTYPE WindowsToast_IteratorQueryInterface(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This, REFIID riid, void** Object)
{
	return E_NOTIMPL;
}

static ULONG STDMETHODCALLTYPE WindowsToast_IteratorAddRef(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This)
{
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToast_IteratorRelease(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This)
{
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToast_IteratorGetIids(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This, ULONG* IidCount, IID** Iids)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToast_IteratorGetRuntimeClassName(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This, HSTRING* ClassName)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToast_IteratorGetTrustLevel(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This, TrustLevel* TrustLevel)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToast_IteratorGetCurrent(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This, __FIKeyValuePair_2_HSTRING_HSTRING** Result)
{
	WindowsToast__Data* Data = CONTAINING_RECORD(This, WindowsToast__Data, Iterator);
	*Result = &Data->Pair;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE WindowsToast_IteratorGetHasCurrent(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This, boolean* Result)
{
	WindowsToast__Data* Data = CONTAINING_RECORD(This, WindowsToast__Data, Iterator);
	*Result = Data->Current < Data->Count;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE WindowsToast_IteratorMoveNext(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This, boolean* Result)
{
	WindowsToast__Data* Data = CONTAINING_RECORD(This, WindowsToast__Data, Iterator);
	*Result = ++Data->Current < Data->Count;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE WindowsToast_IteratorGetMany(__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING* This, UINT32 ItemsLength, __FIKeyValuePair_2_HSTRING_HSTRING** Items, UINT32* Result)
{
	return E_NOTIMPL;
}

static __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl WindowsToast__IteratorVtbl =
{
	.QueryInterface      = &WindowsToast_IteratorQueryInterface,
	.AddRef              = &WindowsToast_IteratorAddRef,
	.Release             = &WindowsToast_IteratorRelease,
	.GetIids             = &WindowsToast_IteratorGetIids,
	.GetRuntimeClassName = &WindowsToast_IteratorGetRuntimeClassName,
	.GetTrustLevel       = &WindowsToast_IteratorGetTrustLevel,
	.get_Current         = &WindowsToast_IteratorGetCurrent,
	.get_HasCurrent      = &WindowsToast_IteratorGetHasCurrent,
	.MoveNext            = &WindowsToast_IteratorMoveNext,
	.GetMany             = &WindowsToast_IteratorGetMany,
};

static HRESULT STDMETHODCALLTYPE WindowsToast__PairQueryInterface(__FIKeyValuePair_2_HSTRING_HSTRING* This, REFIID Riid, void** Object)
{
	return E_NOTIMPL;
}

static ULONG STDMETHODCALLTYPE WindowsToast__PairAddRef(__FIKeyValuePair_2_HSTRING_HSTRING* This)
{
	return 1;
}

static ULONG STDMETHODCALLTYPE WindowsToast__PairRelease(__FIKeyValuePair_2_HSTRING_HSTRING* This)
{
	return 1;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__PairGetIids(__FIKeyValuePair_2_HSTRING_HSTRING* This, ULONG* IidCount, IID** Iids)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__PairGetRuntimeClassName(__FIKeyValuePair_2_HSTRING_HSTRING* This, HSTRING* ClassName)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__PairGetTrustLevel(__FIKeyValuePair_2_HSTRING_HSTRING* This, TrustLevel* TrustLevel)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__PairGetKey(__FIKeyValuePair_2_HSTRING_HSTRING* This, HSTRING* Result)
{
	WindowsToast__Data* Data = CONTAINING_RECORD(This, WindowsToast__Data, Pair);
	WindowsToastHSTRING* KeyString = &Data->KeyString;
	KeyString->Length = lstrlenW(Data->Pairs[Data->Current][0]);
	KeyString->Ptr = Data->Pairs[Data->Current][0];
	*Result = (HSTRING)KeyString;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE WindowsToast__PairGetValue(__FIKeyValuePair_2_HSTRING_HSTRING* This, HSTRING* Result)
{
	WindowsToast__Data* Data = CONTAINING_RECORD(This, WindowsToast__Data, Pair);
	WindowsToastHSTRING* ValueString = &Data->ValueString;
	ValueString->Length = lstrlenW(Data->Pairs[Data->Current][1]);
	ValueString->Ptr = Data->Pairs[Data->Current][1];
	*Result = (HSTRING)ValueString;
	return S_OK;
}

static __FIKeyValuePair_2_HSTRING_HSTRINGVtbl WindowsToast__PairVtbl =
{
	.QueryInterface      = &WindowsToast__PairQueryInterface,
	.AddRef              = &WindowsToast__PairAddRef,
	.Release             = &WindowsToast__PairRelease,
	.GetIids             = &WindowsToast__PairGetIids,
	.GetRuntimeClassName = &WindowsToast__PairGetRuntimeClassName,
	.GetTrustLevel       = &WindowsToast__PairGetTrustLevel,
	.get_Key             = &WindowsToast__PairGetKey,
	.get_Value           = &WindowsToast__PairGetValue,
};

static __x_ABI_CWindows_CUI_CNotifications_CINotificationData* WindowsToast__CreateData(WindowsToast* Toast, WindowsToast__Data* Data, LPCWSTR(*Pairs)[2], UINT32 Count)
{
	Data->Iterable.lpVtbl = &WindowsToast__IterableVtbl;
	Data->Iterator.lpVtbl = &WindowsToast__IteratorVtbl;
	Data->Pair.lpVtbl = &WindowsToast__PairVtbl;
	Data->Pairs = Pairs;
	Data->Count = Count;
	Data->KeyString.Flags = 1;
	Data->ValueString.Flags = 1;

	__x_ABI_CWindows_CUI_CNotifications_CINotificationData* NotificationData;
	HR(__x_ABI_CWindows_CUI_CNotifications_CINotificationDataFactory_CreateNotificationDataWithValues(Toast->DataFactory, &Data->Iterable, &NotificationData));
	return NotificationData;
}

static void WindowsToast_Init(WindowsToast* Toast, LPCWSTR AppName, LPCWSTR AppId)
{
	HR(SetCurrentProcessExplicitAppUserModelID(AppId));

	// create Toast objects
	{
		WindowsToastHSTRING AppIdString = { 1, lstrlenW(AppId), 0, 0, AppId };

		HR(RoGetActivationFactory(WindowsToastCSTR("Windows.UI.Notifications.NotificationData"), &IID_INotificationDataFactory, &Toast->DataFactory));
		HR(RoGetActivationFactory(WindowsToastCSTR("Windows.UI.Notifications.ToastNotification"), &IID_IToastNotificationFactory, &Toast->NotificationFactory));
		HR(SUCCEEDED(RoGetActivationFactory(WindowsToastCSTR("Windows.UI.Notifications.ToastNotificationManager"), &IID_IToastNotificationManagerStatics, &Toast->ManagerStatics)));
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics_CreateToastNotifierWithId(Toast->ManagerStatics, (HSTRING)&AppIdString, &Toast->Notifier));
	}

	// setup shortcut with proper properties
	// https://docs.microsoft.com/en-us/windows/win32/shell/enable-desktop-toast-with-appusermodelid
	{
		WCHAR LinkPath[MAX_PATH];
		Assert(0 != GetEnvironmentVariableW(L"APPDATA", LinkPath, ARRAYSIZE(LinkPath)));
		PathAppendW(LinkPath, L"Microsoft\\Windows\\Start Menu\\Programs");
		PathAppendW(LinkPath, AppName);
		StrCatW(LinkPath, L".lnk");

		IShellLinkW* ShellLink;
		HR(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLinkW, &ShellLink));

		IPersistFile* PersistFile;
		HR(IShellLinkW_QueryInterface(ShellLink, &IID_IPersistFile, &PersistFile));

		IPropertyStore* PropertyStore;
		HR(IShellLinkW_QueryInterface(ShellLink, &IID_IPropertyStore, &PropertyStore));

		if (GetFileAttributesW(LinkPath) != INVALID_FILE_ATTRIBUTES)
		{
			HR(IPersistFile_Load(PersistFile, LinkPath, STGM_READWRITE));

			PROPVARIANT Var;
			if (FAILED(IPropertyStore_GetValue(PropertyStore, &PKEY_AppUserModel_ID, &Var)))
			{
				Var.vt = VT_LPWSTR;
				Var.pwszVal = (LPWSTR)AppId;

				HR(IPropertyStore_SetValue(PropertyStore, &PKEY_AppUserModel_ID, &Var));
				HR(IPropertyStore_Commit(PropertyStore));
				if (IPersistFile_IsDirty(PersistFile) == S_OK)
				{
					HR(IPersistFile_Save(PersistFile, LinkPath, TRUE));
				}
			}
			else
			{
				PropVariantClear(&Var);
			}
		}
		else
		{
			WCHAR ExePath[MAX_PATH];
			GetModuleFileNameW(NULL, ExePath, ARRAYSIZE(ExePath));
			HR(IShellLinkW_SetPath(ShellLink, ExePath));

			HR(IShellLinkW_SetArguments(ShellLink, L""));

			PathRemoveFileSpecW(ExePath);
			HR(IShellLinkW_SetWorkingDirectory(ShellLink, ExePath));

			PROPVARIANT Var =
			{
				.vt = VT_LPWSTR,
				.pwszVal = (LPWSTR)AppId,
			};

			HR(IPropertyStore_SetValue(PropertyStore, &PKEY_AppUserModel_ID, &Var));
			HR(IPropertyStore_Commit(PropertyStore));
			HR(IPersistFile_Save(PersistFile, LinkPath, TRUE));
		}

		IPropertyStore_Release(PropertyStore);
		IPersistFile_Release(PersistFile);
		IShellLinkW_Release(ShellLink);
	}

	Toast->OnActivated.lpVtbl = &WindowsToast__OnActivatedVtbl;
	Toast->OnDismissed.lpVtbl = &WindowsToast__OnDismissedVtbl;
}

static void WindowsToast_HideAll(WindowsToast* Toast, LPCWSTR AppId)
{
	WindowsToastHSTRING AppIdString = { 1, lstrlenW(AppId), 0, 0, AppId };

	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics2* ManagerStatics2;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationHistory* NotificationHistory;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics_QueryInterface(Toast->ManagerStatics, &IID_IToastNotificationManagerStatics2, &ManagerStatics2));
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics2_get_History(ManagerStatics2, &NotificationHistory));
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationHistory_ClearWithId(NotificationHistory, (HSTRING)&AppIdString));
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationHistory_Release(NotificationHistory);
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics2_Release(ManagerStatics2);
}

static void WindowsToast_Done(WindowsToast* Toast)
{
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier_Release(Toast->Notifier);
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationManagerStatics_Release(Toast->ManagerStatics);
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationFactory_Release(Toast->NotificationFactory);
	__x_ABI_CWindows_CUI_CNotifications_CINotificationDataFactory_Release(Toast->DataFactory);
}

static void WindowsToast_ShowSimple(WindowsToast* Toast, LPCWSTR Xml, int XmlLength, LPCWSTR(*Data)[2], UINT32 Count)
{
	void* Item = WindowsToast_Create(Toast, Xml, XmlLength, Data, Count);
	WindowsToast_Show(Toast, Item);
	WindowsToast_Release(Toast, Item);
}

static void* WindowsToast_Create(WindowsToast* Toast, LPCWSTR Xml, int XmlLength, LPCWSTR (*Data)[2], UINT32 Count)
{
	__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument* XmlDocument;
	HR(RoActivateInstance(WindowsToastCSTR("Windows.Data.Xml.Dom.XmlDocument"), (IInspectable**)&XmlDocument));

	__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocumentIO* XmlIO;
	HR(__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument_QueryInterface(XmlDocument, &IID_IXmlDocumentIO, &XmlIO));

	WindowsToastHSTRING XmlString = { 1, XmlLength < 0 ? lstrlenW(Xml) : XmlLength, 0, 0, Xml };
	HR(__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocumentIO_LoadXml(XmlIO, (HSTRING)&XmlString));
	
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification* Notification;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotificationFactory_CreateToastNotification(Toast->NotificationFactory, XmlDocument, &Notification));

	EventRegistrationToken ActivatedToken;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_add_Activated(Notification, &Toast->OnActivated, &ActivatedToken));

	EventRegistrationToken DismissedToken;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_add_Dismissed(Notification, &Toast->OnDismissed, &DismissedToken));

	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification2* Notification2;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_QueryInterface(Notification, &IID_IToastNotification2, &Notification2));

	WCHAR Tag[16 + 1];
	wsprintfW(Tag, L"%016I64x", (UINT64)Notification);
	WindowsToastHSTRING TagString = { 1, ARRAYSIZE(Tag) - 1, 0, 0, Tag };

	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification2_put_Tag(Notification2, (HSTRING)&TagString));

	if (Count)
	{
		__x_ABI_CWindows_CUI_CNotifications_CIToastNotification4* Notification4;
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_QueryInterface(Notification, &IID_IToastNotification4, &Notification4));

		WindowsToast__Data ToastData;
		__x_ABI_CWindows_CUI_CNotifications_CINotificationData* NotificationData = WindowsToast__CreateData(Toast, &ToastData, Data, Count);
		HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotification4_put_Data(Notification4, NotificationData));

		__x_ABI_CWindows_CUI_CNotifications_CINotificationData_Release(NotificationData);
		__x_ABI_CWindows_CUI_CNotifications_CIToastNotification4_Release(Notification4);
	}

	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification2_Release(Notification2);
	__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocumentIO_Release(XmlIO);
	__x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument_Release(XmlDocument);

	return Notification;
}
static void WindowsToast_Show(WindowsToast* Toast, void* Item)
{
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification* Notification = Item;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier_Show(Toast->Notifier, Notification));
}

static void WindowsToast_Hide(WindowsToast* Toast, void* Item)
{
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification* Notification = Item;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier_Hide(Toast->Notifier, Notification));
}

static BOOL WindowsToast_Update(WindowsToast* Toast, void* Item, LPCWSTR(*Data)[2], UINT32 Count)
{
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier2* Notifier2;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier_QueryInterface(Toast->Notifier, &IID_IToastNotifier2, &Notifier2));

	WCHAR Tag[16 + 1];
	wsprintfW(Tag, L"%016I64x", (UINT64)Item);
	WindowsToastHSTRING TagString = { 1, ARRAYSIZE(Tag) - 1, 0, 0, Tag };

	WindowsToast__Data ToastData;
	__x_ABI_CWindows_CUI_CNotifications_CINotificationData* NotificationData = WindowsToast__CreateData(Toast, &ToastData, Data, Count);

	enum __x_ABI_CWindows_CUI_CNotifications_CNotificationUpdateResult Result;
	HR(__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier2_UpdateWithTag(Notifier2, NotificationData, (HSTRING)&TagString, &Result));

	__x_ABI_CWindows_CUI_CNotifications_CINotificationData_Release(NotificationData);
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotifier2_Release(Notifier2);

	return Result == NotificationUpdateResult_Succeeded;
}

static void WindowsToast_Release(WindowsToast* Toast, void* Item)
{
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification* Notification = Item;
	__x_ABI_CWindows_CUI_CNotifications_CIToastNotification_Release(Notification);
}

#ifndef _ARMACCESS__
#define _ARMACCESS__

/* These typedefs show the compiler the formats of the functions. */ 
typedef int (__stdcall *CheckCodeFn)(const char *name, const char *code); 
typedef int (__stdcall *VerifyKeyFn)(const char *name, const char *code); 
typedef int (__stdcall *InstallKeyFn)(const char *name, const char *code); 
typedef int (__stdcall *InstallKeyLaterFn)(const char *name, const char *code); 
typedef int (__stdcall *UninstallKeyFn)(void); 
typedef int (__stdcall *SetDefaultKeyFn)(void); 
typedef int (__stdcall *UpdateEnvironmentFn)(void); 
typedef int (__stdcall *IncrementCounterFn)(void); 
typedef int  (__stdcall *CopiesRunningFn)(void); 
typedef int (__stdcall *ChangeHardwareLockFn)(void); 
typedef DWORD (__stdcall *GetShellProcessIDFn)(void); 
typedef int (__stdcall *FixClockFn)(const char *fixclockkey); 
typedef DWORD (__stdcall *RawFingerprintInfoFn)(DWORD item); 
typedef int (__stdcall *SetUserStringFn)(int which, const char *string); 
typedef DWORD (__stdcall *GetUserStringFn)(int which, char *buffer, DWORD bufferlength); 
typedef int (__stdcall *WriteHardwareChangeLogFn)(const char *filename); 
typedef int (__stdcall *ConnectedToServerFn)(void); 
typedef int (__stdcall *CallBuyNowURLFn)(HWND parent); 
typedef int (__stdcall *CallCustomerServiceURLFn)(HWND parent); 
typedef void (__stdcall *ShowReminderMessageFn)(HWND parent); 
typedef void (__stdcall *ShowReminderMessage2Fn)(HWND parent); 
typedef int (__stdcall *ExpireCurrentKeyFn)(void); 
typedef int (__stdcall *ShowEnterKeyDialogFn)(HWND parent); 

#endif
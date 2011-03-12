//------------------------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------------------------
#include <windows.h>
#include "alut.h"
#include "ac3_stream.h"

//------------------------------------------------------------------------------------------------
// global declarations
//------------------------------------------------------------------------------------------------
LPEAXAC3FNTABLE			g_lpEAXAC3FnTable;		// Pointer to the EAX-AC3 function table
ALboolean				bEAXAC3ExtPresent;		// AC3 device presentation

//------------------------------------------------------------------------------------------------
// function declarations
//------------------------------------------------------------------------------------------------
extern "C" void adas_Set_Last_Error(char *p_Text);

//------------------------------------------------------------------------------------------------
// init
//------------------------------------------------------------------------------------------------
int adas_AC3_Init(void)
{
	LPALEAXAC3GETFUNCTIONTABLE	ALEAXAC3GetFunctionTable;

	g_lpEAXAC3FnTable = new EAXAC3FNTABLE;

	if (!g_lpEAXAC3FnTable)
	{
		adas_Set_Last_Error("Out of Memory");
		return 0;
	}

	bEAXAC3ExtPresent = alIsExtensionPresent((ALubyte*)"EAX-AC3");

	if (!bEAXAC3ExtPresent)
	{
		adas_Set_Last_Error("No EAX-AC3 Extension found");
		return 0;
	}

	ALubyte szFnName[] = "alEAXAC3GetFunctionTable";
	ALEAXAC3GetFunctionTable = (LPALEAXAC3GETFUNCTIONTABLE)alGetProcAddress(szFnName);

	if (!ALEAXAC3GetFunctionTable)
	{
		adas_Set_Last_Error("Cannot find alEAXAC3GetFunctionTable function");
		return 0;
	}

	if (!ALEAXAC3GetFunctionTable(g_lpEAXAC3FnTable))
	{
		adas_Set_Last_Error("Failed to retrieve the EAX-AC3 Function Table");
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// release
//------------------------------------------------------------------------------------------------
void adas_AC3_Release(void)
{
	if (g_lpEAXAC3FnTable)
		delete g_lpEAXAC3FnTable;
}

//------------------------------------------------------------------------------------------------
// enum AC3 devices
//------------------------------------------------------------------------------------------------
int adas_AC3_Enum_Devices(void)
{
	return g_lpEAXAC3FnTable->EAXAC3QueryNumberOfDevices();
}

//------------------------------------------------------------------------------------------------
// get AC3 device caps
//------------------------------------------------------------------------------------------------
void adas_AC3_Get_Device_Caps(int Device, EAXAC3DEVICEINFO *p_Device_Info)
{
	HRESULT hr;

	
	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3QueryDeviceCaps(Device, p_Device_Info, sizeof(EAXAC3DEVICEINFO))))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return;
	}
}

//------------------------------------------------------------------------------------------------
// open AC3 device
//------------------------------------------------------------------------------------------------
int adas_AC3_Open_Device(int Device)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3OpenPlaybackDevice(Device)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// Close AC3 device
//------------------------------------------------------------------------------------------------
int adas_AC3_Close_Device(int Device)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3ClosePlaybackDevice(Device)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// Gets file info
//------------------------------------------------------------------------------------------------
int adas_AC3_Query_File(char *p_File_Name, AC3FILEINFO *p_File_Info)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3QueryFile(p_File_Name, p_File_Info, 
													   sizeof(AC3FILEINFO))))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// open AC3 stream
//------------------------------------------------------------------------------------------------
int adas_AC3_Open_Stream(int Device, char *p_File_Name, int AC3_Source,
			              AC3STREAM *p_AC3_Stream, LPAC3CALLBACK AC3_Call_Back)
{
	HRESULT hr;
	
	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3OpenStream(Device, p_AC3_Stream, AC3_Call_Back,
														p_File_Name, AC3FILE)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// close AC3 stream
//------------------------------------------------------------------------------------------------
int adas_AC3_Close_Stream(AC3STREAM AC3_Stream)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3CloseStream(AC3_Stream)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// play AC3 stream
//------------------------------------------------------------------------------------------------
int adas_AC3_Play_Stream(AC3STREAM AC3_Stream, BOOL Loop)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3PlayStream(AC3_Stream, Loop)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// setup AC3 stream:
// Fuction will perfort exactly same operations as AC3_Play_Stream method  - exept for actually
// starting to play data.
//------------------------------------------------------------------------------------------------
int adas_AC3_Setup_Stream(AC3STREAM AC3_Stream)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3PrePlayStream(AC3_Stream)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// stop AC3 stream
//------------------------------------------------------------------------------------------------
int adas_AC3_Stop_Stream(AC3STREAM AC3_Stream)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3StopStream(AC3_Stream)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// pause AC3 stream
//------------------------------------------------------------------------------------------------
int adas_AC3_Pause_Stream(AC3STREAM AC3_Stream)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3PauseStream(AC3_Stream)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// resume AC3 stream
//------------------------------------------------------------------------------------------------
int adas_AC3_Resume_Stream(AC3STREAM AC3_Stream)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3ResumeStream(AC3_Stream)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// set AC3 file position
//------------------------------------------------------------------------------------------------
int adas_AC3_Set_File_Position(AC3STREAM AC3_Stream, int Pos_Format, int iValue)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3SetFilePosition(AC3_Stream, (enum POSFORMAT)Pos_Format, iValue)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// get AC3 file position
//------------------------------------------------------------------------------------------------
int adas_AC3_Get_File_Position(AC3STREAM AC3_Stream, int Pos_Format, int *iValue)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3GetPosition(AC3_Stream, (enum POSFORMAT)Pos_Format, iValue)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}

//------------------------------------------------------------------------------------------------
// set AC3 playback mode
//------------------------------------------------------------------------------------------------
int adas_AC3_Playback_Mode(int Device, unsigned int Mode)
{
	HRESULT hr;

	if (FAILED(hr = g_lpEAXAC3FnTable->EAXAC3SetPlaybackMode(Device, Mode)))
	{
		char text[256];

		g_lpEAXAC3FnTable->EAXAC3GetErrorString(hr, text, 256);
		adas_Set_Last_Error(text);
		return 0;
	}

return 1;
}


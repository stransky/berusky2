#ifndef _TOOLS__
#define _TOOLS__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	// Get CPU speed
	unsigned __int64 tools_Get_CPU_Speed(void);

	double _ui64tod(unsigned __int64 ui64Value);

	float tools_Base_Priority(unsigned __int64 CPU_Speed);

	void tools_Parse_Command_Line(char *pCommnad, char *pLevel, char *pDemo, char *demo);

	char *_ui64towc(__int64 i64, char *cText);

	void MyMessageBox(HWND hWnd, char *ctagtitle, char *ctagtext, char *addtext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
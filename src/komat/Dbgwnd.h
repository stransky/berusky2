#ifdef __cplusplus
extern "C"
  {
#endif

#ifndef NOTDEBUGPRINT
#define dbgprintf DebugPrint
#else
#define dbgprintf
#endif

void DebugPrint(char *text, ... );


#ifdef __cplusplus
  }
#endif


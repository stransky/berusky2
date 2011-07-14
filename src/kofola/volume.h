#ifndef __VOLUME_
#define __VOLUME_

BOOL amdInitialize(HWND hwnd);
void amdUninitialize();
BOOL amdGetMasterVolumeControl();

BOOL amdGetMasterVolumeValue(float *dwVal);
BOOL amdSetMasterVolumeValue(float dwVal);
#endif

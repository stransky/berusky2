#include "3d_all.h"

//static HMIXER m_hMixer = NULL;
static int m_dwVolumeControlID = 0;
static long m_dwMinimum = 0;
static long m_dwMaximum = 0;

BOOL amdInitialize(HWND hwnd)
{
/*
	MIXERCAPS m_mxcaps;

	// get the number of mixer devices present in the system
	int m_nNumMixers = mixerGetNumDevs();

	ZeroMemory(&m_mxcaps, sizeof(MIXERCAPS));

	// open the first mixer
	// A "mapper" for audio mixer devices does not currently exist.
	if (m_nNumMixers != 0)
	{
		if(mixerOpen(&m_hMixer,0,0,0,MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
		{
			return FALSE;
		}

		if(mixerGetDevCaps((unsigned int) m_hMixer,&m_mxcaps, sizeof(MIXERCAPS)) != MMSYSERR_NOERROR)
		{
			return FALSE;
		}
	}

	kprintf(1, "Mixer: %s", m_mxcaps.szPname);
*/
  return TRUE;
}

void amdUninitialize()
{
/*
	if (m_hMixer != NULL)
	{
		mixerClose(m_hMixer);
		m_hMixer = NULL;
	}
*/
  return;
}

BOOL amdGetMasterVolumeControl()
{
/*
	MIXERLINE mxl;
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;

	if (m_hMixer == NULL)
	{
		return FALSE;
	}

	// get dwLineID
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	
	if (mixerGetLineInfo((HMIXEROBJ)(m_hMixer),&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE)!= MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	// get dwControlID
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (mixerGetLineControls((HMIXEROBJ)(m_hMixer), &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE)!= MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	// store dwControlID
	m_dwMinimum = mxc.Bounds.dwMinimum;
	m_dwMaximum = mxc.Bounds.dwMaximum;
	m_dwVolumeControlID = mxc.dwControlID;
*/
  return TRUE;
}

BOOL amdGetMasterVolumeValue(float *dwVal)
{
/*
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;

	if (m_hMixer == NULL)
	{
		return FALSE;
	}

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (mixerGetControlDetails((HMIXEROBJ)(m_hMixer),&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	*dwVal = mxcdVolume.dwValue / (float)(m_dwMaximum - m_dwMinimum);
*/
  return TRUE;
}

BOOL amdSetMasterVolumeValue(float dwVal)
{
/*
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;

	if (m_hMixer == NULL)
	{
		return FALSE;
	}

	mxcdVolume.dwValue = m_dwMinimum + (int)floor((m_dwMaximum - m_dwMinimum) * dwVal);
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (mixerSetControlDetails((HMIXEROBJ)(m_hMixer),&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
*/
  return TRUE;
}

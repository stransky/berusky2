//-----------------------------------------------------------------------------
// File: ddutil.cpp
//
// Desc: DirectDraw framewark classes. Feel free to use this class as a 
//       starting point for adding extra functionality.
//
//
// Copyright (c) 1995-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include "ddutil.h"
#include "dxutil.h"

extern WORD						NumberRedBits, 
							        NumberGreenBits, 
							        NumberBlueBits;
extern WORD						LowRedBit, 
							        LowGreenBit, 
							        LowBlueBit;

// Converts the supplied RGB values to a DWORD Pixel Formatted Colour
DWORD RGBToColour( DWORD Red, DWORD Green, DWORD Blue )
{
   // Supplied Values(0 - 255) are converted to Pixel Formatted Values
   // If 565 format and Red = 128, RedVal will become 16 since 128 / 256 == 16 / 32(5 bits)
   DWORD RedVal = ( Red * ( 1 << NumberRedBits )) / 256;
   DWORD GreenVal = ( Green * ( 1 << NumberGreenBits )) / 256;
   DWORD BlueVal = ( Blue * ( 1 << NumberBlueBits )) / 256;

   // Convert the Values to the appropriate Location in the returned colour value
   RedVal <<= LowRedBit;
   GreenVal <<= LowGreenBit;
   BlueVal <<= LowBlueBit;

   return RedVal | GreenVal | BlueVal;
}

//-----------------------------------------------------------------------------
// Name: CDisplay()
// Desc:
//-----------------------------------------------------------------------------
CDisplay::CDisplay()
{
    m_pDD                  = NULL;
    m_pddsFrontBuffer      = NULL;
    m_pddsBackBuffer       = NULL;
    m_pddsBackBufferLeft   = NULL;
	i_CursorX[0]		   = 0;
	i_CursorY[0]		   = 0;
	i_CursorX[1]		   = 0;
	i_CursorY[1]		   = 0;
	i_ActBuffer			   = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CDisplay()
// Desc:
//-----------------------------------------------------------------------------
CDisplay::~CDisplay()
{
    DestroyObjects();
}




//-----------------------------------------------------------------------------
// Name: DestroyObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDisplay::DestroyObjects()
{
    SAFE_RELEASE( m_pddsBackBufferLeft );
    SAFE_RELEASE( m_pddsBackBuffer );
    SAFE_RELEASE( m_pddsFrontBuffer );

    if( m_pDD )
        m_pDD->SetCooperativeLevel( m_hWnd, DDSCL_NORMAL );

    SAFE_RELEASE( m_pDD );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DestroyObjects()
// Desc:
//-----------------------------------------------------------------------------
VOID CDisplay::AddBufferCounter()
{
	i_ActBuffer++;

	if(i_ActBuffer > 1)
		i_ActBuffer = 0;
}

INT CDisplay::CheckFlipMode()
{
	COLORREF       rgbT = 1, rgbTb = 1;
	HDC            hdc;

    if(m_pddsBackBuffer->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 100, 100);					   // Save current pixel value
        SetPixel(hdc, 100, 100, RGB(255, 255, 255));       // Set our value
        m_pddsBackBuffer->ReleaseDC(hdc);
    }
	else
		return 1;

	Present();

    if(m_pddsBackBuffer->GetDC(&hdc) == DD_OK)
    {
        rgbTb = GetPixel(hdc, 100, 100);				   // Get current pixel value
        m_pddsBackBuffer->ReleaseDC(hdc);
    }

	if(!rgbTb)
	{
		Present();

		if(m_pddsBackBuffer->GetDC(&hdc) == DD_OK)
		{
			rgbTb = GetPixel(hdc, 100, 100);				   // Get current pixel value
			m_pddsBackBuffer->ReleaseDC(hdc);
		}

		if(!rgbTb)
			return 2;
		else
			return 1;
	}
	else
	{
		m_bWindowed = 1;
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: CreateFullScreenDisplay()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateFullScreenDisplay( HWND hWnd, DWORD dwWidth,
                                           DWORD dwHeight, DWORD dwBPP )
{
    HRESULT hr;

    // Cleanup anything from a previous call
    DestroyObjects();

    // DDraw stuff begins here
    if( FAILED( hr = DirectDrawCreateEx( NULL, (VOID**)&m_pDD,
                                         IID_IDirectDraw7, NULL ) ) )
        return E_FAIL;

    // Set cooperative level
    hr = m_pDD->SetCooperativeLevel( hWnd, DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN );
    if( FAILED(hr) )
        return E_FAIL;

    // Set the display mode
    if( FAILED( m_pDD->SetDisplayMode( dwWidth, dwHeight, dwBPP, 0, 0 ) ) )
        return E_FAIL;

    // Create primary surface (with backbuffer attached)
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize            = sizeof( ddsd );
    ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP |
                             DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;

    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsFrontBuffer,
                                           NULL ) ) )
        return E_FAIL;

    // Get a pointer to the back buffer
    DDSCAPS2 ddscaps;
    ZeroMemory( &ddscaps, sizeof( ddscaps ) );
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

    if( FAILED( hr = m_pddsFrontBuffer->GetAttachedSurface( &ddscaps,
                                                            &m_pddsBackBuffer ) ) )
        return E_FAIL;

    m_pddsBackBuffer->AddRef();

    m_hWnd      = hWnd;
    m_bWindowed = FALSE;
    UpdateBounds();

    return S_OK;
}
    



//-----------------------------------------------------------------------------
// Name: CreateWindowedDisplay()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateWindowedDisplay( HWND hWnd, DWORD dwWidth, DWORD dwHeight )
{
    HRESULT hr;

    // Cleanup anything from a previous call
    DestroyObjects();

    // DDraw stuff begins here
    if( FAILED( hr = DirectDrawCreateEx( NULL, (VOID**)&m_pDD,
                                         IID_IDirectDraw7, NULL ) ) )
        return E_FAIL;

    // Set cooperative level
    hr = m_pDD->SetCooperativeLevel( hWnd, DDSCL_NORMAL );
    if( FAILED(hr) )
        return E_FAIL;

    /*RECT  rcWork;
    RECT  rc;
    DWORD dwStyle;*/

    // If we are still a WS_POPUP window we should convert to a normal app
    // window so we look like a windows app.
    /*dwStyle  = GetWindowStyle( hWnd );
    dwStyle &= ~WS_POPUP;
	//dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
	dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME;
    SetWindowLong( hWnd, GWL_STYLE, dwStyle );

    // Aet window size
    SetRect( &rc, 0, 0, dwWidth, dwHeight );

    AdjustWindowRectEx( &rc, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL,
                        GetWindowExStyle(hWnd) );

    SetWindowPos( hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
                  SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

    SetWindowPos( hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                  SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );

    //  Make sure our window does not hang outside of the work area
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
    GetWindowRect( hWnd, &rc );
    if( rc.left < rcWork.left ) rc.left = rcWork.left;
    if( rc.top  < rcWork.top )  rc.top  = rcWork.top;
    SetWindowPos( hWnd, NULL, rc.left, rc.top, 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );*/

    LPDIRECTDRAWCLIPPER pcClipper;
    
    // Create the primary surface
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize         = sizeof( ddsd );
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if( FAILED( m_pDD->CreateSurface( &ddsd, &m_pddsFrontBuffer, NULL ) ) )
        return E_FAIL;

    // Create the backbuffer surface
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
    ddsd.dwWidth        = dwWidth;
    ddsd.dwHeight       = dwHeight;

    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsBackBuffer, NULL ) ) )
        return E_FAIL;

    if( FAILED( hr = m_pDD->CreateClipper( 0, &pcClipper, NULL ) ) )
        return E_FAIL;

    if( FAILED( hr = pcClipper->SetHWnd( 0, hWnd ) ) )
    {
        pcClipper->Release();
        return E_FAIL;
    }

    if( FAILED( hr = m_pddsFrontBuffer->SetClipper( pcClipper ) ) )
    {
        pcClipper->Release();
        return E_FAIL;
    }

    // Done with clipper
    pcClipper->Release();

    m_hWnd      = hWnd;
    m_bWindowed = TRUE;
    UpdateBounds();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurface( CSurface** ppSurface,
                                 DWORD dwWidth, DWORD dwHeight )
{
    if( NULL == m_pDD )
        return E_POINTER;
    if( NULL == ppSurface )
        return E_INVALIDARG;

    HRESULT        hr;
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize         = sizeof( ddsd );
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT; 
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth        = dwWidth;
    ddsd.dwHeight       = dwHeight;

    (*ppSurface) = new CSurface();

	if(!(*ppSurface))
		return E_FAIL;

    if( FAILED( hr = (*ppSurface)->Create( m_pDD, &ddsd ) ) )
    {
        delete (*ppSurface);
        return hr;
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: CDisplay::CreateSurfaceFromBitmap()
// Desc: Create a DirectDrawSurface from a bitmap resource or bitmap file.
//       Use MAKEINTRESOURCE() to pass a constant into strBMP.
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurfaceFromBitmap( CSurface** ppSurface,
                                           TCHAR* strBMP,                                            
                                           DWORD dwDesiredWidth, 
                                           DWORD dwDesiredHeight )
{
    HRESULT        hr;
    HBITMAP        hBMP = NULL;
    BITMAP         bmp;
    DDSURFACEDESC2 ddsd;

    if( m_pDD == NULL || strBMP == NULL || ppSurface == NULL ) 
        return E_INVALIDARG;

    *ppSurface = NULL;

    //  Try to load the bitmap as a resource, if that fails, try it as a file
    hBMP = (HBITMAP) LoadImage( GetModuleHandle(NULL), strBMP, 
                                IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, 
                                LR_CREATEDIBSECTION );
    if( hBMP == NULL )
    {
        hBMP = (HBITMAP) LoadImage( NULL, strBMP, 
                                    IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, 
                                    LR_LOADFROMFILE | LR_CREATEDIBSECTION );
        if( hBMP == NULL )
            return E_FAIL;
    }

    // Get size of the bitmap
    GetObject( hBMP, sizeof(bmp), &bmp );

    // Create a DirectDrawSurface for this bitmap
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth        = bmp.bmWidth;
    ddsd.dwHeight       = bmp.bmHeight;

    (*ppSurface) = new CSurface();
    if( FAILED( hr = (*ppSurface)->Create( m_pDD, &ddsd ) ) )
    {
        delete (*ppSurface);
        return hr;
    }

    // Draw the bitmap on this surface
    if( FAILED( hr = (*ppSurface)->DrawBitmap( hBMP, 0, 0, 0, 0 ) ) )
    {
        DeleteObject( hBMP );
        return hr;
    }

    DeleteObject( hBMP );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CDisplay::CreateSurfaceFromBitmap()
// Desc: Create a DirectDrawSurface from a bitmap resource or bitmap file.
//       Use MAKEINTRESOURCE() to pass a constant into strBMP.
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurfaceFromBitmapArchive( CSurface** ppSurface,
												  TCHAR* strBMP,                                            
                                                  APAK_HANDLE *pArchive)
{
	HDC					hdc;
	APAK_FILE_HANDLE	*fHandle;
	FILE				*file;
    HRESULT				hr;
    HBITMAP				hBMP = NULL;
	HBITMAP				hBMPOLD = NULL;
    DDSURFACEDESC2		ddsd;
	HDC					bitmapDC;
	char				*pBMP;
	BITMAPINFOHEADER	bmpInfoHeader;
	BITMAPFILEHEADER	bmpFileHeader;
	long				size;
	BITMAPINFO			bmpInfo;

    if( m_pDD == NULL || strBMP == NULL || ppSurface == NULL ) 
        return E_INVALIDARG;

	*ppSurface = NULL;

	pArchive->bError = 0;

	file = aopen(pArchive, strBMP, "rb");
	
	if(pArchive->bError)
		return E_FAIL;

	if(!file)
		return E_FAIL;

	fHandle = (APAK_FILE_HANDLE *)file;

	aread(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	aread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);

	size = bmpFileHeader.bfSize - bmpFileHeader.bfOffBits;
	pBMP = (char *) malloc(size);

	aread(pBMP, size, 1, file);

	aclose(file);

	hdc = GetDC(m_hWnd);

	bitmapDC = CreateCompatibleDC(hdc);

	if(!bitmapDC)
	{
		ReleaseDC(m_hWnd, hdc);
		return E_FAIL;
	}

	bmpInfo.bmiHeader = bmpInfoHeader;

	hBMP = CreateCompatibleBitmap(hdc, bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);

	ReleaseDC(m_hWnd, hdc);

	if(!hBMP)
	{
		DeleteDC(bitmapDC);
		return E_FAIL;
	}

	if(!(hBMPOLD = (HBITMAP) SelectObject(bitmapDC, hBMP)))
	{
		DeleteDC(bitmapDC);
		DeleteObject(hBMP);
		return E_FAIL;
	} 

	SetDIBits(bitmapDC, 
			  hBMP, 
			  0, bmpInfoHeader.biHeight,
			  (CONST VOID *)pBMP,
			  &bmpInfo,
			  DIB_RGB_COLORS);

	if(!(hBMP = (HBITMAP) SelectObject(bitmapDC, hBMPOLD)))
	{
		DeleteDC(bitmapDC);
		DeleteObject(hBMP);
		return E_FAIL;
	} 

	free((void *) pBMP);

    // Create a DirectDrawSurface for this bitmap
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth        = bmpInfoHeader.biWidth;
    ddsd.dwHeight       = bmpInfoHeader.biHeight;

    (*ppSurface) = new CSurface();
    if( FAILED( hr = (*ppSurface)->Create( m_pDD, &ddsd ) ) )
    {
        delete (*ppSurface);
        return hr;
    }

    // Draw the bitmap on this surface
    if( FAILED( hr = (*ppSurface)->DrawBitmap( hBMP, 0, 0, 0, 0 ) ) )
    {	
        DeleteObject( hBMP );
		DeleteDC(bitmapDC);
        return hr;
    }
	
    DeleteObject( hBMP );
	DeleteDC(bitmapDC);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CDisplay::CreateSurfaceFromText()
// Desc: Creates a DirectDrawSurface from a text string using hFont or the default 
//       GDI font if hFont is NULL.
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurfaceFromText( CSurface** ppSurface,
                                         HFONT hFont, TCHAR* strText, 
                                         COLORREF crBackground, COLORREF crForeground )
{
    HDC                  hDC  = NULL;
    LPDIRECTDRAWSURFACE7 pDDS = NULL;
    HRESULT              hr;
    DDSURFACEDESC2       ddsd;
    SIZE                 sizeText;

    if( m_pDD == NULL || strText == NULL || ppSurface == NULL )
        return E_INVALIDARG;

    *ppSurface = NULL;

    hDC = GetDC( NULL );

    if( hFont )
        SelectObject( hDC, hFont );

    GetTextExtentPoint32( hDC, strText, _tcslen(strText), &sizeText );
    ReleaseDC( NULL, hDC );

    // Create a DirectDrawSurface for this bitmap
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth        = sizeText.cx;
    ddsd.dwHeight       = sizeText.cy;

    (*ppSurface) = new CSurface();
    if( FAILED( hr = (*ppSurface)->Create( m_pDD, &ddsd ) ) )
    {
        delete (*ppSurface);
        return hr;
    }

    if( FAILED( hr = (*ppSurface)->DrawText( hFont, strText, 0, 0, 
                                             crBackground, crForeground ) ) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Present()
{
    HRESULT					hr;

    if( NULL == m_pddsFrontBuffer && NULL == m_pddsBackBuffer )
        return E_POINTER;

    while( 1 )
    {
        if( m_bWindowed )
            hr = m_pddsFrontBuffer->Blt( &m_rcWindow, m_pddsBackBuffer,
                                         NULL, DDBLT_WAIT, NULL );
        else
            hr = m_pddsFrontBuffer->Flip( NULL, 0 );

        if( hr == DDERR_SURFACELOST )
        {
            m_pddsFrontBuffer->Restore();
            m_pddsBackBuffer->Restore();
        }

        if( hr != DDERR_WASSTILLDRAWING )
            return hr;
    }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::ShowBitmap( HBITMAP hbm, LPDIRECTDRAWPALETTE pPalette )
{
    if( NULL == m_pddsFrontBuffer ||  NULL == m_pddsBackBuffer )
        return E_POINTER;

    // Set the palette before loading the bitmap
    if( pPalette )
        m_pddsFrontBuffer->SetPalette( pPalette );

    CSurface backBuffer;
    backBuffer.Create( m_pddsBackBuffer );

    if( FAILED( backBuffer.DrawBitmap( hbm, 0, 0, 0, 0 ) ) )
        return E_FAIL;

    return Present();
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::ColorKeyBlt( DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds,
                               RECT* prc)
{
    if( NULL == m_pddsBackBuffer )
        return E_POINTER;

	return m_pddsBackBuffer->BltFast( x, y, pdds, prc, DDBLTFAST_SRCCOLORKEY );
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::FillRect( RECT* prc, DWORD dwColor )
{
    if( NULL == m_pddsBackBuffer )
        return E_POINTER;

    // Fill rect
    DDBLTFX ddbltfx;
    ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
    ddbltfx.dwSize      = sizeof(ddbltfx);
    ddbltfx.dwFillColor = RGBToColour(GetRValue(dwColor), GetGValue(dwColor), GetBValue(dwColor));

    return m_pddsBackBuffer->Blt( prc, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Blt( DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc,
                       DWORD dwFlags )
{
     if( NULL == m_pddsBackBuffer )
        return E_POINTER;

	if( m_bWindowed )
		return  m_pddsBackBuffer->BltFast( x, y, pdds, prc, dwFlags );
	else
		return m_pddsBackBuffer->BltFast( x, y, pdds, prc, dwFlags );

	//return hr;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Blt( DWORD x, DWORD y, CSurface* pSurface, RECT* prc )
{
    if( NULL == pSurface )
        return E_INVALIDARG;

    if( pSurface->IsColorKeyed() )
        return Blt( x, y, pSurface->GetDDrawSurface(), prc, DDBLTFAST_SRCCOLORKEY);
    else
        return Blt( x, y, pSurface->GetDDrawSurface(), prc, 0L );
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::StretchBlt( RECT* dprc, CSurface* pSurface, RECT* sprc )
{
	DDBLTFX ddbltfx;

    if( NULL == pSurface )
        return E_INVALIDARG;

	ZeroMemory(&ddbltfx, sizeof(DDBLTFX));

	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;
 
	HRESULT hr = m_pddsBackBuffer->Blt(dprc,
									   pSurface->GetDDrawSurface(),
                                       sprc,
                                       DDBLT_WAIT,
                                       NULL);

/*	if(hr == DDERR_SURFACELOST)
	{
		pSurface->GetDDrawSurface()->Restore();
		m_pddsBackBuffer->Restore();
		return m_pddsBackBuffer->Blt(dprc,
									 pSurface->GetDDrawSurface(),
                                     sprc,
                                     DDBLT_WAIT,
                                     NULL);
	}*/

	return hr;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Clear( DWORD dwColor )
{
    if( NULL == m_pddsBackBuffer )
        return E_POINTER;

    // Erase the background
    DDBLTFX ddbltfx;
    ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
    ddbltfx.dwSize      = sizeof(ddbltfx);
    ddbltfx.dwFillColor = RGBToColour(GetRValue(dwColor), GetGValue(dwColor), GetBValue(dwColor));

    return m_pddsBackBuffer->Blt( NULL, NULL, NULL, DDBLT_COLORFILL , &ddbltfx );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::SetPalette( LPDIRECTDRAWPALETTE pPalette )
{
    if( NULL == m_pddsFrontBuffer )
        return E_POINTER;

    return m_pddsFrontBuffer->SetPalette( pPalette );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreatePaletteFromBitmap( LPDIRECTDRAWPALETTE* ppPalette,
                                           const TCHAR* strBMP )
{
    HRSRC             hResource      = NULL;
    RGBQUAD*          pRGB           = NULL;
    BITMAPINFOHEADER* pbi = NULL;
    PALETTEENTRY      aPalette[256];
    HANDLE            hFile = NULL;
    DWORD             iColor;
    DWORD             dwColors;
    BITMAPFILEHEADER  bf;
    BITMAPINFOHEADER  bi;
    DWORD             dwBytesRead;

    if( m_pDD == NULL || strBMP == NULL || ppPalette == NULL )
        return E_INVALIDARG;

    *ppPalette = NULL;

    //  Try to load the bitmap as a resource, if that fails, try it as a file
    hResource = FindResource( NULL, strBMP, RT_BITMAP );
    if( hResource )
    {
        pbi = (LPBITMAPINFOHEADER) LockResource( LoadResource( NULL, hResource ) );       
        if( NULL == pbi )
            return E_FAIL;

        pRGB = (RGBQUAD*) ( (BYTE*) pbi + pbi->biSize );

        // Figure out how many colors there are
        if( pbi == NULL || pbi->biSize < sizeof(BITMAPINFOHEADER) )
            dwColors = 0;
        else if( pbi->biBitCount > 8 )
            dwColors = 0;
        else if( pbi->biClrUsed == 0 )
            dwColors = 1 << pbi->biBitCount;
        else
            dwColors = pbi->biClrUsed;

        //  A DIB color table has its colors stored BGR not RGB
        //  so flip them around.
        for( iColor = 0; iColor < dwColors; iColor++ )
        {
            aPalette[iColor].peRed   = pRGB[iColor].rgbRed;
            aPalette[iColor].peGreen = pRGB[iColor].rgbGreen;
            aPalette[iColor].peBlue  = pRGB[iColor].rgbBlue;
            aPalette[iColor].peFlags = 0;
        }

        return m_pDD->CreatePalette( DDPCAPS_8BIT, aPalette, ppPalette, NULL );
    }

    // Attempt to load bitmap as a file
    hFile = CreateFile( strBMP, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    if( NULL == hFile )
        return E_FAIL;

    // Read the BITMAPFILEHEADER
    ReadFile( hFile, &bf, sizeof(bf), &dwBytesRead, NULL );
    if( dwBytesRead != sizeof(bf) )
    {
        CloseHandle( hFile );
        return E_FAIL;
    }

    // Read the BITMAPINFOHEADER
    ReadFile( hFile, &bi, sizeof(bi), &dwBytesRead, NULL );
    if( dwBytesRead != sizeof(bi) )
    {
        CloseHandle( hFile );
        return E_FAIL;
    }

    // Read the PALETTEENTRY 
    ReadFile( hFile, aPalette, sizeof(aPalette), &dwBytesRead, NULL );
    if( dwBytesRead != sizeof(aPalette) )
    {
        CloseHandle( hFile );
        return E_FAIL;
    }

    CloseHandle( hFile );

    // Figure out how many colors there are
    if( bi.biSize != sizeof(BITMAPINFOHEADER) )
        dwColors = 0;
    else if (bi.biBitCount > 8)
        dwColors = 0;
    else if (bi.biClrUsed == 0)
        dwColors = 1 << bi.biBitCount;
    else
        dwColors = bi.biClrUsed;

    //  A DIB color table has its colors stored BGR not RGB
    //  so flip them around since DirectDraw uses RGB
    for( iColor = 0; iColor < dwColors; iColor++ )
    {
        BYTE r = aPalette[iColor].peRed;
        aPalette[iColor].peRed  = aPalette[iColor].peBlue;
        aPalette[iColor].peBlue = r;
    }

    return m_pDD->CreatePalette( DDPCAPS_8BIT, aPalette, ppPalette, NULL );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::UpdateBounds()
{
    if( m_bWindowed )
    {
        GetClientRect( m_hWnd, &m_rcWindow );
        ClientToScreen( m_hWnd, (POINT*)&m_rcWindow );
        ClientToScreen( m_hWnd, (POINT*)&m_rcWindow+1 );
    }
    else
    {
        SetRect( &m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                 GetSystemMetrics(SM_CYSCREEN) );
    }

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: CDisplay::InitClipper
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::InitClipper()
{
    LPDIRECTDRAWCLIPPER pClipper;
    HRESULT hr;

    // Create a clipper when using GDI to draw on the primary surface 
    if( FAILED( hr = m_pDD->CreateClipper( 0, &pClipper, NULL ) ) )
        return hr;

    pClipper->SetHWnd( 0, m_hWnd );

    if( FAILED( hr = m_pddsFrontBuffer->SetClipper( pClipper ) ) )
        return hr;

    // We can release the clipper now since g_pDDSPrimary 
    // now maintains a ref count on the clipper
    SAFE_RELEASE( pClipper );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::SetColorKey( COLORREF dwColorKey )
{
    if( NULL == m_pddsBackBuffer )
        return E_POINTER;

    DDCOLORKEY ddck;
    ddck.dwColorSpaceLowValue  = ConvertGDIColor( dwColorKey );
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;

    return m_pddsBackBuffer->SetColorKey( DDCKEY_SRCBLT, &ddck );
}


//-----------------------------------------------------------------------------
// Name: CSurface::ConvertGDIColor()
// Desc: Converts a GDI color (0x00bbggrr) into the equivalent color on a 
//       DirectDrawSurface using its pixel format.  
//-----------------------------------------------------------------------------
DWORD CDisplay::ConvertGDIColor( COLORREF dwGDIColor )
{
    if( m_pddsBackBuffer == NULL )
	    return 0x00000000;

    COLORREF       rgbT;
    HDC            hdc;
    DWORD          dw = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT        hr;

    //  Use GDI SetPixel to color match for us
    if( dwGDIColor != CLR_INVALID && m_pddsBackBuffer->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
        SetPixel(hdc, 0, 0, dwGDIColor);       // Set our value
        m_pddsBackBuffer->ReleaseDC(hdc);
    }

    // Now lock the surface so we can read back the converted color
    ddsd.dwSize = sizeof(ddsd);
    hr = m_pddsBackBuffer->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
    if( hr == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface; 
        if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 ) // Mask it to bpp
            dw &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;  
        m_pddsBackBuffer->Unlock(NULL);
    }

    //  Now put the color that was there back.
    if( dwGDIColor != CLR_INVALID && m_pddsBackBuffer->GetDC(&hdc) == DD_OK )
    {
        SetPixel( hdc, 0, 0, rgbT );
        m_pddsBackBuffer->ReleaseDC(hdc);
    }
    
    return dw;    
}

HRESULT CDisplay::Save(FILE *file, int x, int y, int idx)
{
	HDC					hdc;
	HDC					hdcMem;
	BITMAPINFO			*pbmiRGB = (BITMAPINFO *) NULL; 
	HBITMAP				hbmRGB  = (HBITMAP) NULL;
	LPBYTE				pjBitsRGB;
	BITMAPFILEHEADER	bmpHeader;
	LPDIRECTDRAWSURFACE7	m_pdds;

	if(idx)
		m_pdds = m_pddsFrontBuffer;
	else
		m_pdds = m_pddsBackBuffer;

    if( NULL == m_pdds )
        return E_POINTER;

    if(m_pdds->GetDC(&hdc) == DD_OK)
	{
		hdcMem  = CreateCompatibleDC(hdc);
		
		pbmiRGB = (BITMAPINFO *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT, sizeof(BITMAPINFO) );
		
		if(!pbmiRGB)
			return E_FAIL;

		pbmiRGB->bmiHeader.biSize	       = sizeof(BITMAPINFOHEADER);     
		pbmiRGB->bmiHeader.biWidth		   = x;     
		pbmiRGB->bmiHeader.biHeight        = y;     
		pbmiRGB->bmiHeader.biPlanes        = 1;     
		pbmiRGB->bmiHeader.biBitCount      = 32;     
		pbmiRGB->bmiHeader.biCompression   = BI_RGB;     
		pbmiRGB->bmiHeader.biSizeImage     = pbmiRGB->bmiHeader.biWidth
											 * abs(pbmiRGB->bmiHeader.biHeight) * 4; 

		hbmRGB = CreateDIBSection(hdcMem, pbmiRGB, DIB_RGB_COLORS,
								  (PVOID *) &pjBitsRGB, NULL, 0);

		if(!hbmRGB)
			return E_FAIL;

		if(!SelectObject(hdcMem, hbmRGB))
			return E_FAIL;

		if (!BitBlt(hdcMem, 0, 0, x, y, hdc, 0, 0, SRCCOPY)) 
			return E_FAIL;

		pjBitsRGB = (LPBYTE) GlobalAlloc(GMEM_FIXED, sizeof(BYTE) * pbmiRGB->bmiHeader.biSizeImage);
		//pjBitsRGB = (LPBYTE) malloc(sizeof(BYTE) * pbmiRGB->bmiHeader.biSizeImage);

		if(!pjBitsRGB) 
			return E_FAIL;

		if (!GetDIBits(hdcMem, hbmRGB, 0, (UINT)pbmiRGB->bmiHeader.biHeight, pjBitsRGB, 
					   (LPBITMAPINFO)pbmiRGB, DIB_RGB_COLORS))
			return E_FAIL;

		bmpHeader.bfType = 'MB';
		bmpHeader.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + pbmiRGB->bmiHeader.biSizeImage;
		bmpHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
		bmpHeader.bfReserved1 = 0;
		bmpHeader.bfReserved2 = 0;

		fwrite(&bmpHeader,sizeof(BITMAPFILEHEADER),1,file);
		fwrite(&pbmiRGB->bmiHeader,sizeof(BITMAPINFOHEADER),1,file);
		fwrite(pjBitsRGB,sizeof(BYTE),pbmiRGB->bmiHeader.biSizeImage,file);

		//free((void *) pjBitsRGB);

		m_pdds->ReleaseDC(hdc);

		DeleteDC(hdcMem);          
		DeleteObject(hbmRGB);
		LocalFree(pbmiRGB);
		GlobalFree((HGLOBAL)pjBitsRGB);
	}
	else
		return E_FAIL;

	return DD_OK;
}
//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSurface::CSurface()
{
    m_pdds = NULL;
    m_bColorKeyed = FALSE;
	dw_ColorKey = 0;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSurface::~CSurface()
{
    SAFE_RELEASE( m_pdds );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Create( LPDIRECTDRAWSURFACE7 pdds )
{
    m_pdds = pdds;

    if( m_pdds )
    {
        m_pdds->AddRef();

        // Get the DDSURFACEDESC structure for this surface
        m_ddsd.dwSize = sizeof(m_ddsd);
        m_pdds->GetSurfaceDesc( &m_ddsd );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Create( LPDIRECTDRAW7 pDD, DDSURFACEDESC2* pddsd )
{
    HRESULT hr;

    // Create the DDraw surface
    if( FAILED( hr = pDD->CreateSurface( pddsd, &m_pdds, NULL ) ) )
        return hr;

    // Prepare the DDSURFACEDESC structure
    m_ddsd.dwSize = sizeof(m_ddsd);

    // Get the DDSURFACEDESC structure for this surface
    m_pdds->GetSurfaceDesc( &m_ddsd );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Destroy()
{
    SAFE_RELEASE( m_pdds );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSurface::DrawBitmap()
// Desc: Draws a bitmap over an entire DirectDrawSurface, stretching the 
//       bitmap if nessasary
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawBitmap( HBITMAP hBMP, 
                              DWORD dwBMPOriginX, DWORD dwBMPOriginY, 
                              DWORD dwBMPWidth, DWORD dwBMPHeight )
{
    HDC            hDCImage;
    HDC            hDC;
    BITMAP         bmp;
    DDSURFACEDESC2 ddsd;
    HRESULT        hr;

    if( hBMP == NULL || m_pdds == NULL )
        return E_INVALIDARG;

    // Make sure this surface is restored.
    if( FAILED( hr = m_pdds->Restore() ) )
        return hr;

    // Get the surface.description
    ddsd.dwSize  = sizeof(ddsd);
    m_pdds->GetSurfaceDesc( &ddsd );

    if( ddsd.ddpfPixelFormat.dwFlags == DDPF_FOURCC )
        return E_NOTIMPL;

    // Select bitmap into a memoryDC so we can use it.
    hDCImage = CreateCompatibleDC( NULL );
    if( NULL == hDCImage )
        return E_FAIL;

    SelectObject( hDCImage, hBMP );

    // Get size of the bitmap
    GetObject( hBMP, sizeof(bmp), &bmp );

    // Use the passed size, unless zero
    dwBMPWidth  = ( dwBMPWidth  == 0 ) ? bmp.bmWidth  : dwBMPWidth;     
    dwBMPHeight = ( dwBMPHeight == 0 ) ? bmp.bmHeight : dwBMPHeight;

    // Stretch the bitmap to cover this surface
    if( FAILED( hr = m_pdds->GetDC( &hDC ) ) )
        return hr;

    StretchBlt( hDC, 0, 0, 
                ddsd.dwWidth, ddsd.dwHeight, 
                hDCImage, dwBMPOriginX, dwBMPOriginY,
                dwBMPWidth, dwBMPHeight, SRCCOPY );


    if( FAILED( hr = m_pdds->ReleaseDC( hDC ) ) )
        return hr;

    DeleteDC( hDCImage );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSurface::DrawText()
// Desc: Draws a text string on a DirectDraw surface using hFont or the default
//       GDI font if hFont is NULL.  
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawText( HFONT hFont, TCHAR* strText, 
                            DWORD dwOriginX, DWORD dwOriginY,
                            COLORREF crBackground, COLORREF crForeground )
{
    HDC     hDC = NULL;
    HRESULT hr;

    if( m_pdds == NULL || strText == NULL )
        return E_INVALIDARG;

    // Make sure this surface is restored.
    if( FAILED( hr = m_pdds->Restore() ) )
        return hr;

    if( FAILED( hr = m_pdds->GetDC( &hDC ) ) )
        return hr;

    // Set the background and foreground color
    SetBkColor( hDC, crBackground );
    SetTextColor( hDC, crForeground );

    if( hFont )
        SelectObject( hDC, hFont );

    // Use GDI to draw the text on the surface
    TextOut( hDC, dwOriginX, dwOriginY, strText, _tcslen(strText) );

    if( FAILED( hr = m_pdds->ReleaseDC( hDC ) ) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSurface::ReDrawBitmapOnSurface()
// Desc: Load a bitmap from a file or resource into a DirectDraw surface.
//       normaly used to re-load a surface after a restore.
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawBitmap( TCHAR* strBMP, 
                              DWORD dwDesiredWidth, DWORD dwDesiredHeight  )
{
    HBITMAP hBMP;
    HRESULT hr;

    if( m_pdds == NULL || strBMP == NULL )
        return E_INVALIDARG;

    //  Try to load the bitmap as a resource, if that fails, try it as a file
    hBMP = (HBITMAP) LoadImage( GetModuleHandle(NULL), strBMP, 
                                IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, 
                                LR_CREATEDIBSECTION );
    if( hBMP == NULL )
    {
        hBMP = (HBITMAP) LoadImage( NULL, strBMP, IMAGE_BITMAP, 
                                    dwDesiredWidth, dwDesiredHeight, 
                                    LR_LOADFROMFILE | LR_CREATEDIBSECTION );
        if( hBMP == NULL )
            return E_FAIL;
    }

    // Draw the bitmap on this surface
    if( FAILED( hr = DrawBitmap( hBMP, 0, 0, 0, 0 ) ) )
    {
        DeleteObject( hBMP );
        return hr;
    }

    DeleteObject( hBMP );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::SetColorKey( COLORREF dwColorKey )
{
    if( NULL == m_pdds )
        return E_POINTER;

	DDCOLORKEY ddck;

	if(m_bColorKeyed)
	{
		ddck.dwColorSpaceLowValue  = dw_ColorKey;
		ddck.dwColorSpaceHighValue = dw_ColorKey;
	}
	else
	{
		m_bColorKeyed = TRUE;

    
		//ddck.dwColorSpaceLowValue  = ConvertGDIColor( dwColorKey );
		ddck.dwColorSpaceLowValue = RGBToColour(GetRValue(dwColorKey), GetGValue(dwColorKey), GetBValue(dwColorKey));
		ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;

		dw_ColorKey = ddck.dwColorSpaceLowValue;
	}

    return m_pdds->SetColorKey( DDCKEY_SRCBLT, &ddck );
}


//-----------------------------------------------------------------------------
// Name: CSurface::ConvertGDIColor()
// Desc: Converts a GDI color (0x00bbggrr) into the equivalent color on a 
//       DirectDrawSurface using its pixel format.  
//-----------------------------------------------------------------------------
DWORD CSurface::ConvertGDIColor( COLORREF dwGDIColor )
{
    if( m_pdds == NULL )
	    return 0x00000000;

    COLORREF       rgbT;
    HDC            hdc;
    DWORD          dw = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT        hr;

    //  Use GDI SetPixel to color match for us
    if( dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
        SetPixel(hdc, 0, 0, dwGDIColor);       // Set our value
        m_pdds->ReleaseDC(hdc);
    }

    // Now lock the surface so we can read back the converted color
    ddsd.dwSize = sizeof(ddsd);
    hr = m_pdds->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
    if( hr == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface; 
        if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 ) // Mask it to bpp
            dw &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;  
        m_pdds->Unlock(NULL);
    }

    //  Now put the color that was there back.
    if( dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK )
    {
        SetPixel( hdc, 0, 0, rgbT );
        m_pdds->ReleaseDC(hdc);
    }
    
    return dw;    
}

//-----------------------------------------------------------------------------
// Name: CSurface::GetBitMaskInfo()
// Desc: Returns the number of bits and the shift in the bit mask
//-----------------------------------------------------------------------------
HRESULT CSurface::GetBitMaskInfo( DWORD dwBitMask, DWORD* pdwShift, DWORD* pdwBits )
{
    DWORD dwShift = 0;
    DWORD dwBits  = 0; 

    if( pdwShift == NULL || pdwBits == NULL )
        return E_INVALIDARG;

    if( dwBitMask )
    {
        while( (dwBitMask & 1) == 0 )
        {
            dwShift++;
            dwBitMask >>= 1;
        }
    }

    while( (dwBitMask & 1) != 0 )
    {
        dwBits++;
        dwBitMask >>= 1;
    }

    *pdwShift = dwShift;
    *pdwBits  = dwBits;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Clear( DWORD dwColor )
{
    if( NULL == m_pdds )
        return E_POINTER;

    // Erase the background
    DDBLTFX ddbltfx;
    ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
    ddbltfx.dwSize      = sizeof(ddbltfx);
    ddbltfx.dwFillColor = RGBToColour(GetRValue(dwColor), GetGValue(dwColor), GetBValue(dwColor));

    return m_pdds->Blt( NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Blt( DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc,
                       DWORD dwFlags )
{
/*	HRESULT hr;
	DDBLTFX bltfx;
	RECT r;*/

    if( NULL == m_pdds )
        return E_POINTER;

/*	r.left = x;
	r.top = y;
	r.right = x + (prc->right - prc->left);
	r.bottom = y + (prc->bottom - prc->top);

	ZeroMemory(&bltfx, sizeof(DDBLTFX));
	bltfx.dwSize = sizeof(DDBLTFX);

	hr = m_pdds->Blt( &r, pdds, prc, dwFlags, &bltfx);

	if(hr == DDERR_SURFACELOST)
	{
		m_pdds->Restore();
		pdds->Restore();
		return m_pdds->Blt( &r, pdds, prc, dwFlags, &bltfx);
	}

	return hr;*/

	return m_pdds->BltFast( x, y, pdds, prc, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Blt( DWORD x, DWORD y, CSurface* pSurface, RECT* prc )
{
    if( NULL == pSurface )
        return E_INVALIDARG;

 /*   if( pSurface->IsColorKeyed() )
        return Blt( x, y, pSurface->GetDDrawSurface(), prc, DDBLTFAST_SRCCOLORKEY );
    else*/
        return Blt( x, y, pSurface->GetDDrawSurface(), prc, 0L );
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::ColorKeyBlt( DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds,
                               RECT* prc )
{
/*	HRESULT hr;
	DDBLTFX bltfx;
	RECT r;*/

    if( NULL == m_pdds )
        return E_POINTER;

    return m_pdds->BltFast( x, y, pdds, prc, DDBLTFAST_SRCCOLORKEY);

/*	r.left = x;
	r.top = y;
	r.right = x + (prc->right - prc->left);
	r.bottom = y + (prc->bottom - prc->top);

	ZeroMemory(&bltfx, sizeof(DDBLTFX));
	bltfx.dwSize = sizeof(DDBLTFX);

	hr = m_pdds->Blt( &r, pdds, prc, DDBLT_KEYSRC, &bltfx);

	if(hr == DDERR_SURFACELOST)
	{
		m_pdds->Restore();
		pdds->Restore();
		return m_pdds->Blt( &r, pdds, prc, DDBLT_KEYSRC, &bltfx);
	}

	return hr;*/
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::FillRect( RECT* prc, DWORD dwColor )
{
    if( NULL == m_pdds )
        return E_POINTER;

    // Fill rect
    DDBLTFX ddbltfx;
    ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
    ddbltfx.dwSize      = sizeof(ddbltfx);
    ddbltfx.dwFillColor = RGBToColour(GetRValue(dwColor), GetGValue(dwColor), GetBValue(dwColor));

    return m_pdds->Blt( prc, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
}

HRESULT CSurface::Save(FILE *file, int x, int y)
{
	HDC					hdc;
	HDC					hdcMem;
	BITMAPINFO			*pbmiRGB = (BITMAPINFO *) NULL; 
	HBITMAP				hbmRGB  = (HBITMAP) NULL;
	LPBYTE				pjBitsRGB;
	BITMAPFILEHEADER	bmpHeader;

    if( NULL == m_pdds )
        return E_POINTER;

    if(m_pdds->GetDC(&hdc) == DD_OK)
	{
		hdcMem  = CreateCompatibleDC(hdc);
		
		pbmiRGB = (BITMAPINFO *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT, sizeof(BITMAPINFO) );
		
		if(!pbmiRGB)
			return E_FAIL;

		pbmiRGB->bmiHeader.biSize	       = sizeof(BITMAPINFOHEADER);     
		pbmiRGB->bmiHeader.biWidth		   = x;     
		pbmiRGB->bmiHeader.biHeight        = y;     
		pbmiRGB->bmiHeader.biPlanes        = 1;     
		pbmiRGB->bmiHeader.biBitCount      = 32;     
		pbmiRGB->bmiHeader.biCompression   = BI_RGB;     
		pbmiRGB->bmiHeader.biSizeImage     = pbmiRGB->bmiHeader.biWidth
											 * abs(pbmiRGB->bmiHeader.biHeight) * 4; 

		hbmRGB = CreateDIBSection(hdcMem, pbmiRGB, DIB_RGB_COLORS,
								  (PVOID *) &pjBitsRGB, NULL, 0);

		if(!hbmRGB)
			return E_FAIL;

		if(!SelectObject(hdcMem, hbmRGB))
			return E_FAIL;

		if (!BitBlt(hdcMem, 0, 0, x, y, hdc, 0, 0, SRCCOPY)) 
			return E_FAIL;

		pjBitsRGB = (LPBYTE) GlobalAlloc(GMEM_FIXED, sizeof(BYTE) * pbmiRGB->bmiHeader.biSizeImage);
		//pjBitsRGB = (LPBYTE) malloc(sizeof(BYTE) * pbmiRGB->bmiHeader.biSizeImage);

		if(!pjBitsRGB) 
			return E_FAIL;

		if (!GetDIBits(hdcMem, hbmRGB, 0, (UINT)pbmiRGB->bmiHeader.biHeight, pjBitsRGB, 
					   (LPBITMAPINFO)pbmiRGB, DIB_RGB_COLORS))
			return E_FAIL;

		bmpHeader.bfType = 'MB';
		bmpHeader.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + pbmiRGB->bmiHeader.biSizeImage;
		bmpHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
		bmpHeader.bfReserved1 = 0;
		bmpHeader.bfReserved2 = 0;

		fwrite(&bmpHeader,sizeof(BITMAPFILEHEADER),1,file);
		fwrite(&pbmiRGB->bmiHeader,sizeof(BITMAPINFOHEADER),1,file);
		fwrite(pjBitsRGB,sizeof(BYTE),pbmiRGB->bmiHeader.biSizeImage,file);

		//free((void *) pjBitsRGB);

		m_pdds->ReleaseDC(hdc);

		DeleteDC(hdcMem);          
		DeleteObject(hbmRGB);
		LocalFree(pbmiRGB);
		GlobalFree((HGLOBAL)pjBitsRGB);
	}
	else
		return E_FAIL;

	return DD_OK;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::MyStretchBlt( RECT* dprc, CSurface* pSurface, RECT* sprc )
{
	DDBLTFX ddbltfx;

    if( NULL == pSurface )
        return E_INVALIDARG;

	if( NULL == m_pdds )
        return E_POINTER;

	ZeroMemory(&ddbltfx, sizeof(DDBLTFX));

	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;
 
	HRESULT hr = m_pdds->Blt(dprc,
							 pSurface->GetDDrawSurface(),
                             sprc,
                             DDBLT_WAIT,
                             NULL);
	return hr;
}

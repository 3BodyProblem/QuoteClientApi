// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DDC43FEA_F7E4_4DF4_865E_154B9783D02A__INCLUDED_)
#define AFX_STDAFX_H__DDC43FEA_F7E4_4DF4_865E_154B9783D02A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _MSC_VER

		typedef unsigned __int64	uint64_t;
		typedef __int64				int64_t;
		typedef unsigned int		uint32_t;
		typedef int					int32_t;
		typedef unsigned short		uint16_t;
		typedef short				int16_t;
		typedef unsigned char		uint8_t;
		typedef char				int8_t;

#else
	#include <stdint.h>
#endif


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>



// TODO: reference additional headers your program requires here
#include <winsock2.h>



#include "QuoteCltDef.h"
#include "QuoteClientApi.h"


#include <stdio.h>




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DDC43FEA_F7E4_4DF4_865E_154B9783D02A__INCLUDED_)

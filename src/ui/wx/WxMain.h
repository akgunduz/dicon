/////////////////////////////////////////////////////////////////////////////
// Name:        WxMain.h
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Thu 03 May 2018 12:54:09 +03
// RCS-ID:      
// Copyright:   Licensed with AGPL v3.0
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXMAIN_H_
#define _WXMAIN_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "Wx.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * WxApp class declaration
 */

class WxMain: public wxApp
{    
    DECLARE_CLASS( WxMain )
    DECLARE_EVENT_TABLE()

    void* controller;

public:
    /// Constructor
    WxMain();
    WxMain(void*);

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin WxMain event handler declarations

////@end WxMain event handler declarations

////@begin WxMain member function declarations

////@end WxMain member function declarations

////@begin WxMain member variables
////@end WxMain member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(WxMain)
////@end declare app

#endif
    // _WXMAIN_H_

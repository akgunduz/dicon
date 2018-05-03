/////////////////////////////////////////////////////////////////////////////
// Name:        WxApp.h
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Thu 03 May 2018 12:54:09 +03
// RCS-ID:      
// Copyright:   Licensed with AGPL v3.0
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXAPP_H_
#define _WXAPP_H_


/*!
 * Includes
 */

////@begin includes
#include <Application.h>
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

class WxApp: public wxApp, public Application
{    
    DECLARE_CLASS( WxApp )
    DECLARE_EVENT_TABLE()

    bool initStart;

public:
    /// Constructor
    WxApp();
    WxApp(int, int);

    void Init(bool);

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin WxApp event handler declarations
////@end WxApp event handler declarations

////@begin WxApp member function declarations
////@end WxApp member function declarations

////@begin WxApp member variables
////@end WxApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(WxApp)
////@end declare app

#endif
    // _WXAPP_H_

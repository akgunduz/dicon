/////////////////////////////////////////////////////////////////////////////
// Name:        UserInterfaceApp.h
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Sat 28 Apr 19:13:19 2018
// RCS-ID:      
// Copyright:   Licensed with AGPL v3.0
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _USERINTERFACEAPP_H_
#define _USERINTERFACEAPP_H_


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
 * UserInterfaceApp class declaration
 */

class UserInterfaceApp: public wxApp
{    
    DECLARE_CLASS( UserInterfaceApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    UserInterfaceApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin UserInterfaceApp event handler declarations

////@end UserInterfaceApp event handler declarations

////@begin UserInterfaceApp member function declarations

////@end UserInterfaceApp member function declarations

////@begin UserInterfaceApp member variables
////@end UserInterfaceApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(UserInterfaceApp)
////@end declare app

#endif
    // _USERINTERFACEAPP_H_

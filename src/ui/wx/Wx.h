/////////////////////////////////////////////////////////////////////////////
// Name:        Wx.h
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Thu 03 May 2018 12:55:15 +03
// RCS-ID:      
// Copyright:   Licensed with AGPL v3.0
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_H_
#define _WX_H_


/*!
 * Includes
 */

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/app.h>
#include <wx/event.h>

////@begin includes
#include "wx/frame.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/notebook.h"
#include "wx/listctrl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxListCtrl;
////@end forward declarations

class Wx;
typedef void (Wx::*fUIUpdater)(wxCommandEvent &event);
/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_USERINTERFACE 10000
#define ID_NOTEBOOK 10001
#define ID_PANEL_DISTRIBUTOR 10002
#define ID_DIST_POLL 10011
#define ID_DIST_COLL_LIST 10012
#define ID_DIST_NODE_LIST 10009
#define ID_PANEL_COLLECTOR 10014
#define ID_COLL_LOAD 10005
#define ID_COLL_PROCESS 10020
#define ID_COLL_FILE_LIST 10032
#define ID_COLL_PROCESS_LIST 10003
#define ID_PANEL_NODE 10015
#define ID_NODE_STATE 10026
#define ID_NODE_FILE_LIST 10029
#define SYMBOL_WX_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_WX_TITLE _("Dicon")
#define SYMBOL_WX_IDNAME ID_USERINTERFACE
#define SYMBOL_WX_SIZE wxSize(600, 570)
#define SYMBOL_WX_POSITION wxDefaultPosition
////@end control identifiers

#define MAX_UI_CB 100

/*!
 * Wx class declaration
 */

class Wx: public wxFrame
{    
    DECLARE_CLASS( Wx )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    Wx(void *);
    Wx(void *, wxWindow* parent, wxWindowID id = SYMBOL_WX_IDNAME, const wxString& caption = SYMBOL_WX_TITLE, const wxPoint& pos = SYMBOL_WX_POSITION, const wxSize& size = SYMBOL_WX_SIZE, long style = SYMBOL_WX_STYLE );

    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_WX_IDNAME, const wxString& caption = SYMBOL_WX_TITLE, const wxPoint& pos = SYMBOL_WX_POSITION, const wxSize& size = SYMBOL_WX_SIZE, long style = SYMBOL_WX_STYLE );

    /// Destructor
    ~Wx();

    /// Initialises member variables
    void Init(void*);

    /// Creates the controls and sizers
    void CreateControls();

    void *componentController;

    wxEventTypeTag<wxCommandEvent> *ui_event;

    fUIUpdater uiUpdater[MAX_UI_CB];

    //Generic

    static void updateUICallback(void*, int, void*);
    void updateUIEvent(int, void*);
    void updateUI(wxCommandEvent &event);

    void componentInit();

    //Distributor

    void distInit();
    void distAddtoCollectorList(wxCommandEvent& event);
    void distAddtoNodeList(wxCommandEvent& event);

    //Collector

    void collInit();
    void collUpdateID(wxCommandEvent &event);
    void collUpdateFileList(wxCommandEvent &event);
    void collUpdateFileListItem(wxCommandEvent &event);
    void collUpdateProcessList(wxCommandEvent &event);
    void collUpdateProcessListItem(wxCommandEvent &event);

    //Node

    void nodeInit();
    void nodeUpdateID(wxCommandEvent& event);
    void nodeUpdateState(wxCommandEvent& event);
    void nodeUpdateProcessList(wxCommandEvent &event);
    void nodeUpdateClear(wxCommandEvent &event);

////@begin Wx event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DIST_POLL
    void OnDistPollClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COLL_LOAD
    void OnCollLoadClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COLL_PROCESS
    void OnCollProcessClick( wxCommandEvent& event );

////@end Wx event handler declarations

    void OnDistPollClickWrapper( wxCommandEvent& event );
    void OnCollLoadClickWrapper( wxCommandEvent& event );
    void OnCollProcessClickWrapper( wxCommandEvent& event );

////@begin Wx member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end Wx member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin Wx member variables
    wxNotebook* mainPanel;
    wxPanel* distPanel;
    wxButton* distPollBtn;
    wxListCtrl* distCollList;
    wxListCtrl* distNodeList;
    wxPanel* collPanel;
    wxButton* collLoadBtn;
    wxButton* collProcessBtn;
    wxListCtrl* collFileList;
    wxListCtrl* collProcessList;
    wxPanel* nodePanel;
    wxStaticText* nodeState;
    wxListCtrl* nodeProcessList;
////@end Wx member variables
};

#endif
    // _WX_H_

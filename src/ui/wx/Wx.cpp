/////////////////////////////////////////////////////////////////////////////
// Name:        Wx.cpp
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Thu 03 May 2018 12:55:15 +03
// RCS-ID:      
// Copyright:   Licensed with AGPL v3.0
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "Wx.h"

////@begin XPM images
////@end XPM images


/*
 * Wx type definition
 */

IMPLEMENT_CLASS( Wx, wxFrame )


/*
 * Wx event table definition
 */

BEGIN_EVENT_TABLE( Wx, wxFrame )

////@begin Wx event table entries
    EVT_BUTTON( ID_DIST_POLL, Wx::OnDistPollClick )
    EVT_BUTTON( ID_BUTTON, Wx::OnButtonClick )
    EVT_BUTTON( ID_COLL_PROCESS, Wx::OnCollProcessClick )
////@end Wx event table entries

END_EVENT_TABLE()


/*
 * Wx constructors
 */

Wx::Wx(void* controller)
{
    Init(controller);
}

Wx::Wx(void* controller, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init(controller);
    Create(parent, id, caption, pos, size, style );
}


/*
 * Wx creator
 */

bool Wx::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin Wx creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end Wx creation

#ifndef DIALOG_BLOCKS
    componentInit();
#endif
    return true;
}


/*
 * Wx destructor
 */

Wx::~Wx()
{
////@begin Wx destruction
////@end Wx destruction
}


/*
 * Member initialisation
 */

void Wx::Init(void* controller)
{
////@begin Wx member initialisation
    distCollDeviceAddress = NULL;
    distPollBtn = NULL;
    distCollList = NULL;
    distNodeList = NULL;
    distNodeDeviceAddress = NULL;
    collNodeDeviceAddress = NULL;
    collLoadBtn = NULL;
    collProcessBtn = NULL;
    collFileList = NULL;
    collProcessList = NULL;
    collDistDeviceAddress = NULL;
    nodeCollAddress = NULL;
    nodeDeviceAddress = NULL;
    nodeState = NULL;
    nodeFileList = NULL;
    nodeExecList = NULL;
////@end Wx member initialisation

    componentController = controller;
}


/*
 * Control creation for Wx
 */

void Wx::CreateControls()
{    
////@begin Wx content construction
    Wx* itemFrame1 = this;

    wxNotebook* itemNotebook2 = new wxNotebook( itemFrame1, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );

    wxPanel* itemPanel3 = new wxPanel( itemNotebook2, ID_PANEL_DISTRIBUTOR, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemPanel3->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel3, wxID_STATIC, _("Collectors :"), wxPoint(10, 140), wxDefaultSize, 0 );

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel3, wxID_STATIC, _("Binded Address :"), wxPoint(10, 30), wxDefaultSize, 0 );

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel3, wxID_STATIC, _("Nodes :"), wxPoint(310, 140), wxDefaultSize, 0 );

    distCollDeviceAddress = new wxStaticText( itemPanel3, ID_DIST_BINDED_ADDRESS, _("0"), wxPoint(150, 30), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    distPollBtn = new wxButton( itemPanel3, ID_DIST_POLL, _("Poll"), wxPoint(10, 70), wxSize(560, 50), 0 );

    distCollList = new wxListCtrl( itemPanel3, ID_DIST_COLL_LIST, wxPoint(10, 180), wxSize(270, 330), wxLC_REPORT|wxLC_NO_HEADER );

    distNodeList = new wxListCtrl( itemPanel3, ID_DIST_NODE_LIST, wxPoint(310, 180), wxSize(270, 330), wxLC_REPORT|wxLC_NO_HEADER );

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel3, wxID_STATIC, _("Binded Address :"), wxPoint(310, 30), wxDefaultSize, 0 );

    distNodeDeviceAddress = new wxStaticText( itemPanel3, ID_STATICTEXT, _("0"), wxPoint(450, 30), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    itemNotebook2->AddPage(itemPanel3, _("Distributor"));

    wxPanel* itemPanel23 = new wxPanel( itemNotebook2, ID_PANEL_COLLECTOR, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemPanel23->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxStaticText* itemStaticText26 = new wxStaticText( itemPanel23, wxID_STATIC, _("Job Files :"), wxPoint(10, 140), wxDefaultSize, 0 );

    wxStaticText* itemStaticText27 = new wxStaticText( itemPanel23, wxID_STATIC, _("Binded Address :"), wxPoint(310, 30), wxDefaultSize, 0 );

    wxStaticText* itemStaticText29 = new wxStaticText( itemPanel23, wxID_STATIC, _("Process List :"), wxPoint(310, 140), wxDefaultSize, 0 );

    collNodeDeviceAddress = new wxStaticText( itemPanel23, ID_COLL_BINDED_ADDRESS, _("0"), wxPoint(450, 30), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    collLoadBtn = new wxButton( itemPanel23, ID_BUTTON, _("Load Job"), wxPoint(10, 70), wxSize(270, 50), 0 );

    collProcessBtn = new wxButton( itemPanel23, ID_COLL_PROCESS, _("Process"), wxPoint(310, 70), wxSize(270, 50), 0 );

    collFileList = new wxListCtrl( itemPanel23, ID_COLL_FILE_LIST, wxPoint(10, 180), wxSize(270, 330), wxLC_REPORT|wxLC_NO_HEADER );

    collProcessList = new wxListCtrl( itemPanel23, ID_COLL_PROCESS_LIST, wxPoint(310, 180), wxSize(270, 330), wxLC_REPORT|wxLC_NO_HEADER );

    wxStaticText* itemStaticText36 = new wxStaticText( itemPanel23, wxID_STATIC, _("Binded Address :"), wxPoint(10, 30), wxDefaultSize, 0 );

    collDistDeviceAddress = new wxStaticText( itemPanel23, ID_STATICTEXT1, _("0"), wxPoint(150, 30), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    itemNotebook2->AddPage(itemPanel23, _("Collector"));

    wxPanel* itemPanel38 = new wxPanel( itemNotebook2, ID_NODE_BINDED_ADDRESS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemPanel38->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    nodeCollAddress = new wxStaticText( itemPanel38, ID_NODE_COLL_ADDRESS, _("0"), wxPoint(160, 70), wxSize(120, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    wxStaticText* itemStaticText40 = new wxStaticText( itemPanel38, wxID_STATIC, _("Collector Address :"), wxPoint(10, 70), wxDefaultSize, 0 );

    wxStaticText* itemStaticText41 = new wxStaticText( itemPanel38, wxID_STATIC, _("Files :"), wxPoint(10, 140), wxDefaultSize, 0 );

    wxStaticText* itemStaticText42 = new wxStaticText( itemPanel38, wxID_STATIC, _("Binded Address :"), wxPoint(10, 30), wxDefaultSize, 0 );

    wxStaticText* itemStaticText43 = new wxStaticText( itemPanel38, wxID_STATIC, _("State :"), wxPoint(310, 70), wxDefaultSize, 0 );

    wxStaticText* itemStaticText44 = new wxStaticText( itemPanel38, wxID_STATIC, _("Executors :"), wxPoint(310, 140), wxDefaultSize, 0 );

    nodeDeviceAddress = new wxStaticText( itemPanel38, ID_NODE_BINDED_ADDRESS, _("0"), wxPoint(450, 30), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    nodeState = new wxStaticText( itemPanel38, ID_NODE_STATE, _("0"), wxPoint(450, 70), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    nodeFileList = new wxListCtrl( itemPanel38, ID_NODE_FILE_LIST, wxPoint(10, 180), wxSize(270, 330), wxLC_REPORT|wxLC_NO_HEADER );

    nodeExecList = new wxListCtrl( itemPanel38, ID_NODE_EXEC_LIST, wxPoint(310, 180), wxSize(270, 330), wxLC_REPORT|wxLC_NO_HEADER );

    itemNotebook2->AddPage(itemPanel38, _("Node"));

////@end Wx content construction
}

/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DIST_POLL
 */

void Wx::OnDistPollClick( wxCommandEvent& event )
{
#ifndef DIALOG_BLOCKS
    OnDistPollClickWrapper(event);
#endif
}

/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COLL_PROCESS
 */

void Wx::OnCollProcessClick( wxCommandEvent& event )
{
#ifndef DIALOG_BLOCKS
    OnCollProcessClickWrapper(event);
#endif
}

/*
 * Should we show tooltips?
 */

bool Wx::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap Wx::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin Wx bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end Wx bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon Wx::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin Wx icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end Wx icon retrieval
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void Wx::OnButtonClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON in Wx.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON in Wx. 
}


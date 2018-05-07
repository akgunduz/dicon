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
    EVT_BUTTON( ID_COLL_LOAD, Wx::OnCollLoadClick )
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
    mainPanel = NULL;
    distPanel = NULL;
    distPollBtn = NULL;
    distCollList = NULL;
    distNodeList = NULL;
    collPanel = NULL;
    collLoadBtn = NULL;
    collProcessBtn = NULL;
    collFileList = NULL;
    collProcessList = NULL;
    nodePanel = NULL;
    nodeState = NULL;
    nodeProcessList = NULL;
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

    mainPanel = new wxNotebook( itemFrame1, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );

    distPanel = new wxPanel( mainPanel, ID_PANEL_DISTRIBUTOR, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    distPanel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxStaticText* itemStaticText6 = new wxStaticText( distPanel, wxID_STATIC, _("Collectors :"), wxPoint(10, 90), wxDefaultSize, 0 );

    wxStaticText* itemStaticText11 = new wxStaticText( distPanel, wxID_STATIC, _("Nodes :"), wxPoint(310, 90), wxDefaultSize, 0 );

    distPollBtn = new wxButton( distPanel, ID_DIST_POLL, _("Poll"), wxPoint(10, 10), wxSize(570, 50), 0 );

    distCollList = new wxListCtrl( distPanel, ID_DIST_COLL_LIST, wxPoint(10, 130), wxSize(270, 360), wxLC_REPORT );

    distNodeList = new wxListCtrl( distPanel, ID_DIST_NODE_LIST, wxPoint(310, 130), wxSize(270, 360), wxLC_REPORT );

    mainPanel->AddPage(distPanel, _("Distributor"));

    collPanel = new wxPanel( mainPanel, ID_PANEL_COLLECTOR, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    collPanel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxStaticText* itemStaticText26 = new wxStaticText( collPanel, wxID_STATIC, _("Job Files :"), wxPoint(10, 90), wxDefaultSize, 0 );

    wxStaticText* itemStaticText29 = new wxStaticText( collPanel, wxID_STATIC, _("Process List :"), wxPoint(310, 90), wxDefaultSize, 0 );

    collLoadBtn = new wxButton( collPanel, ID_COLL_LOAD, _("Load Job"), wxPoint(10, 10), wxSize(270, 50), 0 );

    collProcessBtn = new wxButton( collPanel, ID_COLL_PROCESS, _("Process"), wxPoint(310, 10), wxSize(270, 50), 0 );

    collFileList = new wxListCtrl( collPanel, ID_COLL_FILE_LIST, wxPoint(10, 130), wxSize(270, 360), wxLC_REPORT|wxLC_NO_HEADER );

    collProcessList = new wxListCtrl( collPanel, ID_COLL_PROCESS_LIST, wxPoint(310, 130), wxSize(270, 360), wxLC_REPORT|wxLC_NO_HEADER );

    mainPanel->AddPage(collPanel, _("Collector"));

    nodePanel = new wxPanel( mainPanel, ID_PANEL_NODE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    nodePanel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxStaticText* itemStaticText41 = new wxStaticText( nodePanel, wxID_STATIC, _("Processes :"), wxPoint(10, 90), wxDefaultSize, 0 );

    wxStaticText* itemStaticText43 = new wxStaticText( nodePanel, wxID_STATIC, _("State :"), wxPoint(10, 10), wxSize(270, 50), 0 );

    nodeState = new wxStaticText( nodePanel, ID_NODE_STATE, _("0"), wxPoint(310, 10), wxSize(270, 50), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    nodeProcessList = new wxListCtrl( nodePanel, ID_NODE_FILE_LIST, wxPoint(10, 130), wxSize(570, 360), wxLC_REPORT );

    mainPanel->AddPage(nodePanel, _("Node"));

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
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void Wx::OnCollLoadClick( wxCommandEvent& event )
{
#ifndef DIALOG_BLOCKS
    OnCollLoadClickWrapper(event);
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


#ifndef __UserInterface__
#define __UserInterface__

#include "UserInterfaceGenerated.h"
#include "Distributor.h"
#include "Collector.h"
//
enum PAGES {
	PAGE_DIST,
	PAGE_COLL,
	PAGE_CLIENT,
	PAGE_MAX
};

class UserInterface;

typedef void (UserInterface::*fUIUpdater)(wxCommandEvent &event);

class UserInterface : public MainFrame {
protected:
	wxEventTypeTag<wxCommandEvent> *ui_event;

	fUIUpdater uiUpdater[UI_UPDATE_MAX];

public:
	UserInterface(const wxString &title, const wxPoint &pos, const wxSize &size);

	void cleanupUnixSocketPath();

	//Generic

	void updateUIEvent(int, void*);

	void updateUI(wxCommandEvent &event);
	void updateLog(wxCommandEvent &event);

	//Distributor

	Distributor *distObject;

	void distInit();

	void onDistInitClick(wxCommandEvent& event);
	void distUpdateAddresses(wxCommandEvent& event);
	void onDistPollClick(wxCommandEvent& event);
	void distAddtoCollectorList(wxCommandEvent& event);
	void distAddtoClientList(wxCommandEvent& event);

	void onDistCleanupClick(wxCommandEvent& event);

	void distUpdateBackup(wxCommandEvent &event);
	void distUpdateLog(wxCommandEvent &event);


	//Collector

	Collector *collObject;

	void collInit();

	void onCollInitClick(wxCommandEvent& event);
	void collUpdateAddresses(wxCommandEvent& event);
	void collUpdateAttachedDistAddress(wxCommandEvent& event);
	void collUpdateAttachedClientAddress(wxCommandEvent& event);

	void collUpdateLog(wxCommandEvent &event);
	void collUpdateFileList(wxCommandEvent &event);
	void collUpdateParamList(wxCommandEvent &event);
	void collUpdateExecList(wxCommandEvent &event);

	void onCollProcessClick(wxCommandEvent &event);
	void onCollProcessSliderScroll( wxScrollEvent& event );

	//Node

	Node *clientObject;

	void clientInit();

	void onClientInitClick(wxCommandEvent& event);
	void clientUpdateAddresses(wxCommandEvent& event);
	void clientUpdateState(wxCommandEvent& event);
	void clientUpdateAttachedCollAddress(wxCommandEvent& event);

	void clientUpdateLog(wxCommandEvent &event);
	void clientUpdateFileList(wxCommandEvent &event);
	void clientUpdateParamList(wxCommandEvent &event);
	void clientUpdateExecList(wxCommandEvent &event);

	~UserInterface();

};

#endif // __UserInterface__

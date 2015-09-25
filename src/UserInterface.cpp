
#include "UserInterface.h"
//

UserInterface::UserInterface( const wxString& title, const wxPoint& pos, const wxSize& size )
: MainFrame( NULL, wxID_ANY, title, pos, size) {

	ui_event = new wxEventTypeTag<wxCommandEvent>(wxNewEventType());
	Bind(*ui_event, &UserInterface::updateUI, this, 0, UI_UPDATE_MAX - 1, NULL);
	uiUpdater[UI_UPDATE_LOG] = &UserInterface::updateLog;

	cleanupUnixSocketPath();

	distInit();
	collInit();
	clientInit();

	rootpathPicker->SetPath(getcwd(NULL, 0));

}

void UserInterface::cleanupUnixSocketPath() {

	DIR *unixdir = opendir(UNIXSOCKET_PATH);
	if (!unixdir) {
		printf("Can not open unix socket path!!!");
		return;
	}

	dirent *entry;

	while ((entry = readdir(unixdir)) != NULL) {
		if (strncmp(entry->d_name, "bankor_", 7) == 0) {
			std::string path = std::string(UNIXSOCKET_PATH) + entry->d_name;
			unlink(path.c_str());
		}
	}

	closedir(unixdir);

}

void UserInterface::updateUIEvent(int id, void *data) {

	wxCommandEvent event(*ui_event);
	event.SetId(id);
	event.SetClientData(data);
	wxPostEvent(this, event);

}

void UserInterface::updateUI(wxCommandEvent& event) {

	int id = event.GetId();

	if (uiUpdater[id] != nullptr) {
		((this)->*(uiUpdater[id]))(event);
	}

}

void UserInterface::updateLog(wxCommandEvent& event) {

	EventData *data = (EventData *)event.GetClientData();
	genericLog->Append(data->dataStr);

}

UserInterface::~UserInterface() {
}



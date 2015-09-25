#include <wx/event.h>
#include "UserInterface.h"

void UserInterface::distInit() {

	wxSize unit = distCollList->GetSize() / 2;

	wxListItem column;
	column.SetId(0);
	column.SetText( _("Address") );
	column.SetWidth(unit.GetWidth());
	distCollList->InsertColumn(0, column);

	column.SetId(1);
	column.SetText( _("Attached Client") );
	column.SetWidth(unit.GetWidth());
	distCollList->InsertColumn(1, column);

	column.SetId(2);
	column.SetText( _("Hidden Address") );
	column.SetWidth(0);
	distCollList->InsertColumn(2, column);

	unit = distClientList->GetSize() / 2;

	column.SetId(0);
	column.SetText( _("Address") );
	column.SetWidth(unit.GetWidth());
	distClientList->InsertColumn(0, column);

	column.SetId(1);
	column.SetText( _("State") );
	column.SetWidth(unit.GetWidth());
	distClientList->InsertColumn(1, column);

	column.SetId(2);
	column.SetText( _("Hidden Address") );
	column.SetWidth(0);
	distClientList->InsertColumn(2, column);

	uiUpdater[UI_UPDATE_DIST_ADDRESS] = &UserInterface::distUpdateAddresses;
	uiUpdater[UI_UPDATE_DIST_COLL_LIST] = &UserInterface::distAddtoCollectorList;
	uiUpdater[UI_UPDATE_DIST_CLIENT_LIST] = &UserInterface::distAddtoClientList;
	uiUpdater[UI_UPDATE_DIST_BACKUP] = &UserInterface::distUpdateBackup;
	uiUpdater[UI_UPDATE_DIST_LOG] = &UserInterface::distUpdateLog;

	for (uint32_t i = 0; i < ConnInterface::getInterfaceCount(); i++) {
		distCollInterface->Insert(wxString(sInterfaces[ConnInterface::getInterfaceType(i)]) + " --> " + ConnInterface::getInterfaceName(i), i);
		distClientInterface->Insert(wxString(sInterfaces[ConnInterface::getInterfaceType(i)]) + " --> " + ConnInterface::getInterfaceName(i), i);
	}

}

void UserInterface::onDistInitClick( wxCommandEvent& event ) {

	if (wxStrcmp(distInitBtn->GetLabel(), "Init") == 0) {

		try {
                        distCollList->DeleteAllItems();
			std::string path = std::string(getcwd(nullptr, 0)) + "/" + DISTRIBUTOR_PATH + "/";
                        mkdir(path.c_str(), 0777);

			double backupRate = 0;
			distBackupRate->GetLineText(0).ToDouble(&backupRate);

			distObject = new Distributor((uint32_t)distCollInterface->GetSelection(),
					(uint32_t)distClientInterface->GetSelection(), path, backupRate);

		} catch (std::runtime_error &e) {

			return;
		}

		distBackupRate->SetEditable(false);
		distInitBtn->SetLabel("Stop");

	} else {

		delete distObject;
		distBackupRate->SetEditable(true);
		distInitBtn->SetLabel("Init");
		distCollInterfaceAddress->SetLabel("");
		distClientInterfaceAddress->SetLabel("");

	}
}

void UserInterface::distUpdateAddresses(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	distCollInterfaceAddress->SetLabel(Tools::getAddressStr(data->data64_1));
	distClientInterfaceAddress->SetLabel(Tools::getAddressStr(data->data64_2));

}

void UserInterface::onDistPollClick(wxCommandEvent &event) {

	distObject->reset();
	distClientList->DeleteAllItems();

	distObject->sendWakeupMessagesAll();

}

void UserInterface::distAddtoCollectorList(wxCommandEvent &event) {

	long i = 0;

	EventData *data = (EventData *)event.GetClientData();

	for (; i < distCollList->GetItemCount(); i++) {

		std::string item = distCollList->GetItemText(i, 2).ToStdString();

		if (data->data64_1 == (uint64_t)atoll(item.c_str())) {
			break;
		}

	}

	if (i == distCollList->GetItemCount()) {
		i = distCollList->InsertItem(distCollList->GetItemCount(), 0);
	}

	distCollList->SetItem(i, 0, Tools::getAddressStr(data->data64_1));
	distCollList->SetItem(i, 2, std::to_string(data->data64_1));

	if (data->data64_2 > 0) {
		distCollList->SetItem(i, 1, Tools::getAddressStr(data->data64_2));
	} else {
		distCollList->SetItem(i, 1, "No Available Client!!");
	}

}

void UserInterface::distAddtoClientList(wxCommandEvent &event) {

	long i = 0;

	EventData *data = (EventData *)event.GetClientData();

	for (; i < distClientList->GetItemCount(); i++) {

		std::string item = distClientList->GetItemText(i, 2).ToStdString();

		if (data->data64_1 == (uint64_t)atoll(item.c_str())) {
			break;
		}

	}

	if (i == distClientList->GetItemCount()) {
		i = distClientList->InsertItem(distClientList->GetItemCount(), 0);
	}

	distClientList->SetItem(i, 0, Tools::getAddressStr(data->data64_1));
	distClientList->SetItem(i, 1, sStates[data->data64_2]);
	distClientList->SetItem(i, 2, std::to_string(data->data64_1));

}

void UserInterface::distUpdateBackup(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();

	distBackupCount->SetLabelText(std::to_string(data->data64_1));
	distBackupReady->SetLabelText(std::to_string(data->data64_2));

}

void UserInterface::distUpdateLog(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();

	distLog->Append(wxString::Format("%s", data->dataStr));

}

void UserInterface::onDistCleanupClick(wxCommandEvent &event) {

	cleanupUnixSocketPath();
	
}


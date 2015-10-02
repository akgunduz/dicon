#include <wx/event.h>
#include "UserInterface.h"
#include "Tools.h"
//

void UserInterface::clientInit() {

	wxSize unit = clientFileList->GetSize() / 10;

	wxListItem column;
	column.SetId(0);
	column.SetText( _("Name") );
	column.SetWidth(unit.GetWidth() * 4);
	clientFileList->InsertColumn(0, column);

	column.SetId(1);
	column.SetText( _("MD5") );
	column.SetWidth(unit.GetWidth() * 4);
	clientFileList->InsertColumn(1, column);

	column.SetId(2);
	column.SetText( _("State") );
	column.SetWidth(unit.GetWidth());
	clientFileList->InsertColumn(2, column);

	column.SetId(3);
	column.SetText( _("Flag") );
	column.SetWidth(unit.GetWidth());
	clientFileList->InsertColumn(3, column);

	column.SetId(0);
	column.SetText( _("Type") );
	column.SetWidth(unit.GetWidth() * 2);
	clientParamList->InsertColumn(0, column);

	column.SetId(1);
	column.SetText( _("Value") );
	column.SetWidth(unit.GetWidth() * 3);
	clientParamList->InsertColumn(1, column);

	column.SetId(0);
	column.SetText( _("Executor") );
	column.SetWidth(unit.GetWidth() * 5);
	clientExecList->InsertColumn(0, column);

	uiUpdater[UI_UPDATE_CLIENT_ADDRESS] = &UserInterface::clientUpdateAddresses;
	uiUpdater[UI_UPDATE_CLIENT_STATE] = &UserInterface::clientUpdateState;
	uiUpdater[UI_UPDATE_CLIENT_ATT_COLL_ADDRESS] = &UserInterface::clientUpdateAttachedCollAddress;
	uiUpdater[UI_UPDATE_CLIENT_FILE_LIST] = &UserInterface::clientUpdateFileList;
	uiUpdater[UI_UPDATE_CLIENT_PARAM_LIST] = &UserInterface::clientUpdateParamList;
	uiUpdater[UI_UPDATE_CLIENT_EXEC_LIST] = &UserInterface::clientUpdateExecList;
	uiUpdater[UI_UPDATE_CLIENT_LOG] = &UserInterface::clientUpdateLog;

	for (uint32_t i = 0; i < ConnInterface::getInterfaceCount(); i++) {
		clientDistInterface->Insert(wxString(sInterfaces[ConnInterface::getInterfaceType(i)]) + " --> " + ConnInterface::getInterfaceName(i), i);
		clientCollInterface->Insert(wxString(sInterfaces[ConnInterface::getInterfaceType(i)]) + " --> " + ConnInterface::getInterfaceName(i), i);
	}

}

void UserInterface::onClientInitClick( wxCommandEvent& event ) {

	if (wxStrcmp(clientInitBtn->GetLabel(), "Init") == 0) {

		try {
			std::string path = std::string(getcwd(nullptr, 0)) + "/" + CLIENT_PATH + "/";
                        mkdir(path.c_str(), 0777);

			clientObject = new Client((uint32_t)clientDistInterface->GetSelection(),
					(uint32_t)clientCollInterface->GetSelection(), path);

		} catch (std::runtime_error &e) {

			return;
		}

		clientInitBtn->SetLabel("Stop");

	} else {

		delete clientObject;
		clientInitBtn->SetLabel("Init");
		clientDistInterfaceAddress->SetLabel("");
		clientCollInterfaceAddress->SetLabel("");
	}
}

void UserInterface::clientUpdateAddresses(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	clientDistInterfaceAddress->SetLabel(Address::getString(data->data64_1));
	clientCollInterfaceAddress->SetLabel(Address::getString(data->data64_2));

}

void UserInterface::clientUpdateState(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	clientState->SetLabel(sStates[data->data64_1]);

}

void UserInterface::clientUpdateAttachedCollAddress(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	clientCollAddress->SetLabel(Address::getString(data->data64_1));

}

void UserInterface::clientUpdateLog(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	clientLog->Append(wxString::Format("%s", data->dataStr));

}

void UserInterface::clientUpdateFileList(wxCommandEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(RULE_FILES); j++) {

		FileContent *content = (FileContent *) rule->getContent(RULE_FILES, j);
		if (content == nullptr) {
			return;
		}

		long i = 0;

		for (; i < clientFileList->GetItemCount(); i++) {

			if (clientFileList->GetItemText(i, 0).Cmp(content->getPath()) == 0) {
				break;
			}

		}

		if (i == clientFileList->GetItemCount()) {
			i = clientFileList->InsertItem(clientFileList->GetItemCount(), "");
		}

		clientFileList->SetItem(i, 0, content->getPath());
		clientFileList->SetItem(i, 1, Tools::getMD5Str(content->getMD5()));
		clientFileList->SetItem(i, 2, content->isValid() ? "V" : "I");
		clientFileList->SetItem(i, 3, content->isFlaggedToSent() ? "Y" : "N");
	}

}

void UserInterface::clientUpdateParamList(wxCommandEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(RULE_PARAMETERS); j++) {

		ParamContent *content = (ParamContent *) rule->getContent(RULE_PARAMETERS, j);
		if (content == nullptr) {
			return;
		}

		long index = clientParamList->InsertItem(clientParamList->GetItemCount(), "");
		clientParamList->SetItem(index, 0, sParamTypes[content->getParamType()]);
		switch(content->getParamType()) {
			case PARAM_LONG:
				clientParamList->SetItem(index, 1, wxString::Format(wxT("%i"), content->getParam().latom));
				break;
			case PARAM_DOUBLE:
				clientParamList->SetItem(index, 1, wxString::Format(wxT("%f"), content->getParam().datom));
				break;
			case PARAM_STRING:
				clientParamList->SetItem(index, 1, content->getParam().sPtr);
				break;
			default:
				break;
		}
	}
}

void UserInterface::clientUpdateExecList(wxCommandEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(RULE_EXECUTORS); j++) {

		ExecutorContent *content = (ExecutorContent *) rule->getContent(RULE_EXECUTORS, j);
		if (content == nullptr) {
			return;
		}

		clientExecList->InsertItem(clientExecList->GetItemCount(), content->getExec());

	}

}

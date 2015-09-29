#include <wx/event.h>
#include "UserInterface.h"
#include "Tools.h"
//

void UserInterface::collInit() {

	wxSize unit = collFileList->GetSize() / 10;

	wxListItem column;
	column.SetId(0);
	column.SetText( _("Name") );
	column.SetWidth(unit.GetWidth() * 4);
	collFileList->InsertColumn(0, column);

	column.SetId(1);
	column.SetText( _("MD5") );
	column.SetWidth(unit.GetWidth() * 4);
	collFileList->InsertColumn(1, column);

	column.SetId(2);
	column.SetText( _("State") );
	column.SetWidth(unit.GetWidth());
	collFileList->InsertColumn(2, column);

	column.SetId(3);
	column.SetText( _("Flag") );
	column.SetWidth(unit.GetWidth());
	collFileList->InsertColumn(3, column);

	column.SetId(0);
	column.SetText( _("Type") );
	column.SetWidth(unit.GetWidth() * 2);
	collParamList->InsertColumn(0, column);

	column.SetId(1);
	column.SetText( _("Value") );
	column.SetWidth(unit.GetWidth() * 3);
	collParamList->InsertColumn(1, column);

	column.SetId(0);
	column.SetText( _("Executor") );
	column.SetWidth(unit.GetWidth() * 5);
	collExecList->InsertColumn(0, column);

	uiUpdater[UI_UPDATE_COLL_ADDRESS] = &UserInterface::collUpdateAddresses;
	uiUpdater[UI_UPDATE_COLL_ATT_DIST_ADDRESS] = &UserInterface::collUpdateAttachedDistAddress;
	uiUpdater[UI_UPDATE_COLL_ATT_CLIENT_ADDRESS] = &UserInterface::collUpdateAttachedClientAddress;
	uiUpdater[UI_UPDATE_COLL_FILE_LIST] = &UserInterface::collUpdateFileList;
	uiUpdater[UI_UPDATE_COLL_PARAM_LIST] = &UserInterface::collUpdateParamList;
	uiUpdater[UI_UPDATE_COLL_EXEC_LIST] = &UserInterface::collUpdateExecList;
	uiUpdater[UI_UPDATE_COLL_LOG] = &UserInterface::collUpdateLog;

	for (uint32_t i = 0; i < ConnInterface::getInterfaceCount(); i++) {
		collDistInterface->Insert(wxString(sInterfaces[ConnInterface::getInterfaceType(i)]) + " --> " + ConnInterface::getInterfaceName(i), i);
		collClientInterface->Insert(wxString(sInterfaces[ConnInterface::getInterfaceType(i)]) + " --> " + ConnInterface::getInterfaceName(i), i);
	}

}

void UserInterface::onCollInitClick( wxCommandEvent& event ) {

	if (wxStrcmp(collInitBtn->GetLabel(), "Init") == 0) {

		try {
			std::string path = std::string(getcwd(nullptr, 0)) + "/" + COLLECTOR_PATH + "/";
                        mkdir(path.c_str(), 0777);

			collObject = new Collector((uint32_t)collDistInterface->GetSelection(),
					(uint32_t)collClientInterface->GetSelection(), path);

		} catch (std::runtime_error &e) {

			return;
		}

		collInitBtn->SetLabel("Stop");

	} else {

		delete collObject;
		collInitBtn->SetLabel("Init");
		collDistInterfaceAddress->SetLabel("");
		collClientInterfaceAddress->SetLabel("");
	}
}

void UserInterface::collUpdateAddresses(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	collDistInterfaceAddress->SetLabel(Tools::getAddressStr(data->data64_1));
	collClientInterfaceAddress->SetLabel(Tools::getAddressStr(data->data64_2));

}

void UserInterface::collUpdateAttachedDistAddress(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	collDistAddress->SetLabel(Tools::getAddressStr(data->data64_1));

}

void UserInterface::collUpdateAttachedClientAddress(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	collClientAddress->SetLabel(Tools::getAddressStr(data->data64_1));

}

void UserInterface::collUpdateLog(wxCommandEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	collLog->Append(wxString::Format("%s", data->dataStr));

}

void UserInterface::collUpdateFileList(wxCommandEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(RULE_FILES); j++) {

		FileContent *content = (FileContent *) rule->getContent(RULE_FILES, j);
		if (content == nullptr) {
			return;
		}

		long i = 0;

		for (; i < collFileList->GetItemCount(); i++) {

			if (collFileList->GetItemText(i, 0).Cmp(content->getPath()) == 0) {
				break;
			}

		}

		if (i == collFileList->GetItemCount()) {
			i = collFileList->InsertItem(collFileList->GetItemCount(), "");
		}

		collFileList->SetItem(i, 0, content->getPath());
		collFileList->SetItem(i, 1, Tools::getMD5Str(content->getMD5()));
		collFileList->SetItem(i, 2, content->isValid() ? "V" : "I");
		collFileList->SetItem(i, 3, content->isFlaggedToSent() ? "Y" : "N");
	}

}

void UserInterface::collUpdateParamList(wxCommandEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(RULE_PARAMETERS); j++) {

		ParamContent *content = (ParamContent *) rule->getContent(RULE_PARAMETERS, j);
		if (content == nullptr) {
			return;
		}

		long index = collParamList->InsertItem(collParamList->GetItemCount(), "");
		collParamList->SetItem(index, 0, sParamTypes[content->getParamType()]);
		switch(content->getParamType()) {
			case PARAM_LONG:
				collParamList->SetItem(index, 1, wxString::Format(wxT("%i"), content->getParam().latom));
				break;
			case PARAM_DOUBLE:
				collParamList->SetItem(index, 1, wxString::Format(wxT("%f"), content->getParam().datom));
				break;
			case PARAM_STRING:
				collParamList->SetItem(index, 1, content->getParam().sPtr);
				break;
			default:
				break;
		}
	}
}

void UserInterface::collUpdateExecList(wxCommandEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(RULE_EXECUTORS); j++) {

		ExecutorContent *content = (ExecutorContent *) rule->getContent(RULE_EXECUTORS, j);
		if (content == nullptr) {
			return;
		}

		collExecList->InsertItem(collExecList->GetItemCount(), content->getExec());

	}

}

void UserInterface::onCollProcessClick(wxCommandEvent &event) {

	collFileList->DeleteAllItems();
	collParamList->DeleteAllItems();
	collExecList->DeleteAllItems();

	collObject->syncTime();

	for(int i = 0; i < collProcessSlider->GetValue(); i++) {
		collObject->processRule();
	}

}

void UserInterface::onCollProcessSliderScroll(wxScrollEvent &event) {

	collProcessLabel->SetLabel(wxString::Format(wxT("%i"), collProcessSlider->GetValue()));

}


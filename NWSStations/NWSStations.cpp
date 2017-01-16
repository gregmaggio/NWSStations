// NWSStations.cpp : Defines the entry point for the console application.
//

#include <Objbase.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wchar.h>
#include "States.h"
#include "Stations.h"
#include "json.h"
#include <sstream>

using namespace std;

#include "ogrsf_frmts.h"

#import <msxml6.dll>

void loadFromXml(string& fileName, CStations * stations, CStates * states)
{
	MSXML2::IXMLDOMDocument2Ptr document = NULL;
	document.CreateInstance(__uuidof(MSXML2::DOMDocument60));
	document->load(_variant_t(_bstr_t(fileName.c_str())));
	MSXML2::IXMLDOMNodeListPtr nodes = document->selectNodes(_bstr_t("//wx_station_index/station"));
	for (long ii = 0; ii < nodes->length; ii++)
	{
		MSXML2::IXMLDOMNodePtr node = nodes->Getitem(ii);
		string stationId = "\0";
		string stationName = "\0";
		string stateCode = "\0";
		double latitude = 0.0;
		double longitude = 0.0;
		if (node->hasChildNodes() == VARIANT_TRUE)
		{
			MSXML2::IXMLDOMNodeListPtr childNodes = node->childNodes;
			for (long jj = 0; jj < childNodes->length; jj++)
			{
				MSXML2::IXMLDOMNodePtr child = childNodes->Getitem(jj);
				if (_wcsicmp(child->nodeName, L"station_id") == 0)
				{
					stationId = (char *)child->Gettext();
				}
				else if (_wcsicmp(child->nodeName, L"station_name") == 0)
				{
					stationName = (char *)child->Gettext();
				}
				else if (_wcsicmp(child->nodeName, L"state") == 0)
				{
					stateCode = (char *)child->Gettext();
				}
				else if (_wcsicmp(child->nodeName, L"latitude") == 0)
				{
					latitude = stod((char *)child->Gettext());
				}
				else if (_wcsicmp(child->nodeName, L"longitude") == 0)
				{
					longitude = stod((char *)child->Gettext());
				}
			}
		}
		CStation * station = stations->find(stationId);
		if (!station)
		{
			CState * state = states->find(stateCode);
			if (state)
			{
				OGRPoint point(longitude, latitude);
				OGRFeature * feature = feature = OGRFeature::CreateFeature(stations->getFeatureDefn());
				feature->SetField("station_id", stationId.c_str());
				feature->SetField("name", stationName.c_str());
				feature->SetField("street_no", "\0");
				feature->SetField("street", "\0");
				feature->SetField("city", "\0");
				feature->SetField("state_cd", state->getCode());
				feature->SetField("state_nm", state->getName());
				feature->SetField("zip", "\0");
				feature->SetField("country_cd", "\0");
				feature->SetField("country", "\0");
				feature->SetField("latitude", latitude);
				feature->SetField("longitude", longitude);
				feature->SetField("has_rad", "N");
				feature->SetGeometry(&point);
				stations->getLayer()->CreateFeature(feature);
				long fid = feature->GetFID();
				OGRFeature::DestroyFeature(feature);
				station = new CStation(stations->getLayer()->GetFeature(fid));
				if (!stations->add(station))
				{
					station->Release();
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	CStations * stations = NULL;
	CStates * states = NULL;
	try
	{
		CoInitialize(NULL);

		string xmlFileName = "C:\\Dev\\Applications\\NWSStations\\NWSStations\\data\\stations.xml";
		string shapeFileName = "C:\\Dev\\Applications\\NWSStations\\NWSStations\\data\\stations.shp";
		string stateFileName = "C:\\Dev\\Applications\\NWSStations\\NWSStations\\data\\igra2-us-states.txt";
		for (int ii = 0; ii < argc; ) {
			char* arg = argv[ii++];
			if (_stricmp(arg, "--xmlFileName") == 0) {
				if (ii < argc) {
					xmlFileName = argv[ii++];
				}
			} else if (_stricmp(arg, "--shapeFileName") == 0) {
				if (ii < argc) {
					shapeFileName = argv[ii++];
				}
			} else if (_stricmp(arg, "--stateFileName") == 0) {
				if (ii < argc) {
					stateFileName = argv[ii++];
				}
			}
		}

		OGRRegisterAll();
		
		stations = new CStations(shapeFileName);
		states = new CStates(stateFileName);

		loadFromXml(xmlFileName, stations, states);

		stations->reverseGeocodeAddress();
		stations->checkForRadisondes();
		stations->save();
	}
	catch (exception& ex)
	{
		cout << "Exception: " << ex.what() << endl;
	}
	catch (...)
	{
		cout << "Unknown Exception!" << endl;
	}
	if (stations)
	{
		stations->Release();
	}
	if (states)
	{
		states->Release();
	}

	CoUninitialize();

	system("pause");

	_ASSERT(CStations::getInstances() == 0);
	_ASSERT(CStation::getInstances() == 0);
	_ASSERT(CStates::getInstances() == 0);
	_ASSERT(CState::getInstances() == 0);

	return 0;
}


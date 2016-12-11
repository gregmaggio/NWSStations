#include "Stations.h"
#include <fstream>
#include <iostream>

unsigned long CStations::_instances = 0;

CStations::CStations(void)
{
	InterlockedIncrement(&_instances);
}


CStations::~CStations(void)
{
	for (vector<CStation*>::iterator item = _items.begin(); item != _items.end(); item++)
	{
		(*item)->Release();
	}
	_items.clear();
	InterlockedDecrement(&_instances);
}

unsigned long CStations::getInstances()
{
	return _instances;
}

void CStations::load(string& fileName)
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
		string state = "\0";
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
					state = (char *)child->Gettext();
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
		CStation * station = new CStation();
		station->setStationId(stationId);
		station->setStationName(stationName);
		station->setState(state);
		station->setLatitude(latitude);
		station->setLongitude(longitude);
		_items.push_back(station);
	}
}

void CStations::checkForRadisondes()
{
	MSXML2::IXMLHTTPRequestPtr httpRequest = NULL;
	httpRequest.CreateInstance(__uuidof(MSXML2::XMLHTTP60));

	for (vector<CStation*>::iterator item = _items.begin(); item != _items.end(); item++)
	{
		char url[255];
		memset(url, 0, sizeof(char) * 255);
		sprintf_s(url, "http://weather.unisys.com/upper_air/skew/skew_%s.gif", ((*item)->getStationId().c_str()));
		cout << "Checking if " << url << " exists..." << endl;
		httpRequest->open(_bstr_t(L"GET"), _bstr_t(url), VARIANT_FALSE);
		httpRequest->send();
		bool hasRadiosonde = (httpRequest->Getstatus() != 404) ? true : false;
		(*item)->setHasRadisonde(hasRadiosonde);
	}
}

CStation * CStations::getItem(long index)
{
	return _items[index];
}

long CStations::getLength()
{
	return _items.size();
}
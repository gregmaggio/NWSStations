#include "Stations.h"
#include <fstream>
#include <iostream>
#include "json.h"
#include <sstream>

#import <msxml6.dll>

unsigned long CStations::_instances = 0;

CStations::CStations(string& shapeFileName)
	: _dataSource(NULL), _layer(NULL), _featureDefn(NULL)
{
	InterlockedIncrement(&_instances);
	if (fileExists(shapeFileName))
	{
		_dataSource = openDataSource(shapeFileName);
	}
	else
	{
		_dataSource = createDataSource(shapeFileName);
	}
	_layer = _dataSource->GetLayer(0);
	_featureDefn = _layer->GetLayerDefn();
	loadFromShapeFile(shapeFileName);
}

CStations::~CStations(void)
{
	for (vector<CStation*>::iterator item = _items.begin(); item != _items.end(); item++)
	{
		(*item)->Release();
	}
	if (_dataSource)
	{
		_dataSource->Release();
	}
	_items.clear();
	_dataSource = NULL;
	_layer = NULL;
	_featureDefn = NULL;
	InterlockedDecrement(&_instances);
}

unsigned long CStations::getInstances()
{
	return _instances;
}

OGRDataSource * CStations::getDataSource()
{
	return _dataSource;
}

OGRLayer * CStations::getLayer()
{
	return _layer;
}

OGRFeatureDefn * CStations::getFeatureDefn()
{
	return _featureDefn;
}

bool CStations::fileExists(string& fileName)
{
    ifstream f(fileName.c_str());
    return f.good();
}

OGRSFDriver * CStations::getShapeFileDriver()
{
	OGRSFDriverRegistrar * registrar = OGRSFDriverRegistrar::GetRegistrar();
	int driverCount = registrar->GetDriverCount();
	cout << "driverCount: " << driverCount << endl;
	for (int ii = 0; ii < driverCount; ii++)
	{
		OGRSFDriver * driver = registrar->GetDriver(ii);
		cout << "driver: " << driver->GetName() << endl;
		if (_stricmp(driver->GetName(), "ESRI Shapefile") == 0)
		{
			return driver;
		}
	}
	return NULL;
}

OGRDataSource * CStations::createDataSource(string& fileName)
{
	OGRSFDriver * shapeFileDriver = getShapeFileDriver();
	OGRDataSource * dataSource = shapeFileDriver->CreateDataSource(fileName.c_str());
	OGRLayer * layer = dataSource->CreateLayer("stations", NULL, wkbPoint, NULL);
	OGRFieldDefn stationIdField("station_id", OFTString);
	stationIdField.SetWidth(55);
	if (layer->CreateField(&stationIdField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating station_id field.");
	}
	OGRFieldDefn stationNameField("name", OFTString);
	stationNameField.SetWidth(55);
	if (layer->CreateField(&stationNameField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating name field.");
	}
	OGRFieldDefn streetNoField("street_no", OFTString);
	streetNoField.SetWidth(15);
	if (layer->CreateField(&streetNoField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating street_no field.");
	}
	OGRFieldDefn streetField("street", OFTString);
	streetField.SetWidth(155);
	if (layer->CreateField(&streetField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating street field.");
	}
	OGRFieldDefn cityField("city", OFTString);
	cityField.SetWidth(155);
	if (layer->CreateField(&cityField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating city field.");
	}
	OGRFieldDefn stateField("state", OFTString);
	stateField.SetWidth(5);
	if (layer->CreateField(&stateField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating state field.");
	}
	OGRFieldDefn zipField("zip", OFTString);
	zipField.SetWidth(15);
	if (layer->CreateField(&zipField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating zip field.");
	}
	OGRFieldDefn countryCodeField("country_cd", OFTString);
	countryCodeField.SetWidth(5);
	if (layer->CreateField(&countryCodeField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating country_cd field.");
	}
	OGRFieldDefn countryField("country", OFTString);
	countryField.SetWidth(155);
	if (layer->CreateField(&countryField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating country field.");
	}
	OGRFieldDefn latitudeField("latitude", OFTReal);
	if (layer->CreateField(&latitudeField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating latitude field.");
	}
	OGRFieldDefn longitudeField("longitude", OFTReal);
	if (layer->CreateField(&longitudeField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating longitude field.");
	}
	OGRFieldDefn hasRadisondeField("has_rad", OFTString);
	if (layer->CreateField(&hasRadisondeField) != OGRERR_NONE)
	{
		dataSource->Release();
		throw exception("Error creating has_rad field.");
	}
	return dataSource;
}

OGRDataSource * CStations::openDataSource(string& fileName)
{
	OGRSFDriver * shapeFileDriver = getShapeFileDriver();
	return shapeFileDriver->Open(fileName.c_str(), TRUE);
}

void CStations::loadFromShapeFile(string& fileName)
{
	long featureCount = _layer->GetFeatureCount();
	for (long ii = 0; ii < featureCount; ii++)
	{
		OGRFeature * feature = _layer->GetFeature(ii);
		CStation * station = new CStation(feature);
		if (!add(station))
		{
			station->Release();
		}
	}
}

bool CStations::add(CStation * station)
{
	string stationId = station->getStationId();
	if (_indexByStationId.find(stationId) != _indexByStationId.end())
	{
		return false;
	}
	_items.push_back(station);
	long index = _items.size() - 1;
	_indexByStationId.insert(pair<string, long>(stationId, index));
	return true;
}

CStation * CStations::find(string& stationId)
{
	map<string, long>::iterator iter = _indexByStationId.find(stationId);
	if (iter != _indexByStationId.end())
	{
		return _items[(*iter).second];
	}
	return NULL;
}

void CStations::checkForRadisondes()
{
	checkForRadisondes(0, _items.size() - 1);
}

void CStations::checkForRadisondes(long start, long end)
{
	MSXML2::IXMLHTTPRequestPtr httpRequest = NULL;
	httpRequest.CreateInstance(__uuidof(MSXML2::XMLHTTP60));

	for (long index = start; index <= end; index++)
	{
		CStation * station = _items[index];
		char url[255];
		memset(url, 0, sizeof(char) * 255);
		sprintf_s(url, "http://weather.unisys.com/upper_air/skew/skew_%s.gif", station->getStationId());
		cout << "Checking if " << url << " exists..." << endl;
		httpRequest->open(_bstr_t(L"GET"), _bstr_t(url), VARIANT_FALSE);
		httpRequest->send();
		bool hasRadiosonde = (httpRequest->Getstatus() != 404) ? true : false;
		station->setHasRadisonde(hasRadiosonde);
	}
}

void CStations::reverseGeocodeAddress()
{
	reverseGeocodeAddress(0, _items.size() - 1);
}

void CStations::reverseGeocodeAddress(long start, long end)
{
	MSXML2::IXMLHTTPRequestPtr httpRequest = NULL;
	httpRequest.CreateInstance(__uuidof(MSXML2::XMLHTTP60));

	for (long index = start; index <= end; index++)
	{
		CStation * station = _items[index];
		try
		{
			char url[255];
			memset(url, 0, sizeof(char) * 255);
			sprintf_s(url, "https://maps.googleapis.com/maps/api/geocode/json?latlng=%f,%f&key=AIzaSyDKbSM5ETagsfedoIx3y3ggGHyKZjLCB5E", (station->getLatitude()), (station->getLongitude()));
			cout << "Reverse Geocoding " << url << "..." << endl;
			httpRequest->open(_bstr_t(L"GET"), _bstr_t(url), VARIANT_FALSE);
			httpRequest->send();
			string json = (char *)httpRequest->GetresponseText();
			stringstream jsonStream(json);
			Json::Value root;   // will contains the root value after parsing.
			Json::Reader reader;
			bool parsingSuccessful = reader.parse(jsonStream, root, false);
			if (!parsingSuccessful)
			{
				// report to the user the failure and their locations in the document.
				cout  << reader.getFormatedErrorMessages() << endl;
			}
			else
			{
				Json::Value results = root["results"];
				Json::Value result = results[0];
				Json::Value addressComponents = result["address_components"];
				string streetNumber = "\0";
				string streetName = "\0";
				string city = "\0";
				string zip = "\0";
				string countryName = "\0";
				string countryCode = "\0";
				for (unsigned int ii = 0; ii < addressComponents.size(); ii++)
				{
					Json::Value addressComponent = addressComponents[ii];
					Json::Value longName = addressComponent["long_name"];
					cout << "long_name: " << longName.asCString() << endl;
					Json::Value shortName = addressComponent["short_name"];
					cout << "short_name: " << shortName.asCString() << endl;
					Json::Value types = addressComponent["types"];
					for (unsigned int jj = 0; jj < types.size(); jj++)
					{
						Json::Value type = types[jj];
						cout << "type: " << type.asCString() << endl;
						if (_stricmp("street_number", type.asCString()) == 0)
						{
							streetNumber = longName.asCString();
							break;
						}
						else if (_stricmp("route", type.asCString()) == 0)
						{
							streetName = longName.asCString();
							break;
						}
						else if (_stricmp("locality", type.asCString()) == 0)
						{
							city = longName.asCString();
							break;
						}
						else if (_stricmp("postal_code", type.asCString()) == 0)
						{
							zip = longName.asCString();
							break;
						}
						else if (_stricmp("country", type.asCString()) == 0)
						{
							countryName = longName.asCString();
							countryCode = shortName.asCString();
							break;
						}
					}
				}
				cout << "streetNumber: " << streetNumber << endl;
				cout << "streetName: " << streetName << endl;
				cout << "city: " << city << endl;
				cout << "zip: " << zip << endl;
				cout << "countryName: " << zip << endl;
				cout << "countryCode: " << zip << endl;
				station->setStreetNumber(streetNumber.c_str());
				station->setStreetName(streetName.c_str());
				station->setCity(city.c_str());
				station->setZip(zip.c_str());
				station->setCountryCode(countryCode.c_str());
				station->setCountryName(countryName.c_str());
			}
		}
		catch (...)
		{
			cout << "Exception Occurred!" << endl;
		}
	}
}

void CStations::save()
{
	for (vector<CStation *>::iterator item = _items.begin(); item != _items.end(); item++)
	{		
		_layer->SetFeature((*item)->getFeature());
	}
	_dataSource->SyncToDisk();
}

CStation * CStations::getItem(long index)
{
	return _items[index];
}

long CStations::getLength()
{
	return _items.size();
}
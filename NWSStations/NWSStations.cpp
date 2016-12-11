// NWSStations.cpp : Defines the entry point for the console application.
//

#include <Objbase.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wchar.h>
#include "Stations.h"

using namespace std;

#include "ogrsf_frmts.h"

int main(int argc, char** argv)
{
	OGRSFDriver * shapeFileDriver = NULL;
	OGRDataSource * output = NULL;
	OGRFeature * feature = NULL;
	CStations * stations = NULL;
	try
	{
		CoInitialize(NULL);

		string inputFileName = "C:\\Data\\NWS\\stations.xml";
		string outputFileName = "C:\\Data\\NWS\\stations.shp";

		for (int ii = 0; ii < argc; ) {
			char* arg = argv[ii++];
			if (_stricmp(arg, "--inputFileName") == 0) {
				if (ii < argc) {
					inputFileName = argv[ii++];
				}
			} else if (_stricmp(arg, "--outputFileName") == 0) {
				if (ii < argc) {
					outputFileName = argv[ii++];
				}
			}
		}

		stations = new CStations();
		stations->load(inputFileName);
		stations->checkForRadisondes();

		OGRRegisterAll();
		
		OGRSFDriverRegistrar * registrar = OGRSFDriverRegistrar::GetRegistrar();
		int driverCount = registrar->GetDriverCount();
		cout << "driverCount: " << driverCount << endl;

		for (int ii = 0; ii < driverCount; ii++)
		{
			OGRSFDriver * driver = registrar->GetDriver(ii);
			cout << "driver: " << driver->GetName() << endl;
			if (_stricmp(driver->GetName(), "ESRI Shapefile") == 0)
			{
				shapeFileDriver = driver;
				break;
			}
		}

		output = shapeFileDriver->CreateDataSource(outputFileName.c_str());
		OGRLayer * layer = output->CreateLayer("stations", NULL, wkbPoint, NULL);
		OGRFieldDefn stationIdField("station_id", OFTString);
		stationIdField.SetWidth(55);
		if (layer->CreateField(&stationIdField) != OGRERR_NONE)
		{
			throw exception("Error creating station_id field.");
		}
		OGRFieldDefn stationNameField("station_name", OFTString);
		stationNameField.SetWidth(55);
		if (layer->CreateField(&stationNameField) != OGRERR_NONE)
		{
			throw exception("Error creating station_name field.");
		}
		OGRFieldDefn stateField("state", OFTString);
		stateField.SetWidth(5);
		if (layer->CreateField(&stateField) != OGRERR_NONE)
		{
			throw exception("Error creating state field.");
		}
		OGRFieldDefn latitudeField("latitude", OFTReal);
		if (layer->CreateField(&latitudeField) != OGRERR_NONE)
		{
			throw exception("Error creating latitude field.");
		}
		OGRFieldDefn longitudeField("longitude", OFTReal);
		if (layer->CreateField(&longitudeField) != OGRERR_NONE)
		{
			throw exception("Error creating longitude field.");
		}
		OGRFieldDefn hasRadisondeField("has_rad", OFTString);
		if (layer->CreateField(&hasRadisondeField) != OGRERR_NONE)
		{
			throw exception("Error creating has_rad field.");
		}
		for (long ii = 0; ii < stations->getLength(); ii++)
		{
			CStation * station = stations->getItem(ii);
			OGRFeatureDefn * featureDefn = layer->GetLayerDefn();
			OGRPoint point(station->getLongitude(), station->getLatitude());
			feature = OGRFeature::CreateFeature(featureDefn);
			feature->SetField("station_id", station->getStationId().c_str());
			feature->SetField("station_name", station->getStationName().c_str());
			feature->SetField("state", station->getState().c_str());
			feature->SetField("latitude", station->getLatitude());
			feature->SetField("longitude", station->getLongitude());
			feature->SetField("has_rad", (station->hasRadisonde() ? "Y" : "N"));
			feature->SetGeometry(&point);
			layer->CreateFeature(feature);
			OGRFeature::DestroyFeature(feature);
			feature = NULL;
		}
		output->SyncToDisk();
	}
	catch (exception& ex)
	{
		cout << "Exception: " << ex.what() << endl;
	}
	catch (...)
	{
		cout << "Unknown Exception!" << endl;
	}
	if (feature)
	{
		OGRFeature::DestroyFeature(feature);
	}
	if (output)
	{
		output->Release();
	}
	if (stations)
	{
		stations->Release();
	}

	CoUninitialize();

	system("pause");

	_ASSERT(CStations::getInstances() == 0);
	_ASSERT(CStation::getInstances() == 0);

	return 0;
}


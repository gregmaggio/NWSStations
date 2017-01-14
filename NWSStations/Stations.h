#pragma once

#include "Station.h"
#include "UnknownImpl.h"
#include <vector>
#include <map>
#include "ogrsf_frmts.h"

using namespace std;

class CStations : public CUnknownImpl
{
private:
	static unsigned long _instances;
	vector<CStation *> _items;
	map<string, long> _indexByStationId;
	OGRDataSource * _dataSource;
	OGRLayer * _layer;
	OGRFeatureDefn * _featureDefn;
	
	bool fileExists(string& fileName);
	OGRSFDriver * getShapeFileDriver();
	OGRDataSource * createDataSource(string& fileName);
	OGRDataSource * openDataSource(string& fileName);
	void loadFromShapeFile(string& fileName);
public:
	CStations(string& shapeFileName);
	virtual ~CStations(void);

	static unsigned long getInstances();
	OGRDataSource * getDataSource();
	OGRLayer * getLayer();
	OGRFeatureDefn * getFeatureDefn();
	bool add(CStation * station);
	CStation * find(string& stationId);
	void checkForRadisondes();
	void checkForRadisondes(long start, long end);
	void reverseGeocodeAddress();
	void reverseGeocodeAddress(long start, long end);
	void save();
	CStation * getItem(long index);
	long getLength();
};


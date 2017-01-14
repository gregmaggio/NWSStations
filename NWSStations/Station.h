#pragma once

#include <string>
#include "UnknownImpl.h"
#include "ogrsf_frmts.h"

using namespace std;

class CStation : public CUnknownImpl
{
private:
	static unsigned long _instances;
	OGRFeature * _feature;

public:

	CStation(OGRFeature * feature) :
		_feature(feature)
	{
		InterlockedIncrement(&_instances);
	}

	virtual ~CStation(void)
	{
		_feature = NULL;
		InterlockedDecrement(&_instances);
	}

	static unsigned long getInstances()
	{
		return _instances;
	}

	const char * getStationId()
	{
		return _feature->GetFieldAsString("station_id");
	}

	const char * getStationName()
	{
		return _feature->GetFieldAsString("name");
	}

	const char * getStreetNumber()
	{
		return _feature->GetFieldAsString("street_no");
	}
	
	const char * getStreetName()
	{
		return _feature->GetFieldAsString("street");
	}

	const char * getCity()
	{
		return _feature->GetFieldAsString("city");
	}

	const char * getState()
	{
		return _feature->GetFieldAsString("state");
	}
	
	const char * getZip()
	{
		return _feature->GetFieldAsString("zip");
	}

	const char * getCountryCode()
	{
		return _feature->GetFieldAsString("country_cd");
	}

	const char * getCountryName()
	{
		return _feature->GetFieldAsString("country");
	}

	double getLatitude()
	{
		return _feature->GetFieldAsDouble("latitude");
	}

	double getLongitude()
	{
		return _feature->GetFieldAsDouble("longitude");
	}

	bool hasRadisonde()
	{
		return (_stricmp(_feature->GetFieldAsString("has_rad"), "Y") == 0) ? true : false;
	}

	OGRFeature * getFeature()
	{
		return _feature;
	}

	void setStationId(const char * newVal)
	{
		_feature->SetField("station_id", newVal);
	}

	void setStationName(const char * newVal)
	{
		_feature->SetField("name", newVal);
	}

	void setStreetNumber(const char * newVal)
	{
		_feature->SetField("street_no", newVal);
	}
	
	void setStreetName(const char * newVal)
	{
		_feature->SetField("street", newVal);
	}

	void setCity(const char * newVal)
	{
		_feature->SetField("city", newVal);
	}

	void setState(const char * newVal)
	{
		_feature->SetField("state", newVal);
	}
	
	void setZip(const char * newVal)
	{
		_feature->SetField("zip", newVal);
	}

	void setCountryCode(const char * newVal)
	{
		_feature->SetField("country_cd", newVal);
	}

	void setCountryName(const char * newVal)
	{
		_feature->SetField("country", newVal);
	}

	void setLatitude(double newVal)
	{
		_feature->SetField("latitude", newVal);
	}

	void setLongitude(double newVal)
	{
		_feature->SetField("longitude", newVal);
	}

	void setHasRadisonde(bool newVal)
	{
		_feature->SetField("has_rad", (newVal ? "Y" : "N"));
	}
};


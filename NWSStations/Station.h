#pragma once

#include <string>
#include "UnknownImpl.h"

using namespace std;

class CStation : public CUnknownImpl
{
private:
	static unsigned long _instances;
	string _stationId;
	string _stationName;
	string _state;
	double _latitude;
	double _longitude;
	bool _hasRadiosonde;

public:

	CStation(void) :
		_stationId("\0"),
		_stationName("\0"),
		_state("\0"),
		_latitude(0),
		_longitude(0),
		_hasRadiosonde(false)
	{
		InterlockedIncrement(&_instances);
	}

	virtual ~CStation(void)
	{
		_stationId = "\0";
		_stationName = "\0";
		_state = "\0";
		_latitude = 0;
		_longitude = 0;
		_hasRadiosonde = false;
		InterlockedDecrement(&_instances);
	}

	static unsigned long getInstances()
	{
		return _instances;
	}

	string& getStationId()
	{
		return _stationId;
	}

	string& getStationName()
	{
		return _stationName;
	}

	string& getState()
	{
		return _state;
	}
	
	double getLatitude()
	{
		return _latitude;
	}

	double getLongitude()
	{
		return _longitude;
	}

	bool hasRadisonde()
	{
		return _hasRadiosonde;
	}

	void setStationId(string& newVal)
	{
		_stationId = newVal;
	}

	void setStationName(string& newVal)
	{
		_stationName = newVal;
	}

	void setState(string& newVal)
	{
		_state = newVal;
	}
	
	void setLatitude(double newVal)
	{
		_latitude = newVal;
	}

	void setLongitude(double newVal)
	{
		_longitude = newVal;
	}

	void setHasRadisonde(bool newVal)
	{
		_hasRadiosonde = newVal;
	}
};


#pragma once

#include "Station.h"
#include "UnknownImpl.h"
#include <vector>

using namespace std;

#import <msxml6.dll>

class CStations : public CUnknownImpl
{
private:
	static unsigned long _instances;
	vector<CStation *> _items;

public:
	CStations(void);
	virtual ~CStations(void);

	static unsigned long getInstances();
	void load(string& fileName);
	void checkForRadisondes();
	CStation * getItem(long index);
	long getLength();
};


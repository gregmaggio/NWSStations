#pragma once

#include "State.h"
#include <vector>
#include <map>

using namespace std;

class CStates : public CUnknownImpl
{
private:
	static unsigned long _instances;
	vector<CState *> _items;
	map<string, long> _indexByStateCode;

	bool add(CState * state);
public:
	CStates(string& stateFileName);
	~CStates(void);

	static unsigned long getInstances()
	{
		return _instances;
	}

	CState * find(string& code);
	CState * getItem(long index);
	long getLength();
};


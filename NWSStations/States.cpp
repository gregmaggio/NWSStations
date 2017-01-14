#include "States.h"
#include <fstream>
#include <string>
#include "Utils.h"

unsigned long CStates::_instances = 0;

CStates::CStates(string& stateFileName)
{
	InterlockedIncrement(&_instances);

	ifstream file(stateFileName.c_str());
	string currentLine; 
	while (getline(file, currentLine))
	{
		// Process str
		string code = currentLine.substr(0,  2);
		string name = currentLine.substr(3);
		trim(code);
		trim(name);
		CState * state = new CState(code.c_str(), name.c_str());
		if (!add(state))
		{
			state->Release();
		}
	}
}


CStates::~CStates(void)
{
	for (vector<CState *>::iterator iter = _items.begin(); iter != _items.end(); iter++)
	{
		(*iter)->Release();
	}
	_items.clear();
	_indexByStateCode.clear();
	InterlockedDecrement(&_instances);
}

bool CStates::add(CState * state)
{
	string code = state->getCode();
	if (_indexByStateCode.find(code) != _indexByStateCode.end())
	{
		return false;
	}
	_items.push_back(state);
	long index = _items.size() - 1;
	_indexByStateCode.insert(pair<string, long>(code, index));
	return true;
}

CState * CStates::find(string& code)
{
	map<string, long>::iterator iter = _indexByStateCode.find(code);
	if (iter != _indexByStateCode.end())
	{
		return _items[(*iter).second];
	}
	return NULL;
}

CState * CStates::getItem(long index)
{
	return _items[index];
}

long CStates::getLength()
{
	return _items.size();
}


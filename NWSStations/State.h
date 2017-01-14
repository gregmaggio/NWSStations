#pragma once

#include <string>
#include "UnknownImpl.h"

using namespace std;

class CState : public CUnknownImpl
{
private:
	static unsigned long _instances;
	string _code;
	string _name;

public:
	CState(const char * code, const char * name)
		: _code(code), _name(name)
	{
		InterlockedIncrement(&_instances);
	}

	virtual ~CState(void)
	{
		_code = "\0";
		_name = "\0";
		InterlockedDecrement(&_instances);
	}

	static unsigned long getInstances()
	{
		return _instances;
	}

	const char * getCode()
	{
		return _code.c_str();
	}

	const char * getName()
	{
		return _name.c_str();
	}
};


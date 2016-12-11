#pragma once

#include <Unknwn.h>

class CUnknownImpl : public IUnknown
{
private:
	unsigned long _refCount;

public:

	CUnknownImpl(void) :
		_refCount(1)
	{
	}

	virtual ~CUnknownImpl(void)
	{
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( /* [in] */ REFIID riid, /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == __uuidof(IUnknown))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return InterlockedIncrement(&_refCount);
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)
	{
		InterlockedDecrement(&_refCount);
		if (_refCount == 0)
		{
			delete this;
			return 0;
		}
		return _refCount;
	}

};


#pragma once

class C_RegKey
{
public:
	C_RegKey(void) : m_hKey(NULL) { };
	C_RegKey(C_RegKey & key) : m_hKey(NULL) { Attach(key.Detach()); };
//	explicit C_RegKey(HKEY hKey) : m_hKey(hKey) { };
	~C_RegKey(void) { Close(); };

	C_RegKey& operator=(C_RegKey & key)
	{
		Close();
		Attach(key.Detach());

		return *this;
	};

	operator HKEY() const { return m_hKey; };

// Operations
public:
	LONG Create(HKEY hKeyParent, const wchar_t * wszKeyName,
					wchar_t * wszClass = REG_NONE, DWORD dwOptions = REG_OPTION_NON_VOLATILE,
					REGSAM samDesired = KEY_READ | KEY_WRITE,
					SECURITY_ATTRIBUTES * pSecAttr = NULL,
					DWORD * pdwDisposition = NULL) 
	{
		DWORD dw = 0;
		HKEY hKey = NULL;
		LONG res = ::RegCreateKeyExW(hKeyParent, wszKeyName, 0,
										wszClass, dwOptions, samDesired, pSecAttr, &hKey, &dw);
		if (pdwDisposition != NULL) *pdwDisposition = dw;
		if (res == ERROR_SUCCESS) { res = Close(); m_hKey = hKey; };
		return res;
	};

	LONG Open(HKEY hKeyParent, const wchar_t * wszKeyName, REGSAM samDesired = KEY_READ | KEY_WRITE) 
	{
		HKEY hKey = NULL;
		LONG res = ::RegOpenKeyExW(hKeyParent, wszKeyName, 0, samDesired, &hKey);
		if (res == ERROR_SUCCESS) { res = Close(); m_hKey = hKey; };
		return res;
	};

	LONG Close()
	{
		LONG res = (m_hKey != NULL) ? ::RegCloseKey(m_hKey) : ERROR_SUCCESS;
		m_hKey = NULL;
		return res;
	};
	
	void Attach(HKEY hKey) { m_hKey = hKey; };
	HKEY Detach() { HKEY hKey = m_hKey; m_hKey = NULL; return hKey;	};
	LONG Flush() { return ::RegFlushKey(m_hKey); };

public:
	LONG SetDWORD(const wchar_t * wszValueName, DWORD dwValue) 
	{
		return ::RegSetValueExW(m_hKey, wszValueName, NULL, REG_DWORD, 
						reinterpret_cast<const BYTE *>(&dwValue), sizeof(DWORD));
	};

	LONG SetString(const wchar_t * wszValueName, const wchar_t * wszValue, DWORD dwType = REG_SZ)
	{
		return ::RegSetValueEx(m_hKey, wszValueName, NULL, dwType, 
			reinterpret_cast<const BYTE *>(wszValue), (wcslen(wszValue)+1)*sizeof(wchar_t));
	};

	LONG SetString(const wchar_t * wszValueName, const std::wstring & wsValue, DWORD dwType = REG_SZ)
	{
		return SetString(wszValueName, wsValue.c_str(), dwType);
	};

	LONG QueryDWORD(const wchar_t * wszValueName, DWORD& dwValue)
	{
		DWORD dwType = 0;
		ULONG nBytes = sizeof(DWORD);
		LONG res = ::RegQueryValueExW(m_hKey, wszValueName, NULL, &dwType, 
											reinterpret_cast<BYTE *>(&dwValue), &nBytes);
		if ((res == ERROR_SUCCESS) && (dwType != REG_DWORD)) res = ERROR_INVALID_DATA;
		return res;
	};

	LONG QueryBool(const wchar_t * wszValueName, bool& fValue)
	{
		DWORD dw = 0;
		LONG res = QueryDWORD(wszValueName, dw);
		if (res == ERROR_SUCCESS) fValue = (dw != 0);
		return res;
	};

	LONG QueryString(const wchar_t * wszValueName, wchar_t * wszValue, ULONG* pnChars)
	{
		DWORD dwType = 0;
		ULONG nBytes = (*pnChars) * sizeof(wchar_t);
		*pnChars = 0;
		LONG res = ::RegQueryValueExW(m_hKey, wszValueName, NULL, &dwType, 
											reinterpret_cast<BYTE *>(wszValue), &nBytes);
		if ((res == ERROR_SUCCESS) && (dwType != REG_SZ)) res = ERROR_INVALID_DATA;
		if (res == ERROR_SUCCESS) *pnChars = nBytes/sizeof(wchar_t);
		return res;
	};

	LONG QueryString(const wchar_t * wszValueName, std::wstring & wsValue)
	{
		DWORD dwType = 0;
		ULONG nBytes = 0;
		LONG res = ::RegQueryValueExW(m_hKey, wszValueName, NULL, &dwType, NULL, &nBytes);
		if ((res == ERROR_SUCCESS) && (dwType != REG_SZ)) res = ERROR_INVALID_DATA;
		if (res == ERROR_SUCCESS) 
		{
			void * p = calloc(nBytes, 1);
			if (!p) res = ERROR_OUTOFMEMORY;
			if (res == ERROR_SUCCESS) 
			{
				res = ::RegQueryValueExW(m_hKey, wszValueName, NULL, &dwType, 
										reinterpret_cast<BYTE *>(p), &nBytes);
				if (res == ERROR_SUCCESS) wsValue = (wchar_t *)p;
				free(p);
			}
		}
		return res;
	};

protected:
	HKEY m_hKey;
};

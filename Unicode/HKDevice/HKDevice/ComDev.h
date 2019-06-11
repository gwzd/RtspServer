// ComDev.h : header file
//

#if !defined(_COMDEV_)
#define _COMDEV_

//////////////////////////////////////////////////////////////////////////
//宏定义
#define		COMBUFFER_LEN				1024			//串口接收数据缓冲区大小
#define		MAX_DATA_NUM				32				//最多保存的数据数
#define		MAX_QUERYDATA				100				//最长查询数据长度

/////////////////////////////////////////////////////////////////////////////
// CComDevice

class CComDevice
{
protected:
	BYTE	LPC( BYTE* pbyBuf,WORD nLen );
	void	NeatenRxdBuff();
	virtual void SeartchFrame(){};
	BYTE	CRC16Hi( BYTE * puchMsg, short usDataLen);
	BYTE	CRC16Lo( BYTE * puchMsg, short usDataLen);

public:
	CComDevice();
	virtual ~CComDevice();

	bool	Init(HWND hMainWnd, BYTE byCommNo, DWORD dwBaud, BYTE byDataBit = 8,
				BYTE byParity = NOPARITY, BYTE byStopbit = ONESTOPBIT);
	void	Deinit();

	void	AddDataToComBuffer(BYTE* pBuf, int nLen);

	//获取缓冲区内的数据（一般批量读取后放到缓冲区内），nIndex从1开始
	BOOL GetIntValue(int nIndex, int& nVal)
	{
		if(nIndex > 0 &&  nIndex <= m_nDataChanCount_Int && nIndex <= MAX_DATA_NUM)
		{
			nVal = m_nDataBufferInt[nIndex-1];
			return TRUE;
		}
		return FALSE;
	}

	BOOL GetFloatValue(int nIndex, float& fVal)
	{
		if(nIndex > 0 &&  nIndex <= m_nDataChanCount_Float && nIndex <= MAX_DATA_NUM)
		{
			fVal = m_fDataBufferFloat[nIndex-1];
			return TRUE;
		}
		return FALSE;
	}

	int GetDataChanCount_Int()
	{
		return m_nDataChanCount_Int;
	}
	int GetDataChanCount_Float()
	{
		return m_nDataChanCount_Float;
	}

	void SetTimeout(DWORD dwTimeout)
	{
		m_dwTimeout = dwTimeout;
	}
	DWORD GetTimeout()
	{
		return m_dwTimeout;
	}

	//获取查询数据
	virtual BOOL GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen){return FALSE;};

	//通过串口读取数据（nChanNo = 0：批量读取，1～n：读取单个数据）
	virtual BOOL ReadIntData(int nChanNo, int& nVal, CString& strAllVal){return FALSE;};
	virtual BOOL ReadFloatData(int nChanNo, float& fVal, CString& strAllVal){return FALSE;};

protected:
	int		m_nWritePointer;					//串口接收数据缓冲区写指针(0到COMBUFFER_LEN-1)
	int		m_nReadPointer;						//串口接收数据缓冲区读指针(0到COMBUFFER_LEN-1)
	BYTE	m_szComBuffer[COMBUFFER_LEN];		//串口接收数据缓冲区

	BYTE	m_byMinFrameLen;					//最短命令长度
	int		m_nDataChanCount_Int;				//设备整数值通道数
	int		m_nDataChanCount_Float;				//设备浮点值通道数

	HANDLE	m_hEventGetDataOK;					//是否获得正确的数据

	int		m_nDataBufferInt[MAX_DATA_NUM];
	float	m_fDataBufferFloat[MAX_DATA_NUM];

	DWORD	m_dwTimeout;						//读取超时，单位为ms
};

/////////////////////////////////////////////////////////////////////////////
// CComDeviceWS302
class CComDeviceWS302 : public CComDevice
{
protected:
	float m_fWendu;
	float m_fShidu;

	virtual void SeartchFrame();

public:
	CComDeviceWS302();
	virtual ~CComDeviceWS302();

	//获取查询帧数据
	virtual BOOL GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen);

	//通过串口读取数据（nChanNo = 0：批量读取，1～n：读取单个数据）
	virtual BOOL ReadIntData(int nChanNo, int& nVal, CString& strAllVal);
	virtual BOOL ReadFloatData(int nChanNo, float& fVal, CString& strAllVal);

	float GetWendu(){return m_fWendu;}
	float GetShidu(){return m_fShidu;}

protected:
	void ProcessData(BYTE nAddr, BYTE c5, BYTE c6, BYTE c7, BYTE c8, BYTE c9, BYTE c10);

};

/////////////////////////////////////////////////////////////////////////////
// CComDeviceJWSL
class CComDeviceJWSL : public CComDevice
{
protected:
	float m_fWendu;
	float m_fShidu;

	virtual void SeartchFrame();
	void ProcessData(BYTE* pData);

public:
	CComDeviceJWSL();
	virtual ~CComDeviceJWSL();

	//获取查询帧数据
	virtual BOOL GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen);

	//通过串口读取数据（nChanNo = 0：批量读取，1～n：读取单个数据）
	virtual BOOL ReadIntData(int nChanNo, int& nVal, CString& strAllVal);
	virtual BOOL ReadFloatData(int nChanNo, float& fVal, CString& strAllVal);

	float GetWendu(){return m_fWendu;}
	float GetShidu(){return m_fShidu;}
};

/////////////////////////////////////////////////////////////////////////////
// CComDeviceLTM8000
class CComDeviceLTM8000 : public CComDevice
{
protected:
	float m_fWendu;
	float m_fShidu;

	virtual void SeartchFrame();

public:
	CComDeviceLTM8000();
	virtual ~CComDeviceLTM8000();

	//获取查询帧数据
	virtual BOOL GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen);
	
	//通过串口读取数据（nChanNo = 0：批量读取，1～n：读取单个数据）
	virtual BOOL ReadIntData(int nChanNo, int& nVal, CString& strAllVal);
	virtual BOOL ReadFloatData(int nChanNo, float& fVal, CString& strAllVal);

	float GetWendu(){return m_fWendu;}
	float GetShidu(){return m_fShidu;}

protected:
	BYTE HexStrToVal(BYTE by1, BYTE by2);
	BYTE CheckSum( BYTE* pbyBuf,WORD nLen );
	void ProcessData(BYTE* pData);
};

#endif

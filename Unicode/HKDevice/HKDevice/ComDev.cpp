// ComDev.cpp : implementation file
//

#include "stdafx.h"
#include "Plugins.h"
#include "ComDev.h"

/////////////////////////////////////////////////////////////////////////////
// CComDevice


//////////////////////////////////////////////////////////////////////////
//16位 CRC校验

BYTE auchCRCHi[]=
{// CRC值的高字节列表
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
		0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40
};

BYTE auchCRCLo[] = 
{// CRC值的低字节列表 
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
		0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
		0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
		0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
		0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
		0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
		0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
		0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
		0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
		0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
		0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
		0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
		0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
		0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
		0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
		0x40
};


CComDevice::CComDevice()
{
	m_nWritePointer = 0;
	m_nReadPointer = 0;
	m_hEventGetDataOK = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_dwTimeout = 500;
}

CComDevice::~CComDevice()
{
	if (m_hEventGetDataOK)
	{
		CloseHandle(m_hEventGetDataOK);
		m_hEventGetDataOK = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CComDevice message handlers

bool CComDevice::Init(HWND hMainWnd, BYTE byCommNo, DWORD dwBaud, BYTE byDataBit, BYTE byParity, BYTE byStopbit)
{
	return true;
}

void CComDevice::Deinit()
{
}

//向串口缓冲区内添加数据
void CComDevice::AddDataToComBuffer(BYTE* pBuf, int nLen)
{
	if (COMBUFFER_LEN - m_nWritePointer >= nLen)
	{
		memcpy(m_szComBuffer + m_nWritePointer, pBuf, nLen);
		m_nWritePointer += nLen;
	}
	else	//缓冲区已写满！
	{
		memcpy(m_szComBuffer + m_nWritePointer, pBuf, COMBUFFER_LEN - m_nWritePointer);
		m_nWritePointer = COMBUFFER_LEN - 1;
	}

	if( m_nWritePointer - m_nReadPointer >= m_byMinFrameLen )		//收到的字节数超过一定数( 最短数据帧长 )
	{
		//处理数据
		SeartchFrame();
	}
}

//整理接收缓冲区，判断是否越界等
void CComDevice::NeatenRxdBuff()
{
	int i,j;

	if( m_nReadPointer == 0 )
		return;
	
	//容错处理
	if( ( m_nReadPointer >= m_nWritePointer ) || 	//读指针>=写指针为出错
		( m_nWritePointer > COMBUFFER_LEN -1 ))	//写指针>缓冲区大小
	{
		m_nReadPointer = m_nWritePointer = 0;
		return;
    }

	//向前移动数据(将读指针到写指针的数据移到缓冲区头部)
	i = 0;
	j = m_nReadPointer;

	while( j < m_nWritePointer )
		m_szComBuffer[i++] = m_szComBuffer[j++];

	m_nReadPointer  = 0;	//重置读指针
	m_nWritePointer = i;	//重置写指针

	return;
}

//获得LPC校验码(累加和)
BYTE CComDevice::LPC( BYTE* pbyBuf,WORD nLen )
{
	BYTE byResult = 0;

	for( WORD i=0; i<nLen; i++ )
		byResult += pbyBuf[i];	//累加和
	return byResult;
}

//CRC校验：得到CRC的高字节
BYTE CComDevice::CRC16Hi( BYTE * puchMsg, short usDataLen)
{
	BYTE uchCRCHi=0xFF;
	BYTE uchCRCLo=0xFF;
	BYTE uIndex;
	
	while ( usDataLen-- )
	{
		uIndex = uchCRCHi ^ * puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return ( uchCRCHi );
}

//CRC校验：得到CRC的低字节
BYTE CComDevice::CRC16Lo( BYTE * puchMsg, short usDataLen)
{
	BYTE uchCRCHi=0xFF;
	BYTE uchCRCLo=0xFF;
	BYTE uIndex;
	
	while ( usDataLen-- )
	{
		uIndex = uchCRCHi ^ * puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return ( uchCRCLo );
}


//////////////////////////////////////////////////////////////////////
// CComDeviceWS302 message handlers
CComDeviceWS302::CComDeviceWS302()
{
	m_byMinFrameLen = 13;
	m_fWendu = 0.0f;
	m_fShidu = 0.0f;

	m_nDataChanCount_Int = 0;
	m_nDataChanCount_Float = 2;
}

CComDeviceWS302::~CComDeviceWS302()
{

}

//从缓冲区中查找帧(对WS302协议)
void CComDeviceWS302::SeartchFrame()
{

LOOP:	NeatenRxdBuff();

//	TRACE("---------pRead=%d,pWrite=%d\n",m_nReadPointer,m_nWritePointer);

	int nTmpPtr = m_nReadPointer;
	int nWriPtr = m_nWritePointer;

	//是否小于最小帧长(13个字节)?
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )	return;

	while( m_szComBuffer[nTmpPtr] != 0x55)
	{
		nTmpPtr++;
		if( nTmpPtr >= nWriPtr )
		{
			//查完了所有数据
			m_nReadPointer = nTmpPtr;
			return;
		}
	}

	//再次检查数据长度是否小于最小帧长?
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )
	{
		m_nReadPointer = nTmpPtr;
		return;
	}

	int nDataLen = m_szComBuffer[nTmpPtr + 4];			//帧中的第5个字节是数据长度
	int nFrameLen = m_byMinFrameLen;
	//第三次确认帧长
	if ( (nWriPtr - nTmpPtr) < nFrameLen )
	{
		m_nReadPointer = nTmpPtr;
		return;
	}

	//确认帧尾及校验位
	if (m_szComBuffer[nTmpPtr + nFrameLen - 1] == 0xAA &&
		m_szComBuffer[nTmpPtr + nFrameLen - 2] == LPC(m_szComBuffer + nTmpPtr + 1, nFrameLen - 3))
	{
		//得到了正确的帧!
		TRACE(" ---- WS302: Received a OK frame!\n");

		ProcessData(m_szComBuffer[nTmpPtr+1], m_szComBuffer[nTmpPtr+5], m_szComBuffer[nTmpPtr+6], m_szComBuffer[nTmpPtr+7],
			m_szComBuffer[nTmpPtr+8],m_szComBuffer[nTmpPtr+9],m_szComBuffer[nTmpPtr+10]);

		//数据处理结束
		SetEvent(m_hEventGetDataOK);
	}
	else	//校验失败，丢掉第一个数据，返回
	{
		nFrameLen = 1;
	}


	//重置读指针
	m_nReadPointer = nTmpPtr + nFrameLen;

	//继续处理数据
	goto LOOP;

}

void CComDeviceWS302::ProcessData(BYTE nAddr, BYTE c5, BYTE c6, BYTE c7, BYTE c8, BYTE c9, BYTE c10)
{
	BYTE mask;
	mask = 0x80;
	float fVal = 0;
	if(c5 & mask) //温度负数
	{
		WORD val;
		val = MAKEWORD(c6,c5);
		val = ~val+1;
		fVal = -(val /10.0f);
	}
	else               //温度正数
	{
		WORD val;
		val = MAKEWORD(c6,c5);
		fVal = val /10.0f;
	}
	m_fWendu = fVal;

	mask = 0x80;
	if(c7 & mask) //湿度负数
	{
		WORD val;
		val = MAKEWORD(c8,c7);
		val = ~val+1;
		fVal = -(val /10.0f);
	}
	else              //湿度正数
	{
		WORD val;
		val = MAKEWORD(c8,c7);
		fVal = val /10.0f;
	}
	m_fShidu = fVal;

//	g_WS302[nIndex].byMemFault = (c9 & 0x20) ? 1 : 0;		//存储器故障
//	g_WS302[nIndex].byShiduFault = (c9 & 0x40) ? 1 : 0;		//湿度测量故障
//	g_WS302[nIndex].byWenduFault = (c9 & 0x80) ? 1 : 0;		//温度测量故障
}

//获取查询帧数据
BOOL CComDeviceWS302::GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen)
{
	if (buff == NULL || nBuffLen < 8)	// WS302查询帧长度为8
	{
		return FALSE;
	}

	buff[0] = 0x55;
	buff[1] = (BYTE)nAddr;
	buff[2] = 0x00;
	buff[3] = 0x01;
	buff[4] = 0x01;
	buff[5] = 0x00;
	buff[6] = 0x00;
	for (int i=1 ; i<6 ; i++)
		buff[6] += buff[i];
	buff[6]  = buff[6] & 0xFF;
	buff[7]	= 0xAA;

	nBuffLen = 8;

	return TRUE;
}

BOOL CComDeviceWS302::ReadIntData(int nChanNo, int& nVal, CString& strAllVal)
{
	return FALSE;
}

BOOL CComDeviceWS302::ReadFloatData(int nChanNo, float& fVal, CString& strAllVal)
{
	if (WaitForSingleObject(m_hEventGetDataOK, m_dwTimeout) == WAIT_TIMEOUT)
	{
		//sprintf(g_szLastError, "IDVR_GetIOValue: 超时!");
		return FALSE;
	}

	//获取成功！
	if (nChanNo == 1)
		fVal = m_fWendu;
	else if(nChanNo == 2)
		fVal = m_fShidu;
	else if(nChanNo == 0)
		strAllVal.Format(_T("COUNT=2;VAL=%.2f,%.2f,;"), m_fWendu, m_fShidu);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CComDeviceJWSL message handlers
CComDeviceJWSL::CComDeviceJWSL()
{
	m_byMinFrameLen = 9;
	m_nDataChanCount_Int = 0;
	m_nDataChanCount_Float = 2;
	m_fWendu = 0.0f;
	m_fShidu = 0.0f;
}

CComDeviceJWSL::~CComDeviceJWSL()
{
}

//从缓冲区中查找帧
void CComDeviceJWSL::SeartchFrame()
{
	
LOOP:	NeatenRxdBuff();
		
	//	TRACE("---------pRead=%d,pWrite=%d\n",m_nReadPointer,m_nWritePointer);
	int nTmpPtr = m_nReadPointer;
	int nWriPtr = m_nWritePointer;

	//是否小于最小帧长?
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )	return;

	while( m_szComBuffer[nTmpPtr+1] != 0x03 &&
		   m_szComBuffer[nTmpPtr+2] != 0x04)
	{
		nTmpPtr++;
		if( nTmpPtr >= nWriPtr )
		{
			//查完了所有数据
			m_nReadPointer = nTmpPtr;
			return;
		}
	}

	int nFrameLen = 0;

	//再次确认帧长
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )
		return;

	//校验
	if (m_szComBuffer[nTmpPtr+7] == CRC16Hi(m_szComBuffer+nTmpPtr, 7) &&		//CRC高字节
		m_szComBuffer[nTmpPtr+8] == CRC16Lo(m_szComBuffer+nTmpPtr, 7))			//CRC低字节
	{
		nFrameLen = m_byMinFrameLen;
		TRACE(" ---- JWSL: Received a OK frame!\n");

		ProcessData(m_szComBuffer+nTmpPtr);

		//数据处理结束
		SetEvent(m_hEventGetDataOK);
	}
	else	//校验失败，丢掉第一个数据，返回
	{
		nFrameLen = 1;
	}

	//重置读指针
	m_nReadPointer = nTmpPtr + nFrameLen;
	
	//继续处理数据
	goto LOOP;
}

BOOL CComDeviceJWSL::ReadIntData(int nChanNo, int& nVal, CString& strAllVal)
{
	//此设备无整数值可提供
	return FALSE;
}

BOOL CComDeviceJWSL::ReadFloatData(int nChanNo, float& fVal, CString& strAllVal)
{
	if (WaitForSingleObject(m_hEventGetDataOK, m_dwTimeout) == WAIT_TIMEOUT)
	{
		//sprintf(g_szLastError, "IDVR_GetIOValue: 超时!");
		return FALSE;
	}

	//获取成功！
	if (nChanNo == 1)
		fVal = m_fWendu;
	else if(nChanNo == 2)
		fVal = m_fShidu;
	else if(nChanNo == 0)
		strAllVal.Format(_T("COUNT=2;VAL=%.2f,%.2f,;"), m_fWendu, m_fShidu);

	return TRUE;
}

//获取查询帧数据
BOOL CComDeviceJWSL::GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen)
{
	if (buff == NULL || nBuffLen < 8)	// JWSL查询帧长度为8
	{
		return FALSE;
	}

	buff[0] = (BYTE)nAddr;
	buff[1] = 0x03;
	buff[2] = 0x00;
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x02;

	buff[6] = CRC16Hi(buff, 6);
	buff[7] = CRC16Lo(buff, 6);

 	nBuffLen = 8;

	return TRUE;
}

void CComDeviceJWSL::ProcessData(BYTE* pData)
{
	if (pData == NULL)
		return;
	
	BYTE mask;
	mask = 0x80;
	float fVal = 0;
	if((pData[3] & 0x80) != 0)	//温度负数
	{
		WORD val;
		mask = 0x7f;
		val = 0xFFFF - MAKEWORD(pData[4], pData[3]);
		fVal = -(val /10.0f);
	}
	else						 //温度正数
	{
		WORD val;
		val = MAKEWORD(pData[4], pData[3]);
		fVal = val /10.0f;
	}
	m_fWendu = fVal;
	
	WORD val;
	val = MAKEWORD(pData[6], pData[5]);
	m_fShidu = val /10.0f;
}

//////////////////////////////////////////////////////////////////////
// CComDeviceLTM8000 message handlers
CComDeviceLTM8000::CComDeviceLTM8000()
{
	m_byMinFrameLen = 23;
	m_fWendu = 0.0f;
	m_fShidu = 0.0f;

	m_nDataChanCount_Int = 0;
	m_nDataChanCount_Float = 2;
}

CComDeviceLTM8000::~CComDeviceLTM8000()
{

}


BYTE CComDeviceLTM8000::HexStrToVal(BYTE by1, BYTE by2)
{
	BYTE low=0, high=0;
	if (by1 >= 0x30 && by1 <= 0x39)		//0～9
	{
		high = by1-48;
	}
	else if(by1 >= 0x41 && by1 <= 0x46)	//大写A～B
	{
		high = by1-55;
	}
	else if(by1 >= 0x61 && by1 <= 0x66)	//小写a～b
	{
		high = by1-87;
	}
	
	if (by2 >= 0x30 && by2 <= 0x39)		//0～9
	{
		low = by2-48;
	}
	else if(by2 >= 0x41 && by2 <= 0x46)	//大写A～B
	{
		low = by2-55;
	}
	else if(by2 >= 0x61 && by2 <= 0x66)	//小写a～b
	{
		low = by2-87;
	}
	
	return (high*16 + low);
}


//获得校验码(累加和，取反，+1)
BYTE CComDeviceLTM8000::CheckSum( BYTE* pbyBuf,WORD nLen )
{
	ASSERT(nLen == 18);
	BYTE temp=0, last=0;
	for (int i=0; i<9; i++)
	{
		temp = HexStrToVal(pbyBuf[2*i], pbyBuf[2*i+1]);
		last += temp;
	}
	last = ~last;
	last += 1;
	return last;
}

//从缓冲区中查找帧(对LTM8000协议)
void CComDeviceLTM8000::SeartchFrame()
{

LOOP:	NeatenRxdBuff();

//	TRACE("---------pRead=%d,pWrite=%d\n",m_nReadPointer,m_nWritePointer);

	int nTmpPtr = m_nReadPointer;
	int nWriPtr = m_nWritePointer;

	//是否小于最小帧长?
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )	return;

	while( m_szComBuffer[nTmpPtr] != 0x3A ||
		m_szComBuffer[21] != 0x0D ||
		m_szComBuffer[22] != 0x0A)
	{
		nTmpPtr++;
		if( nTmpPtr >= nWriPtr )
		{
			//查完了所有数据
			m_nReadPointer = nTmpPtr;
			return;
		}
	}

	int nFrameLen = 0;
	
	//再次确认帧长
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )
		return;

	//校验
	if (HexStrToVal(m_szComBuffer[nTmpPtr+19], m_szComBuffer[nTmpPtr+20]) == CheckSum(m_szComBuffer+nTmpPtr+1, 18))
	{
		nFrameLen = m_byMinFrameLen;
		TRACE(" ---- LTM8000: Received a OK frame!\n");

		ProcessData(m_szComBuffer+nTmpPtr);

		//数据处理结束
		SetEvent(m_hEventGetDataOK);
	}
	else	//校验失败，丢掉第一个数据，返回
	{
		nFrameLen = 1;
	}

	//重置读指针
	m_nReadPointer = nTmpPtr + nFrameLen;

	//继续处理数据
	goto LOOP;

}

void CComDeviceLTM8000::ProcessData(BYTE* pData)
{
	if (pData == NULL)
		return;

	//获取温湿度值
	BYTE TH = HexStrToVal(pData[11], pData[12]);
	BYTE TL = HexStrToVal(pData[13], pData[14]);
	if ((TH & 0x08) == 0)
	{
		m_fWendu = ((TH & 0x07)*256+TL)*0.0625f;		//正温度
	}
	else
	{
		m_fWendu = -((TH & 0x07)*256+TL)*0.0625f;		//负温度
	}
	
	BYTE HH = HexStrToVal(pData[15], pData[16]);
	BYTE HL = HexStrToVal(pData[17], pData[18]);
	m_fShidu = (HH * 256 + HL)/ 10.0f;
}

//获取查询帧数据
BOOL CComDeviceLTM8000::GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen)
{
	if (buff == NULL || nBuffLen < 17)	// 查询帧长度为17
	{
		return FALSE;
	}

	//帧头
	buff[0] = 0x3A;	//分号(:)

	char s[10];
	sprintf(s, "%02X", nAddr);
	buff[1] = s[0];
	buff[2] = s[1];

	buff[3] = 0x30;
	buff[4] = 0x34;
	buff[5] = 0x30;
	buff[6] = 0x30;
	buff[7] = 0x38;
	buff[8] = 0x30;
	buff[9] = 0x30;
	buff[10] = 0x30;
	buff[11] = 0x30;
	buff[12] = 0x33;
	
	//0x87 为 byData[3] ～ byData[12] 的ASCII码之累加和（两个字节算一个16进制数）
	BYTE CheckSum = ~(0x87 + nAddr) + 1;
	sprintf(s, "%02X", CheckSum);
	buff[13] = s[0];
	buff[14] = s[1];

	//帧尾
	buff[15] = 0x0D;
	buff[16] = 0x0A;

	nBuffLen = 17;

	return TRUE;
}

BOOL CComDeviceLTM8000::ReadIntData(int nChanNo, int& nVal, CString& strAllVal)
{
	return FALSE;
}

BOOL CComDeviceLTM8000::ReadFloatData(int nChanNo, float& fVal, CString& strAllVal)
{
	if (WaitForSingleObject(m_hEventGetDataOK, m_dwTimeout) == WAIT_TIMEOUT)
	{
		//sprintf(g_szLastError, "IDVR_GetIOValue: 超时!");
		return FALSE;
	}

	//获取成功！
	if (nChanNo == 1)
		fVal = m_fWendu;
	else if(nChanNo == 2)
		fVal = m_fShidu;
	else if(nChanNo == 0)
		strAllVal.Format(_T("COUNT=2;VAL=%.2f,%.2f,;"), m_fWendu, m_fShidu);

	return TRUE;
}

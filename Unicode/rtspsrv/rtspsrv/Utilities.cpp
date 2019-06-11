#include"stdafx.h"
#include "Utilities.h"
//CLastError* CLastError::m_pInstance =NULL;
#include "request.hpp"
namespace RTSP {
	namespace RTSPSrv {
		std::string headers_to_string(const std::vector<header>& headers)
		{	
			std::stringstream ss;
			for (std::size_t i = 0; i < headers.size(); i++)
			{
				const header& h = headers[i];
				ss <<h.name<<": "<<h.value<<std::endl;				
			}		 
			ss.flush();
			return ss.str();
		}

		//字符串格式化函数
		std::string format(const char *fmt,...) 
		{ 
			std::string strResult="";
			if (NULL != fmt)
			{
				va_list marker = NULL;            
				va_start(marker, fmt);                            //初始化变量参数 
				size_t nLength = _vscprintf(fmt, marker) + 1;    //获取格式化字符串长度
				std::vector<char> vBuffer(nLength, '\0');        //创建用于存储格式化字符串的字符数组
				int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker);
				if (nWritten>0)
				{
					strResult = &vBuffer[0];
				}            
				va_end(marker);                                    //重置变量参数
			}
			return strResult; 
		}

		//字符串格式化函数
		std::wstring format(const wchar_t *fmt,...) 
		{ 
			std::wstring strResult=L"";
			if (NULL != fmt)
			{
				va_list marker = NULL;            
				va_start(marker, fmt);                            //初始化变量参数
				size_t nLength = _vscwprintf(fmt, marker) + 1;    //获取格式化字符串长度
				std::vector<wchar_t> vBuffer(nLength, L'\0');    //创建用于存储格式化字符串的字符数组
				int nWritten = _vsnwprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker); 
				if (nWritten > 0)
				{
					strResult = &vBuffer[0];
				}
				va_end(marker);                                    //重置变量参数
			}
			return strResult; 
		} 
	}
}
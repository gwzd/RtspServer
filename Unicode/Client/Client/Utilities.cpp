#include"stdafx.h"
#include "Utilities.h"
#include<fstream>
#include<locale>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

CLastError* CLastError::m_pInstance = CLastError::GetInstance();

/*
static char * __cdecl strtok_mt(
	const char * string,
	char ** nextoken,
	const char * control,
	char *buffer
	)
{
	unsigned char *str;
	const unsigned char *ctrl = (const unsigned char *)control;

	unsigned char map[32];
	int count;


	// Clear control map
	for (count = 0; count < 32; count++)
		map[count] = 0;



	// Set bits in delimiter table 
	do {
		map[*ctrl >> 3] |= (1 << (*ctrl & 7));
	} while (*ctrl++);

	// Initialize str. If string is NULL, set str to the saved
	// pointer (i.e., continue breaking tokens out of the string
	// from the last strtok call)
	if (string)
		str = (unsigned char*)string;
	else
		str = (unsigned char*)*nextoken;


	// Find beginning of token (skip over leading delimiters). Note that
	// there is no token iff this loop sets str to point to the terminal
	// null (*str == '\0') 
	while ((map[*str >> 3] & (1 << (*str & 7))) && *str)
		str++;

	string = (char*)str;
	strcpy(buffer, string);

	// Find the end of the token. If it is not the end of the string,
	// put a null there. 
	for (; *str; str++)
	{
		if (map[*str >> 3] & (1 << (*str & 7))) {
			int len = (char*)(str)-string;
			strncpy(buffer, string, len);
			buffer[len] = 0;
			*str++;
			break;
		}
	}
	// Update nextoken (or the corresponding field in the per-thread data
	// structure
	*nextoken = (char*)str;


	// Determine if a token has been found.
	if (string == (char*)str)
		return NULL;
	else
		return buffer;
}
*/


// int SplitString(const char *szSource,const char* szDelimiter,CStringArray &arrString)
// {	
// 	char * delimiter = (char*)szDelimiter;
// 	if(szDelimiter==NULL)
// 		delimiter = ";" ;
// 	int count =0;
// 	arrString.RemoveAll();
// 	if(szSource!=NULL)
// 	{
// 		char *token;
// 		char *nextoken;
// 		char buffer[1024];		
// 		token = (char*)strtok_mt(szSource,&nextoken,delimiter,buffer);
// 		if(token!=NULL)
// 		{		    
// 			arrString.Add(token);
// 			count++;
// 			while( token != NULL)
// 			{		  
// 				/* While there are tokens in "string" */		
// 				/* Get next token: */
// 				//printf("Token,%s ",token);				
// 				token = (char*)strtok_mt( NULL, &nextoken,delimiter,buffer);
// 				if(token!=NULL)
// 				{					
// 					arrString.Add(token);				
// 					count++;
// 				}
// 				else
// 					break;
// 			
// 			}	 
// 		}		
// 	}
// 	return count;
// }

//int _vstprintf(TCHAR *buffer, const TCHAR *format, va_list argptr);
void CLastError::LogError(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[512];

	nBuf = wsprintf(szBuffer, lpszFormat, args);

	// was there an error? was the expanded string too long?
	//	ASSERT(nBuf >= 0);

	LogErrorMsg(szBuffer);
	va_end(args);
}

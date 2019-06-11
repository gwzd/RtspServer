#include "StdAfx.h"
#include <jrtplib/rtpsession.h>
#include <jrtplib/rtppacket.h>
#include <jrtplib/rtpudpv4transmitter.h>
#include <jrtplib/rtpipv4address.h>
#include <jrtplib/rtpsessionparams.h>
#include <jrtplib/rtperrors.h>
#include <jrtplib/rtpsourcedata.h>
#ifndef WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif // WIN32
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using   namespace   std;
using   namespace   jrtplib;

//#pragma comment(lib,"jrtplib.lib")
//#pragma comment(lib,"jthread.lib")
#pragma comment(lib,"WS2_32.lib")

long datalen = 0;

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}

//
// The new class routine
//

class MyRTPSession : public RTPSession
{
protected:
	void OnNewSource(RTPSourceData *dat)
	{
		if (dat->IsOwnSSRC())
			return;

		uint32_t ip;
		uint16_t port;

		if (dat->GetRTPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else if (dat->GetRTCPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort() - 1;
		}
		else
			return;

		RTPIPv4Address dest(ip, port);
		AddDestination(dest);

		//	struct in_addr inaddr;
		//	inaddr.s_addr = htonl(ip);		

		//	std::cout << "Adding destination " << std::string(inet_ntoa(inaddr)) << ":" << port << std::endl;
	}

	void OnBYEPacket(RTPSourceData *dat)
	{
		if (dat->IsOwnSSRC())
			return;

		uint32_t ip;
		uint16_t port;

		if (dat->GetRTPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else if (dat->GetRTCPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort() - 1;
		}
		else
			return;

		RTPIPv4Address dest(ip, port);
		DeleteDestination(dest);

	}

	void OnRemoveSource(RTPSourceData *dat)
	{
		if (dat->IsOwnSSRC())
			return;
		if (dat->ReceivedBYE())
			return;

		uint32_t ip;
		uint16_t port;



		if (dat->GetRTPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else if (dat->GetRTCPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort() - 1;
		}
		else
			return;

		RTPIPv4Address dest(ip, port);
		DeleteDestination(dest);


	}
};


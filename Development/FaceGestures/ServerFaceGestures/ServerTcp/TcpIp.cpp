#include <string>
#include <stdio.h>
#include <iostream>
#include <string>

#include "winsock2.h"
#include "WS2tcpip.h"
#include "TcpIp.h"
#include "Distributor.h"

#define TCP_DEBUG_MODE

using namespace std;

char				msg[256];
CRITICAL_SECTION	CriticalSection;

/***************************************************************************/
/*  FUNCTION-NAME  :  dump                                                 */
/*  DESCRIPTION    :  dumps a buffer                                       */
/*  PARAMETERS     :  - buffer to be dumped                                */
/*                    - lenght of buffer                                   */
/*  RETURN-VALUE   :  - TRUE if OK.                                        */
/*                    - FALSE otherwise                                    */
/*  EXTERNAL REFERENCES : none                                             */
/*  MODIFIED EXTERNALS  : none                                             */
/***************************************************************************/
BOOL dump(  const char* mess, void *buf, unsigned long len )
{
#define DUMP_LINE_LEN 16
#define min(a,b)  ((a) < (b) ? (a) : (b) )

	unsigned short	line, incr;
    unsigned char*	p;
    unsigned short	i, c;
    unsigned char	left[DUMP_LINE_LEN*3+2], right[DUMP_LINE_LEN+1];
    char			sRes[100];

	if( !buf )
		return FALSE;

    for (p = (unsigned char*)buf, line = 0; len; p += incr, len -= incr, line++)
    {
        incr = (unsigned short)min( len, DUMP_LINE_LEN);
	    memset(right, 0, sizeof(right));
	
		for (i = 0; i < incr; i++ )
		{
            sprintf((char*)&left[i*3], "%02X ", c = (unsigned short)p[i]);
            right[i] = (unsigned char)(isprint(c) ? c:'.');
        }
        
		if (mess)
            sprintf(sRes,"%s:%04X: %-48s [%-16s]\n",mess,line*DUMP_LINE_LEN, left, right);
        else
            sprintf(sRes,"%04X: %-48s [%-16s]\n", line*DUMP_LINE_LEN, left, right);
		
		printf(sRes);
    }

	return TRUE;
} // dump


//---------------------------------------------------------------------------
// streamServer class
//---------------------------------------------------------------------------
// ctor
streamServer::streamServer()
{
};

//---------------------------------------------------------------------------
// constructor
streamServer::streamServer(unsigned short port_a)
{    
    if (port_a == 0)
    {
        throw string("streamServer: port == 0");
        return;
    }

    m_port = port_a;   
    DWORD dwThreadId;    
	WSADATA wsaData;
    DWORD Flags = 0;
	m_image_data.buffer	=  NULL;
	m_image_data.len = 0;

    int Ret, ii = 0;
    DWORD Total = 0;
    int addrlen = sizeof(sockaddr_in);
	m_EventTotal = 0;

	InitializeCriticalSection(&CriticalSection);
	m_algo = new Distributor(this, Ret); 
	
    m_ListenSocket = INVALID_SOCKET;
    if ((Ret = WSAStartup(MAKEWORD(2,2),&wsaData)) != 0)
    {
        //printf("%s\n", GetErrorMsg("WSAStartup() failed with error"));
		WSACleanup();
		throw string("streamServer: WSAStartup(..) 1") + string(GetErrorMsg(": "));        
        return;
	}

	if ((m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		//printf("%s\n", GetErrorMsg("WSASocket() failed with error"));
		WSACleanup();
		throw string("streamServer: WSASocket(..)") + string(GetErrorMsg(": "));       
		return;
	}

    int yes=1;
    struct timeval timeout0;
    timeout0.tv_sec  = 5;
    setsockopt(m_ListenSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout0, sizeof(timeout0));

	m_svraddr.sin_family = AF_INET;
	m_svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_svraddr.sin_port = htons(m_port);

	if (bind(m_ListenSocket, (SOCKADDR *) &m_svraddr, sizeof(m_svraddr)) == SOCKET_ERROR)
	{
		//printf("%s\n", GetErrorMsg("Binding failed with error"));
		closesocket(m_ListenSocket);
		WSACleanup();
		throw string("streamServer: bind(..)") + string(GetErrorMsg(": "));   
		return;
	}

	if (listen(m_ListenSocket, 5))
	{
		//printf("listen() failed with error %d\n", WSAGetLastError());
		closesocket(m_ListenSocket);
		WSACleanup();
		throw string("streamServer: listen(..)") + string(GetErrorMsg(": "));   
		return;          
	}

	if ((m_AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		//printf("Failed to get a socket %d\n", WSAGetLastError());
		throw string("streamServer: WSASocket(..) 2") + string(GetErrorMsg(": "));  
		return;
	}

	if ((m_EventArray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		//printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
		throw string("streamServer: WSACreateEventt(..)") + string(GetErrorMsg(": "));  
		return;
	}
   
    m_hThread_a = CreateThread( NULL, 0, streamServer::exec_accept, this, CREATE_SUSPENDED, &dwThreadId);
    if (m_hThread_a == NULL)
    {		
		throw string("streamServer: CreateThread(..) a") + string(GetErrorMsg(": "));  
        return;
    }

	m_hThread_p = CreateThread( NULL, 0, streamServer::exec_process, this, CREATE_SUSPENDED, &dwThreadId);
    if (m_hThread_p == NULL)
    {		
		throw string("streamServer: CreateThread(..) p") + string(GetErrorMsg(": "));  
        return;
    }

	m_EventTotal = 1; 
};

//---------------------------------------------------------------------------
void streamServer::start()
{	
    m_stop = 0;
    ResumeThread(m_hThread_a);
	ResumeThread(m_hThread_p);
}

//---------------------------------------------------------------------------
void streamServer::stop()
{
	TerminateThread(m_hThread_p,1);
    TerminateThread(m_hThread_a,1);

	if (m_image_data.buffer)
		 GlobalFree(m_image_data.buffer);

	m_image_data.len = 0;
}

//---------------------------------------------------------------------------
// destructor
streamServer::~streamServer()
{
	TerminateThread(m_hThread_p,1);
    TerminateThread(m_hThread_a,1);
    m_terminated = true;
    ::Sleep(100);
    
	if (m_ListenSocket != INVALID_SOCKET)
    {
        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
    }
    if (m_AcceptSocket != INVALID_SOCKET)
    {
		closesocket(m_AcceptSocket);
        m_AcceptSocket = INVALID_SOCKET;            
    }

    WSACleanup();

	if (m_algo !=  NULL)
		delete m_algo;
    
	return;
};

//---------------------------------------------------------------------------
char* streamServer::GetErrorMsg(char* msg_l)
{
    sprintf(m_error, "%s:%d\n", msg_l,WSAGetLastError() );
    return m_error;
}

//---------------------------------------------------------------------------
int streamServer::sendErrorMsg(SOCKET socket, char* msg, int len)
{
	m_image_data.len = 0;

	if((send(socket, msg, len,0))==SOCKET_ERROR)						
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf("WSASend() failed with error %d\n", WSAGetLastError());
			return 1;
		}
	}
	else
		printf("WSASend()-ERR1 is OK!\n");

	return 0;
}

//---------------------------------------------------------------------
DWORD WINAPI streamServer::exec_accept( void* thread )
{
    int res;

    try
    {
      res = ((streamServer*)thread)->run_accept();
    }
    catch(...)
    {
		throw string("streamServer: exec_accept(..)---failer!"); 
    }

    return res;
}

//---------------------------------------------------------------------
DWORD WINAPI streamServer::exec_process( void* thread )
{
    int res;

    try
    {
      res = ((streamServer*)thread)->run_process();
    }
    catch(...)
    {
		throw string("streamServer: exec_process(..)---failer!"); 
    }

    return res;
}

//---------------------------------------------------------------------
// Main Thread function.
int streamServer::run_accept()
{
	DWORD Flags;
	DWORD RecvBytes;

	while(TRUE)
	{
		if ((m_AcceptSocket = accept(m_ListenSocket, NULL, NULL)) == INVALID_SOCKET)
		{
			//printf("accept() failed with error %d\n", WSAGetLastError());
			throw string("streamServer:accept()") + string(GetErrorMsg(": ")); 
			return 1;
		}
		else
			printf("accept() is OK!\n");

		EnterCriticalSection(&CriticalSection);    

		if ((m_SocketArray[m_EventTotal] = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
		{
			//printf("GlobalAlloc() failed with error %d\n", GetLastError());
			throw string("streamServer:GlobalAlloc()") + string(GetErrorMsg(": ")); 
			return 1;
		}     

		m_SocketArray[m_EventTotal]->Socket = m_AcceptSocket;
		ZeroMemory(&(m_SocketArray[m_EventTotal]->Overlapped), sizeof(OVERLAPPED));
		m_SocketArray[m_EventTotal]->BytesSEND = 0;
		m_SocketArray[m_EventTotal]->BytesRECV = 0;
		m_SocketArray[m_EventTotal]->DataBuf.len = DATA_BUFSIZE;
		m_SocketArray[m_EventTotal]->DataBuf.buf = m_SocketArray[m_EventTotal]->Buffer;

		if ((m_SocketArray[m_EventTotal]->Overlapped.hEvent = m_EventArray[m_EventTotal] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			//printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
			throw string("streamServer:WSACreateEvent()") + string(GetErrorMsg(": ")); 
			return 1;
		}
		else
			printf("WSACreateEvent() is OK!\n");

		Flags = 0;

		if (WSARecv(m_SocketArray[m_EventTotal]->Socket, &(m_SocketArray[m_EventTotal]->DataBuf), 1, &RecvBytes, &Flags, &(m_SocketArray[m_EventTotal]->Overlapped), NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				//printf("WSARecv() failed with error %d\n", WSAGetLastError());
				throw string("streamServer:WSARecv()") + string(GetErrorMsg(": ")); 
				return 1;
			}
		}
		else
			printf("WSARecv() should be working!\n");
      
		m_EventTotal++;
		// full_data_len = 0;
		LeaveCriticalSection(&CriticalSection);

		if (WSASetEvent(m_EventArray[0]) == FALSE)
		{
			//printf("WSASetEvent() failed with error %d\n", WSAGetLastError());
			throw string("streamServer:WSASetEvent() ") + string(GetErrorMsg(": ")); 
			return 1;
		}
		else
			printf("WSASetEvent() is OK!\n");
   }
}

int streamServer::run_process()
{
	DWORD Index;
	DWORD Flags;
	LPSOCKET_INFORMATION SI;
	DWORD BytesTransferred;
	int i;
	//DWORD RecvBytes, SendBytes;

	while(TRUE)
	{
		if ((Index = WSAWaitForMultipleEvents(m_EventTotal, m_EventArray, FALSE,  WSA_INFINITE, FALSE)) == WSA_WAIT_FAILED)
		{
			printf("WSAWaitForMultipleEvents() failed %d\n", WSAGetLastError());
			return 0;
		}
		//else
		//	printf("WSAWaitForMultipleEvents() is OK:%d!\n", Index);

		if ((Index - WSA_WAIT_EVENT_0) == 0)	
		{
			WSAResetEvent(m_EventArray[0]);
			continue;
		}

		SI = m_SocketArray[Index - WSA_WAIT_EVENT_0];
		SI->DataBuf.len = DATA_BUFSIZE;
		SI->DataBuf.buf = SI->Buffer;
		WSAResetEvent(m_EventArray[Index - WSA_WAIT_EVENT_0]);

		if (WSAGetOverlappedResult(SI->Socket, &(SI->Overlapped), &BytesTransferred, FALSE, &Flags) == FALSE || BytesTransferred == 0)
		{
			printf("Closing socket %d\n", SI->Socket);
         
			if (closesocket(SI->Socket) == SOCKET_ERROR)
			{
				printf("closesocket() failed with error %d\n", WSAGetLastError());
			}
			else
				printf("closesocket() is OK!\n");
         
			GlobalFree(SI);
			WSACloseEvent(m_EventArray[Index - WSA_WAIT_EVENT_0]);

			EnterCriticalSection(&CriticalSection);

			if ((Index - WSA_WAIT_EVENT_0) + 1 != m_EventTotal)
			{
				for (i = Index - WSA_WAIT_EVENT_0; i < m_EventTotal; i++)
				{
					m_EventArray[i] = m_EventArray[i + 1];
					m_SocketArray[i] = m_SocketArray[i + 1];
				}
			}
         
			m_EventTotal--;
			LeaveCriticalSection(&CriticalSection);
			continue;
		}
		
		//printf("BytesTransferred-len:%d,  0x%x\n",BytesTransferred,SI->Buffer[0]);
		SI->BytesRECV = BytesTransferred;
        Flags = 0;
        
		if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &BytesTransferred, &Flags, &(SI->Overlapped), NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
            {
               printf("WSARecv() failed with error %d\n", WSAGetLastError());
               continue;
            }
        }
//        else
//			printf("WSARecv() is OK!\n");		
		
		//printf("BytesTransferred-len:%d,  0x%x\n",BytesTransferred,SI->Buffer[0]);
		
		if (SI->BytesRECV)
		{
			//completed        
//			printf("WSARecv()-len:%d\n",SI->BytesRECV);		
			//dump("tcp_rec_header:",(void*)SI->Buffer,SI->BytesRECV);
			//printf("TCPIP_IMAGE_INFORMATION-len:%d, 0x%x\n",sizeof(TCPIP_IMAGE_INFORMATION),SI->Buffer[0]);
			if ((SI->BytesRECV == sizeof(TCPIP_IMAGE_INFORMATION)) && (SI->Buffer[0] == (char)0xaa))
			{ 
				// header
				//dump("tcp_rec_header:",(void*)SI->Buffer,SI->BytesRECV); // DEBUG

				//Image haeader adatait megkapta, ha jo akkor ACK1, rossz akkor ERR1 kuld!	
				m_image_data_information =   *((TCPIP_IMAGE_INFORMATION*)&(SI->Buffer[0]));
				//printf("size:%d - width:%d - hight:%d\n",m_image_data_information.size, m_image_data_information.width,m_image_data_information.height);
				
				if ( ((m_image_data.len != m_image_data_information.size) || (m_image_data.buffer == NULL) ))
				{
					if (m_image_data.buffer)
					{						
						GlobalFree(m_image_data.buffer);
						m_image_data.buffer = NULL;						
					}				
					if (m_image_data_information.size < MAX_IMAGE_SIZE)
					{
						m_image_data.buffer = (char*)GlobalAlloc(GPTR, m_image_data_information.size);
					}
					else
					{
						sendErrorMsg(SI->Socket, "ERR1", 5);
						continue;
					}
				}

				m_image_data.len = 0;			
				
				if((send(SI->Socket,"ACK1", 4,0))==SOCKET_ERROR)						
				{
					if (WSAGetLastError() != ERROR_IO_PENDING)
					{
						printf("WSASend() failed with error %d\n", WSAGetLastError());
						continue;
					}
				}
/*				else
					printf("WSASend()-ACK1 is OK!\n");	*/	

				continue;
			}
			else
			{ 
				// data
				if ((m_image_data.len+SI->BytesRECV) <= m_image_data_information.size)	
				{
					//printf("m_image_data.len-%d!\n",m_image_data.len);
					memmove(m_image_data.buffer + m_image_data.len, SI->DataBuf.buf , SI->BytesRECV);
					m_image_data.len += SI->BytesRECV;		

					if (m_image_data.len == m_image_data_information.size)				
					{
						// completed				
						// Algoritmus vegrehajto, ha jo akkor ACK2, rossz Akkor ERR2 kuld!	
						string result = m_algo->executor();

						if((send(SI->Socket, result.c_str()/*"ACK2"*/, result.length(),0))==SOCKET_ERROR)					
						{
							if (WSAGetLastError() != ERROR_IO_PENDING)
							{
								printf("WSASend() failed with error %d\n", WSAGetLastError());
								continue;						
							}
							//else
							//	printf("WSASend()-%s!\n",result.c_str());
						}
						//else
						//	printf("WSASend()-%s!\n",result.c_str());

						m_image_data.len = 0;						
					}
				}
				else				
				{
					m_image_data.len = 0;
					sendErrorMsg(SI->Socket, "ERR2", 5);
				}				
			}			
		}		       		        
   }
    
   return 0;
}




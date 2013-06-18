#ifndef TCPIP_H_
#define TCPIP_H_

#include <winsock2.h>
#include "WS2tcpip.h"
#include "macros.h"

#define MAX_IMAGE_SIZE	(1600*1600)
#define TCP_MAX_REC_LEN 131200
#define DATA_BUFSIZE TCP_MAX_REC_LEN

// image header: TODO elso vetel: 32 byte
typedef struct _TCPIP_IMAGE_INFORMATION
{
	int				control_ID;
	unsigned int	frame_count;
	int				depth;
	int				nChannels;
	int				width;
	int				height;
	int				pExpression;
	int				pTracking;
	long			size;
}	TCPIP_IMAGE_INFORMATION;

typedef struct _TCPIP_IMAGE
{
	char*			buffer;
	int				len;
}	TCPIP_IMAGE;

typedef struct _SOCKET_INFORMATION 
{
	CHAR			Buffer[DATA_BUFSIZE];
	WSABUF			DataBuf;
	SOCKET			Socket;
	WSAOVERLAPPED	Overlapped;
	DWORD			BytesSEND;
	DWORD			BytesRECV;
}	SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

typedef void (PARSER)(LPSOCKET_INFORMATION);

class Distributor;

class FACE_API streamServer
{
private:
	char			m_error[64];
    unsigned short  m_port;

    bool			m_terminated;
    HANDLE			m_hThread_a, m_hThread_p;
    HANDLE			m_hEvent;
    SOCKET			m_ListenSocket;
	SOCKET			m_AcceptSocket;
    SOCKADDR_IN		m_svraddr;
    WSADATA			m_wsaData;

    DWORD			m_SendBytes;
    DWORD			m_RecvBytes;   
    char			m_stop;
	DWORD			m_EventTotal;
	WSAEVENT		m_EventArray[WSA_MAXIMUM_WAIT_EVENTS];

private:
    static DWORD WINAPI		exec_accept		( void* );
    int						run_accept		( void );
    static DWORD WINAPI		exec_process	( void* );
    int						run_process		( void );
    char*					GetErrorMsg		( char* msg_l );
	int						sendErrorMsg	( SOCKET socket, char* msg, int len );

protected:

protected:
    // cprot
	streamServer( void );

public:
	LPSOCKET_INFORMATION	m_SocketArray[WSA_MAXIMUM_WAIT_EVENTS];
    bool					m_accept;
	TCPIP_IMAGE				m_image_data;
	TCPIP_IMAGE_INFORMATION m_image_data_information;
	Distributor*			m_algo;

public:
	streamServer	( unsigned short port_a );
	~streamServer	( void );

	virtual void	start	( void );
	void			stop	( void );
	void			Send	( void );
};

#endif

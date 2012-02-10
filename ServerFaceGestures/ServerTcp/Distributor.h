#ifndef DISTRIBUTOR_H_
#define DISTRIBUTOR_H_

#include "TcpIp.h"
#include "IFaceTracker.h"

using namespace std;

class streamServer;
class IFaceTracker;

class FACE_API Distributor
{
private:
	streamServer*	m_server;
	char			cTemp[64];
	IFaceTracker*	iface;

public:
	
private:
	Distributor		(void);

public:
	Distributor		(streamServer* server, int &res);
	~Distributor	(void);
	string executor	(void);
};
	
#endif

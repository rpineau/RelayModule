//
//  USB Relay module X2 plugin
//
//  Created by Rodolphe Pineau on 1/10/2026.


#ifndef __RELAY_MODULE_C__
#define __RELAY_MODULE_C__

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <thread>
#include <ctime>

#include "../../licensedinterfaces/sberrorx.h"
#include "../../licensedinterfaces/serxinterface.h"

#define PLUGIN_DEBUG 2

#define SERIAL_BUFFER_SIZE 1024
#define TEXT_BUFFER_SIZE    1024

#define MAX_TIMEOUT 500
#define MAX_READ_WAIT_TIMEOUT 25

typedef unsigned char byte;


enum Plugin_Errors    {PLUGIN_OK = 0, NOT_CONNECTED, ND_CANT_CONNECT, BAD_CMD_RESPONSE, COMMAND_TIMEOUT, COMMAND_FAILED};

#define PLUGIN_VERSION  1.0

#define NB_PORTS 4


class CRelayModule
{
public:
	CRelayModule();
	~CRelayModule();
	
	int         Connect(std::string sPort);
	void        Disconnect(void);
	bool        IsConnected(void) { return m_bIsConnected; };
	
	void        SetSerxPointer(SerXInterface *p) { m_pSerx = p; };
	
	int			getPortCount();
	
	// getter and setter
	int         getStatus(int &nStatus);
	int         getFirmwareVersion(std::string sVersion);
	
	int         setPortState(const int &nPortNumber, const bool &bEnabled);
	bool		getPortState(int nPort);
	
protected:
	
	int			portCommand(byte *cmd);
	int			readResponse(std::string &sResp, int nTimeout = MAX_TIMEOUT, char cEndOfResponse = 0x0a);
	
	SerXInterface   *m_pSerx;
	
	bool		m_bDebugLog = false;
	bool		m_bIsConnected = false;;
	char		m_szFirmwareVersion[TEXT_BUFFER_SIZE];
	
	bool port1On = false;
	bool port2On = false;
	bool port3On = false;
	bool port4On = false;
	
	
#ifdef PLUGIN_DEBUG
	std::string hexOut;
	void    hexdump(const byte *inputData, int inputSize,  std::string &outHex);
	// timestamp for logs
	const std::string getTimeStamp();
	std::ofstream m_sLogFile;
	std::string m_sLogfilePath;
#endif
	
};

#endif //__RELAY_MODULE_C__

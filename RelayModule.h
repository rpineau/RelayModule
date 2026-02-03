//
//
//  Created by Rodolphe Pineau on 3/11/2020.


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

    // getter and setter
    int         getStatus(int &nStatus);
    int         getFirmwareVersion(std::string sVersion);

	bool        getPortOn(const int &nPortNumber);
    int         setPortOn(const int &nPortNumber, const bool &bEnabled);
	bool		portState(int nPort);

protected:

	int			readResponse(byte *sResp, int size, int nTimeout = MAX_TIMEOUT);
	int			parseResp(const std::string sResp, std::vector<std::string> &svFields, char cSeparator);


	int		port1Toggle(bool bOn);
	int		port2Toggle(bool bOn);
	int		port3Toggle(bool bOn);
	int		port4Toggle(bool bOn);

	int		getPortStates();

	int		portCommand(byte *cmd);

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

#endif //__PEGASUS_C__

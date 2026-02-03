//
//  Pegasus pocket power box X2 plugin
//
//  Created by Rodolphe Pineau on 3/11/2020.


#include "RelayModule.h"


CRelayModule::CRelayModule()
{

    m_pSerx = NULL;

#ifdef PLUGIN_DEBUG
#if defined(WIN32)
	m_sLogfilePath = getenv("HOMEDRIVE");
	m_sLogfilePath += getenv("HOMEPATH");
	m_sLogfilePath += "\\RelayModole.txt";
#else
	m_sLogfilePath = getenv("HOME");
	m_sLogfilePath += "/RTI-Dome-Log.txt";
#endif
	m_sLogFile.open(m_sLogfilePath, std::ios::out |std::ios::trunc);
#endif


#if defined PLUGIN_DEBUG
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Version " << std::fixed << std::setprecision(2) << PLUGIN_VERSION << " build " << __DATE__ << " " << __TIME__ << std::endl;
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Constructor Called." << std::endl;
	m_sLogFile.flush();
#endif


}

CRelayModule::~CRelayModule()
{
#ifdef	PLUGIN_DEBUG
	// Close LogFile
	if(m_sLogFile.is_open())
		m_sLogFile.close();
#endif
}

int CRelayModule::Connect(std::string sPort)
{
    int nErr = PLUGIN_OK;

    if(!m_pSerx)
        return ERR_COMMNOLINK;

#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Called." << std::endl;
	m_sLogFile.flush();
#endif

    // 9600 8N1
    nErr = m_pSerx->open(sPort.c_str(), 9600, SerXInterface::B_NOPARITY, "-DTR_CONTROL 1");
    if(nErr == 0)
        m_bIsConnected = true;
    else
        m_bIsConnected = false;

    if(!m_bIsConnected)
        return nErr;

#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
		m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Connection failed, nErr = " << nErr <<  std::endl;
		m_sLogFile.flush();
#endif

    return nErr;
}

void CRelayModule::Disconnect()
{
    if(m_bIsConnected && m_pSerx)
        m_pSerx->close();
 
	m_bIsConnected = false;
}

#pragma mark getters and setters
int CRelayModule::getStatus(int &nStatus)
{
	int nErr = PLUGIN_OK;
	std::string sResp;

	if(!m_bIsConnected)
		return ERR_COMMNOLINK;

    // OK_ppb or OK_PPB
    if(nErr)
        return nErr;
    return nErr;
}



int CRelayModule::getFirmwareVersion(std::string sVersion)
{
    int nErr = PLUGIN_OK;
	std::string sResp;

	if(!m_bIsConnected)
		return ERR_COMMNOLINK;

    if(!m_bIsConnected)
        return NOT_CONNECTED;

    if(nErr)
        return nErr;

	sVersion.assign(sResp);
    return nErr;
}


bool CRelayModule::getPortOn(const int &nPortNumber)
{

	return false;
}

int CRelayModule::setPortOn(const int &nPortNumber, const bool &bEnabled)
{
    int nErr = PLUGIN_OK;

	switch(nPortNumber) {
		case 0:
			port1Toggle(bEnabled);
			break;
		case 1:
			port2Toggle(bEnabled);
			break;
		case 2:
			port3Toggle(bEnabled);
			break;
		case 3:
			port4Toggle(bEnabled);
			break;
		default:
			return ERR_CMDFAILED;
			break;
	}

    return nErr;
}


int CRelayModule::port1Toggle(bool bOn)
{
	int nErr = PLUGIN_OK;
	byte portOnCmd[4] = {0xA0, 0x01, 0x01, 0xA2};
	byte portOffCmd[4] = {0xA0, 0x01, 0x00, 0xA1};

	if(!m_bIsConnected)
		return ERR_COMMNOLINK;

	if(bOn) {
		portCommand(portOnCmd);
	}
	else {
		portCommand(portOffCmd);
	}

	return nErr;
}

int CRelayModule::port2Toggle(bool bOn)
{
	int nErr = PLUGIN_OK;
	byte portOnCmd[4] = {0xA0, 0x02, 0x01, 0xA3};
	byte portOffCmd[4] = {0xA0, 0x02, 0x00, 0xA2};

	if(!m_bIsConnected)
		return ERR_COMMNOLINK;

	if(bOn) {
		portCommand(portOnCmd);
	}
	else {
		portCommand(portOffCmd);
	}

	return nErr;
}

int CRelayModule::port3Toggle(bool bOn)
{
	int nErr = PLUGIN_OK;
	byte portOnCmd[4] = {0xA0, 0x03, 0x01, 0xA4};
	byte portOffCmd[4] = {0xA0, 0x03, 0x00, 0xA3};

	if(!m_bIsConnected)
		return ERR_COMMNOLINK;

	if(bOn) {
		portCommand(portOnCmd);
	}
	else {
		portCommand(portOffCmd);
	}

	return nErr;
}

int CRelayModule::port4Toggle(bool bOn)
{
	int nErr = PLUGIN_OK;
	byte portOnCmd[4] = {0xA0, 0x04, 0x01, 0xA5};
	byte portOffCmd[4] = {0xA0, 0x04, 0x00, 0xA};

	if(!m_bIsConnected)
		return ERR_COMMNOLINK;

	if(bOn) {
		portCommand(portOnCmd);
	}
	else {
		portCommand(portOffCmd);
	}
	return nErr;
}

int CRelayModule::getPortStates()
{
	int nErr = PLUGIN_OK;
	byte respBuffer[16];
	byte portStatesCmd[4] = {0xA0, 0x0F, 0x02, 0xB1};

	if(!m_bIsConnected)
		return ERR_COMMNOLINK;

	portCommand(portStatesCmd);
	readResponse(respBuffer, 16);
	return nErr;

}

bool CRelayModule::portState(int nPort)
{
	getPortStates();
	switch(nPort) {
		case 0:
			return port1On;
			break;
		case 1:
			return port2On;
			break;
		case 2:
			return port3On;
			break;
		case 3:
			return port4On;
			break;
		default:
			return false;
			break;
	}
	return false;
}


int CRelayModule::portCommand(byte *cmd)
{
	int nErr = PLUGIN_OK;
	int nBytesWaiting;
	unsigned long  ulBytesWrite;

	if(!m_bIsConnected)
		return ERR_COMMNOLINK;

	nErr = m_pSerx->bytesWaitingRx(nBytesWaiting);
	if(nBytesWaiting) {
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
		m_sLogFile << "["<<getTimeStamp()<<"]"<< " [nBytesWaiting] calling purgeTxR"<< std::endl;
		m_sLogFile.flush();
#endif
		m_pSerx->purgeTxRx();
	}

	nErr = m_pSerx->writeFile((void *)cmd, 4, ulBytesWrite);
	m_pSerx->flushTx();

	if(nErr){
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
		m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] writeFile error : " << nErr << std::endl;
		m_sLogFile.flush();
#endif
		return nErr;
	}

	return nErr;

}



#pragma mark command and response functions


int CRelayModule::readResponse(byte *sResp, int size, int nTimeout )
{
	int nErr = PLUGIN_OK;
	byte pszBuf[SERIAL_BUFFER_SIZE];
	unsigned long ulBytesRead = 0;
	unsigned long ulTotalBytesRead = 0;
	byte *pszBufPtr;
	int nBytesWaiting = 0 ;
	int nbTimeouts = 0;

	memset(pszBuf, 0, SERIAL_BUFFER_SIZE);
	pszBufPtr = pszBuf;

	do {
		nErr = m_pSerx->bytesWaitingRx(nBytesWaiting);
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 3
		m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] nBytesWaiting = " << nBytesWaiting << std::endl;
		m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] nBytesWaiting nErr = " << nErr << std::endl;
		m_sLogFile.flush();
#endif
		if(!nBytesWaiting) {
			nbTimeouts += MAX_READ_WAIT_TIMEOUT;
			if(nbTimeouts >= nTimeout) {
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 3
				m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] bytesWaitingRx timeout, no data for" << nbTimeouts <<" ms" << std::endl;
				m_sLogFile.flush();
#endif
				nErr = COMMAND_TIMEOUT;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(MAX_READ_WAIT_TIMEOUT));
			continue;
		}
		nbTimeouts = 0;
		if(ulTotalBytesRead + nBytesWaiting <= SERIAL_BUFFER_SIZE)
			nErr = m_pSerx->readFile(pszBufPtr, nBytesWaiting, ulBytesRead, nTimeout);
		else {
			nErr = ERR_RXTIMEOUT;
			break; // buffer is full.. there is a problem !!
		}
		if(nErr) {
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
			m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] readFile error." << std::endl;
			m_sLogFile.flush();
#endif
			return nErr;
		}

		if (ulBytesRead != nBytesWaiting) { // timeout
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
			m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] readFile Timeout Error." << std::endl;
			m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] readFile nBytesWaiting = " << nBytesWaiting << std::endl;
			m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] readFile ulBytesRead =" << ulBytesRead << std::endl;
			m_sLogFile.flush();
#endif
		}

		ulTotalBytesRead += ulBytesRead;
		pszBufPtr+=ulBytesRead;
	} while (ulTotalBytesRead < size);


#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	hexdump(pszBuf,  size, hexOut);
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [haltFocuser] sending : " << std::endl << hexOut << std::endl;

	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] pszBuf = '" << pszBuf << "'" << std::endl;
	m_sLogFile.flush();
#endif


	if(!ulTotalBytesRead)
		nErr = COMMAND_TIMEOUT; // we didn't get an answer.. so timeout
	else
		memcpy(sResp,pszBuf, size);
	return nErr;
}


int CRelayModule::parseResp(const std::string sResp, std::vector<std::string> &svFields, char cSeparator)
{
	int nErr = PLUGIN_OK;
	std::string sSegment;

#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] sResp = " << sResp << std::endl;
	m_sLogFile.flush();
#endif

	if(sResp.size()==0) {
		return ERR_CMDFAILED;
	}

	std::stringstream ssTmp(sResp);

	svFields.clear();
	// split the string into vector elements
	while(std::getline(ssTmp, sSegment, cSeparator))
	{
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 3
		m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] sSegment = " << sSegment << std::endl;
		m_sLogFile.flush();
#endif
		svFields.push_back(sSegment);
	}

	if(svFields.size()==0) {
		nErr = ERR_CMDFAILED;
	}
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Done all good." << std::endl;
	m_sLogFile.flush();
#endif

	return nErr;
}


#ifdef PLUGIN_DEBUG
void  CRelayModule::hexdump(const byte *inputData, int inputSize,  std::string &outHex)
{
	int idx=0;
	std::stringstream ssTmp;

	outHex.clear();
	for(idx=0; idx<inputSize; idx++){
		if((idx%16) == 0 && idx>0)
			ssTmp << std::endl;
		ssTmp << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)inputData[idx] <<" ";
	}
	outHex.assign(ssTmp.str());
}
const std::string CRelayModule::getTimeStamp()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}
#endif

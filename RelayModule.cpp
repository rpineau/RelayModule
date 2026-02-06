//
//  USB Relay module X2 plugin
//
//  Created by Rodolphe Pineau on 1/10/2026.


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
	m_sLogfilePath += "/RelayModole.txt";
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
	
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Connecting to " << sPort << std::endl;
	m_sLogFile.flush();
#endif
	
	// 9600 8N1
	nErr = m_pSerx->open(sPort.c_str(), 9600, SerXInterface::B_NOPARITY, "-DTR_CONTROL 1");
	if(nErr == 0)
		m_bIsConnected = true;
	else
		m_bIsConnected = false;
	
	if(!m_bIsConnected) {
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
		m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Connection failed, nErr = " << nErr <<  std::endl;
		m_sLogFile.flush();
#endif
		return nErr;
	}
	
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Connected to " << sPort << std::endl;
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
int CRelayModule::getPortCount()
{
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Called." << std::endl;
	m_sLogFile.flush();
#endif
	return NB_PORTS;
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


int CRelayModule::setPortState(const int &nPortNumber, const bool &bEnabled)
{
	int nErr = PLUGIN_OK;
	byte portStateCmd[4] = {0xA0, 0x00, 0x00, 0x00};
	
	if(!m_bIsConnected)
		return ERR_COMMNOLINK;
	
	
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Called." << std::endl;
	m_sLogFile.flush();
#endif
	
	//set port
	portStateCmd[1] = byte(nPortNumber+1);
	// set state
	portStateCmd[2] = byte(bEnabled?1:0);
	
	// checksum
	portStateCmd[3] = portStateCmd[0] + portStateCmd[1] + portStateCmd[2];
	
	nErr = portCommand(portStateCmd);
	
	return nErr;
}


bool CRelayModule::getPortState(int nPort)
{
	bool bState = false;
	int nErr = PLUGIN_OK;
	std::string sResp;
	byte portStatesCmd[4] = {0xA0, 0x01, 0x02, 0xA3};
	
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Called." << std::endl;
	m_sLogFile.flush();
#endif
	
	if(!m_bIsConnected)
		return ERR_COMMNOLINK;
	
	//set port
	portStatesCmd[1] = byte(nPort+1);
	// checksum
	portStatesCmd[3] = portStatesCmd[0] + portStatesCmd[1] + portStatesCmd[2];
	
	portCommand(portStatesCmd);
	nErr = readResponse(sResp);
	if(sResp.find(":ON") != std::string::npos)
		bState = true;
	else if(sResp.find(":OFF") != std::string::npos)
		bState = false;
	
	return bState;
}

int CRelayModule::portCommand(byte *cmd)
{
	int nErr = PLUGIN_OK;
	int nBytesWaiting;
	unsigned long  ulBytesWrite;
	
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] Called." << std::endl;
	m_sLogFile.flush();
#endif
	
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


int CRelayModule::readResponse(std::string &sResp, int nTimeout, char cEndOfResponse)
{
	int nErr = PLUGIN_OK;
	char pszBuf[SERIAL_BUFFER_SIZE];
	unsigned long ulBytesRead = 0;
	unsigned long ulTotalBytesRead = 0;
	char *pszBufPtr;
	int nBytesWaiting = 0 ;
	int nbTimeouts = 0;
	
	sResp.clear();
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
	} while (ulTotalBytesRead < SERIAL_BUFFER_SIZE  && *(pszBufPtr-1) != cEndOfResponse);
	
	
#if defined PLUGIN_DEBUG && PLUGIN_DEBUG >= 2
	m_sLogFile << "["<<getTimeStamp()<<"]"<< " [" << __func__ << "] pszBuf = '" << pszBuf << "'" << std::endl;
	m_sLogFile.flush();
#endif
	
	
	if(!ulTotalBytesRead)
		nErr = COMMAND_TIMEOUT; // we didn't get an answer.. so timeout
	else
		*(pszBufPtr-1) = 0; //remove the cEndOfResponse
	
	sResp.assign(pszBuf);
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

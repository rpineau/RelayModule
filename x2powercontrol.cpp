#include "x2powercontrol.h"

X2PowerControl::X2PowerControl(const char* pszDisplayName,
										const int& nInstanceIndex,
										SerXInterface						* pSerXIn,
										TheSkyXFacadeForDriversInterface	* pTheSkyXIn,
										SleeperInterface					* pSleeperIn,
										BasicIniUtilInterface				* pIniUtilIn,
										LoggerInterface						* pLoggerIn,
										MutexInterface						* pIOMutexIn,
										TickCountInterface					* pTickCountIn):m_bLinked(0)
{
    char portName[255];
	std::string sLabel;
    int i;

	m_pTheSkyXForMounts = pTheSkyXIn;
	m_pSleeper = pSleeperIn;
	m_pIniUtil = pIniUtilIn;
	m_pIOMutex = pIOMutexIn;
	m_pTickCount = pTickCountIn;

	m_nISIndex = nInstanceIndex;
    
    if (m_pIniUtil) {
		// load port names
		for(i=0; i<NB_PORTS; i++) {
            sLabel = "Fusion port " + std::to_string(i+1);
            m_pIniUtil->readString(PARENT_KEY, m_IniPortKey[i].c_str(), sLabel.c_str(), portName, 255);
            m_sPortNames.push_back(std::string(portName));
		}
	}
}

X2PowerControl::~X2PowerControl()
{
	//Delete objects used through composition
	if (GetTheSkyXFacadeForDrivers())
		delete GetTheSkyXFacadeForDrivers();
	if (GetSleeper())
		delete GetSleeper();
	if (GetSimpleIniUtil())
		delete GetSimpleIniUtil();
	if (GetMutex())
		delete GetMutex();
}

int X2PowerControl::establishLink(void)
{
	int nErr = SB_OK;

	m_PowerPorts.Connect("");

    m_bLinked = true;

	return nErr;
}

int X2PowerControl::terminateLink(void)
{
	m_bLinked = false;

    return SB_OK;
}

bool X2PowerControl::isLinked() const
{
	return m_bLinked;
}


void X2PowerControl::deviceInfoNameShort(BasicStringInterface& str) const
{
	str = "Pegasus Astro PPB";
}
void X2PowerControl::deviceInfoNameLong(BasicStringInterface& str) const
{
	str = "Pegasus Astro PPB";
}
void X2PowerControl::deviceInfoDetailedDescription(BasicStringInterface& str) const
{
	str = "Pegasus Astro PPB power port controll";
}
void X2PowerControl::deviceInfoFirmwareVersion(BasicStringInterface& str)
{
	str = "None";
}
void X2PowerControl::deviceInfoModel(BasicStringInterface& str)
{
	str = "Pegasus Astro PPB";
}

void X2PowerControl::driverInfoDetailedInfo(BasicStringInterface& str) const
{
	str = "Pegasus Astro PPB X2 plugin by Rodolphe Pineau";
}

double X2PowerControl::driverInfoVersion(void) const
{
	return PLUGIN_VERSION;
}

int X2PowerControl::queryAbstraction(const char* pszName, void** ppVal)
{
	*ppVal = NULL;

	if (!strcmp(pszName, SerialPortParams2Interface_Name))
		*ppVal = dynamic_cast<SerialPortParams2Interface*>(this);
	else if (!strcmp(pszName, CircuitLabelsInterface_Name))
        *ppVal = dynamic_cast<CircuitLabelsInterface*>(this);
    else if (!strcmp(pszName, SetCircuitLabelsInterface_Name))
        *ppVal = dynamic_cast<SetCircuitLabelsInterface*>(this);

	return 0;
}

#pragma mark - UI binding


int X2PowerControl::numberOfCircuits(int& nNumber)
{
	// nNumber = m_PowerPorts.getPortCount();
	return 0;
}

int X2PowerControl::circuitState(const int& nIndex, bool& bZeroForOffOneForOn)
{
	int nErr = SB_OK;

	if(!m_bLinked)
        return ERR_NOLINK;
/*
	if (nIndex >= 0 && nIndex<m_PowerPorts.getPortCount())
        bZeroForOffOneForOn = m_PowerPorts.getPortStatus(nIndex+1, bZeroForOffOneForOn);
	else
		nErr = ERR_INDEX_OUT_OF_RANGE;
*/
	return nErr;
}

int X2PowerControl::setCircuitState(const int& nIndex, const bool& bZeroForOffOneForOn)
{
	int nErr = SB_OK;

	if(!m_bLinked)
        return ERR_NOLINK;
/*
	if (nIndex >= 0 && nIndex < m_PowerPorts.getPortCount())
        nErr = m_PowerPorts.setPort(nIndex+1, bZeroForOffOneForOn);
	else
		nErr = ERR_INDEX_OUT_OF_RANGE;
*/
	return nErr;
}

int X2PowerControl::circuitLabel(const int &nZeroBasedIndex, BasicStringInterface &str)
{
    int nErr = SB_OK;
    if(m_sPortNames.size() >= nZeroBasedIndex+1) {
        str = m_sPortNames[nZeroBasedIndex].c_str();
    }
    else {
        std::string sLabel = "Fusion port " + std::to_string(nZeroBasedIndex+1);
        str = sLabel.c_str();
    }

    return nErr;
}

int X2PowerControl::setCircuitLabel(const int &nZeroBasedIndex, const char *str)
{
    int nErr = SB_OK;

    if(m_sPortNames.size() >= nZeroBasedIndex+1) {
        m_sPortNames[nZeroBasedIndex] = str;
        m_pIniUtil->writeString(PARENT_KEY, m_IniPortKey[nZeroBasedIndex].c_str(), str);
    }
    else {
        nErr = ERR_CMDFAILED;
    }
    return nErr;
}

//
// SerialPortParams2Interface
//
#pragma mark - SerialPortParams2Interface

void X2PowerControl::portName(BasicStringInterface& str) const
{
	std::string sPortName;
	getPortName(sPortName);
	str = sPortName.c_str();

}

void X2PowerControl::setPortName(const char* szPort)
{
	if (m_pIniUtil)
		m_pIniUtil->writeString(PARENT_KEY, CHILD_KEY_PORTNAME, szPort);
}


void X2PowerControl::getPortName(std::string &sPortName) const
{
	sPortName.assign(DEF_PORT_NAME);

	if (m_pIniUtil) {
		char port[255];
		m_pIniUtil->readString(PARENT_KEY, CHILD_KEY_PORTNAME, sPortName.c_str(), port, 255);
		sPortName.assign(port);
	}

}


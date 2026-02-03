#include <string.h>

#include "../../licensedinterfaces/theskyxfacadefordriversinterface.h"
#include "../../licensedinterfaces/sleeperinterface.h"
#include "../../licensedinterfaces/loggerinterface.h"
#include "../../licensedinterfaces/basiciniutilinterface.h"
#include "../../licensedinterfaces/mutexinterface.h"
#include "../../licensedinterfaces/basicstringinterface.h"
#include "../../licensedinterfaces/tickcountinterface.h"
#include "../../licensedinterfaces/powercontroldriverinterface.h"
#include "../../licensedinterfaces/sberrorx.h"
#include "../../licensedinterfaces/circuitlabels.h"
#include "../../licensedinterfaces/setcircuitlabels.h"
#include "../../licensedinterfaces/serialportparams2interface.h"

#include "RelayModule.h"

// Forward declare the interfaces that this device is dependent upon
class BasicStringInterface;
class SerXInterface;
class TheSkyXFacadeForDriversInterface;
class SleeperInterface;
class BasicIniUtilInterface;
class LoggerInterface;
class MutexInterface;
class TickCountInterface;
class CircuitLabelsInterface;

#define PARENT_KEY			        "USBPowerModule"
#define CHILD_KEY_PORTNAME	        "PortName"
#define DEF_PORT_NAME				"No port found"

#define CHILD_KEY_PORT1_NAME	"PLug1"
#define CHILD_KEY_PORT2_NAME	"PLug2"
#define CHILD_KEY_PORT3_NAME	"PLug3"
#define CHILD_KEY_PORT4_NAME	"PLug4"

 class X2PowerControl : public PowerControlDriverInterface, public CircuitLabelsInterface, public SetCircuitLabelsInterface, public SerialPortParams2Interface 
{
public:
	X2PowerControl( const char* pszDisplayName,
                    const int& nInstanceIndex,
                    SerXInterface						* pSerXIn,
                    TheSkyXFacadeForDriversInterface	* pTheSkyXIn,
                    SleeperInterface					* pSleeperIn,
                    BasicIniUtilInterface				* pIniUtilIn,
                    LoggerInterface						* pLoggerIn,
                    MutexInterface						* pIOMutexIn,
                    TickCountInterface					* pTickCountIn);

    virtual ~X2PowerControl();


public:

	/*!\name DriverRootInterface Implementation
	See DriverRootInterface.*/
	//@{
	virtual DeviceType  deviceType(void) {return DriverRootInterface::DT_POWERCONTROL;}
	virtual int         queryAbstraction(const char* pszName, void** ppVal);
	//@}

	/*!\name DriverInfoInterface Implementation
	See DriverInfoInterface.*/
	//@{
	virtual void        driverInfoDetailedInfo(BasicStringInterface& str) const;
	virtual double      driverInfoVersion(void) const;
	//@}

	/*!\name HardwareInfoInterface Implementation
	See HardwareInfoInterface.*/
	//@{
	virtual void deviceInfoNameShort(BasicStringInterface& str) const;
	virtual void deviceInfoNameLong(BasicStringInterface& str) const;
	virtual void deviceInfoDetailedDescription(BasicStringInterface& str) const;
	virtual void deviceInfoFirmwareVersion(BasicStringInterface& str);
	virtual void deviceInfoModel(BasicStringInterface& str);
	//@}

	/*!\name LinkInterface Implementation
	See LinkInterface.*/
	//@{
	virtual int		establishLink(void);
	virtual int		terminateLink(void);
	virtual bool	isLinked(void) const;
	//@}

	//PowerControlDriverInterface
	virtual int		numberOfCircuits(int& nNumber);
	virtual int		circuitState(const int& nIndex, bool& bZeroForOffOneForOn);
	virtual int		setCircuitState(const int& nIndex, const bool& bZeroForOffOneForOn);

    virtual int     circuitLabel (const int &nZeroBasedIndex, BasicStringInterface &str);
    virtual int     setCircuitLabel	(const int &nZeroBasedIndex, const char *str);

	//SerialPortParams2Interface
	virtual void			portName(BasicStringInterface& str) const;
	virtual void			setPortName(const char* szPort);
	virtual unsigned int	baudRate() const			{return 9600;};
	virtual void			setBaudRate(unsigned int)	{};
	virtual bool			isBaudRateFixed() const		{return true;};

	virtual SerXInterface::Parity	parity() const				{return SerXInterface::B_NOPARITY;};
	virtual void					setParity(const SerXInterface::Parity& parity){};
	virtual bool					isParityFixed() const		{return true;};


private:

	//Standard device driver tools
	TheSkyXFacadeForDriversInterface*	m_pTheSkyXForMounts;
	SleeperInterface*                   m_pSleeper;
	BasicIniUtilInterface*              m_pIniUtil;
	LoggerInterface*                    m_pLogger;
	mutable MutexInterface*             m_pIOMutex;
	TickCountInterface*                 m_pTickCount;

	TheSkyXFacadeForDriversInterface    *GetTheSkyXFacadeForDrivers() { return m_pTheSkyXForMounts; }
	SleeperInterface                    *GetSleeper() { return m_pSleeper; }
	BasicIniUtilInterface               *GetSimpleIniUtil() { return m_pIniUtil; }
	LoggerInterface                     *GetLogger() { return m_pLogger; }
	MutexInterface                      *GetMutex() { return m_pIOMutex; }
	TickCountInterface                  *GetTickCountInterface() { return m_pTickCount; }

	bool    m_bLinked;
	int	    m_nISIndex;
    
	CRelayModule	m_PowerPorts;

	void getPortName(std::string &sPortName) const;

    std::vector<std::string>    m_sPortNames;
    std::vector<std::string>    m_IniPortKey = {CHILD_KEY_PORT1_NAME, CHILD_KEY_PORT2_NAME, CHILD_KEY_PORT3_NAME, CHILD_KEY_PORT4_NAME};
};

/************************************************************************/
/*                                                                      */
/*    ncDriverDefines                                                   */
/*      Defines various header information and structs that are         */
/*      to be used by the service in order to communicate with          */
/*      with the driver.                                                */
/*                                                                      */
/************************************************************************/

/*
 * Control Codes
 *	Used as a line of security 
 *	against spoofing
 */
#define NC_CONNECTION_CODE_IMAGES			6254
#define NC_CONNECTION_CODE_SYSINFO			1551
#define NC_CONNECTION_CODE_THREADS			2061
#define NC_CONNECTION_CODE_PROCESSES		8105

/*
 * Version
 *	Used to detect version changes
 *
 *	This is the driver/service protocol link
 *	version, and does not reflect upon the overall
 *	noCheat version!
 */
#define NC_DS_LINK_VERSION 2

/*
 * Event Backlog
 *	Defines how many events can back up
 *	before the driver starts ignoring events
 *
 *	If the amount of information getting exchanged
 *	in a single event becomes rather large, or if
 *	errors start emerging about buffer sizes, then
 *	lower this number.
 */
#define NC_EVENT_BACKLOG 100

/*
 * Security code
 *	Used to verify the validity of the connection
 *
 *	Formerly "EatStool" from V1
 */
#define NC_LINK_SEC_CODE 0xEA757001

/*
 * A single image event object
 */
struct NC_IMAGE_EVENT
{
	char szImageName[260];	// MAX_PATH which isn't defined in the DDK
	unsigned __int32 iPID;
	unsigned __int32 iSize;
	unsigned __int64 iImageBase;
	unsigned char bKernelLand;
};

/*
 * Holds image events along with some
 *	header info
 */
struct NC_IMAGE_CONTAINER
{
	unsigned __int16 iCount;
	struct NC_IMAGE_EVENT oEvents[NC_EVENT_BACKLOG];
};

/*
 * Information sent from the service
 *	to the driver upon link
 */
struct NC_CONNECT_INFO_R
{
	__int32 iSecurityCode;

	unsigned __int32 iNCConnectInfoRSize;
	unsigned __int32 iNCImageEventSize;
	unsigned __int32 iNCImageContainerSize;

	unsigned __int16 iDSLinkVersion;

	unsigned __int64 pBuff;
};
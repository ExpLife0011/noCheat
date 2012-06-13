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
#define NC_CONNECTION_CODE_IMAGES			1001
#define NC_CONNECTION_CODE_MINIPORT_MAC		1551

/*
 * Version
 *	Used to detect version changes
 *
 *	This is the driver/service protocol link
 *	version, and does not reflect upon the overall
 *	noCheat version!
 */
#define NC_DS_LINK_VERSION 1

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
	union
	{
		unsigned char WriteBlocks;
		struct 
		{
			unsigned char bDriverWriting : 1;
			unsigned char bServiceWriting : 7;
		};
	};
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

#if defined(_AMD64_) || defined(_IA64_) || defined(_M_IA64) || defined(_M_X64)	// 64-bit
	__int64 pBuff;
#else																			// 32-bit
	__int64 pBuff;
	//__int32 vPadding;
#endif
};
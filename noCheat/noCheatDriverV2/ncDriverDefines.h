/************************************************************************/
/*                                                                      */
/*    ncDriverDefines                                                   */
/*      Defines various header information and structs that are         */
/*      to be used by the service in order to communicate with          */
/*      with the driver.                                                */
/*                                                                      */
/************************************************************************/
#ifndef NCH_DRIVERDEFINES__
#define NCH_DRIVERDEFINES__

/*
 * Control Code
 *	Used as a line of security 
 *	against spoofing
 */
#define NC_CONNECTION_CODE			6254

/*
 * Version
 *	Used to detect version changes
 *
 *	This is the driver/service protocol link
 *	version, and does not reflect upon the overall
 *	noCheat version!
 */
#define NC_DS_LINK_VERSION 4

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
#define NC_EVENT_BACKLOG 10

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
 * A single process event object
 */
struct NC_PROCESS_EVENT
{
	unsigned char bExtended;
	unsigned __int32 iParentPID;
	unsigned __int32 iPID;
	unsigned char szCommandLine[2048];		// Maximum command line length
	unsigned char szProcessFileName[260];	// MAX_PATH which isn't defined in the DDK
	unsigned char szFileName[260];			//  || -- This is the filename pointed to by the fileobject itself

	struct
	{
		unsigned __int32 iUniqueProcess;
		unsigned __int32 iUniqueThread;
	} iCallingThread;
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
 * Holds process events along with
 *	some header info
 */
struct NC_PROCESS_CONTAINER
{
	unsigned __int16 iCount;
	struct NC_PROCESS_EVENT oEvents[NC_EVENT_BACKLOG];
};

/*
 * Information sent from the driver
 *	to the service upon link
 */
struct NC_CONNECT_INFO_OUTPUT
{
	union
	{
		unsigned char Parameters;
		struct  
		{
			unsigned char bSuccess : 1;			// The linking was successful
			unsigned char bBlocked : 1;			// Whether or not the connection was blocked (there is already another connection)
			unsigned char bAccessDenied : 1;	// Access denied (security verifications went wrong!)
			unsigned char bSizeMismatch : 1;	// An event struct's size is mismatched
		};
	};
};

/*
 * Information sent from the service
 *	to the driver upon link
 */
struct NC_CONNECT_INFO_INPUT
{
	unsigned __int32 iSecurityCode;							// Security code

	unsigned __int32 iReturnSize;							// - NC_CONNECT_INFO_OUTPUT			NOTE: input size is not needed, as it is checked by the driver automatically
	unsigned __int32 iImageEventSize;						// - NC_IMAGE_EVENT
	unsigned __int32 iImageContainerSize;					// - NC_IMAGE_CONTAINER
	unsigned __int32 iProcessEventSize;						// - NC_PROCESS_EVENT
	unsigned __int32 iProcessContainerSize;					// - NC_PROCESS_CONTAINER

	unsigned __int16 iDSLinkVersion;						// Link protocol version	

	unsigned __int64 pReturnInfo;							// Return information struct
	unsigned __int64 pImageLoadContainer;					// Container for image-loading events
	unsigned __int64 pProcessCreateContainer;				// Container for process creation events
	unsigned __int64 pThreadCreateContainer;				// Container for thread creation events
	unsigned __int64 pSysInfoIn;							// Container for input-events regarding system info
	unsigned __int64 pSysInfoOut;							// Container for output-events regarding system info
	unsigned __int64 pProcInfoIn;							// Container for input-events regarding process info
	unsigned __int64 pProcInfoOut;							// Container for output-events regarding process info
};


#endif
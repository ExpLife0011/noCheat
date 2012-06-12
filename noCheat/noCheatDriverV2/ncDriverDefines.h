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
#define NC_CONNECTION_CODE_IMAGES			1000
#define NC_CONNECTION_CODE_MINIPORT_MAC		1551

/*
 * Information sent from the service
 *	to the driver upon link
 */
struct NC_CONNECT_INFO_R
{
	char Padding;
};

/*
 * Information sent from the driver
 *	back to the service upon link
 *
 *	This information is sent on the 
 *	connection buffer as opposed to
 *	the output buffer!
 */
struct NC_CONNECT_INFO_S
{
	char Padding;
};

static char cpyrght[] = "Copyright NCR Corporation, Atlanta, Georgia. U.S.A. 1996-2013."; 

#include <prec.h>
#include <direct.h>

#include "5991sup.h"
#include "AllDictionaryWords.h"
#include "appmnufn.h"
#include "appmon.h"
#include "Callbacks.h"                              // GL 31Mar09 VPOSD 2008.09.08 Migration
#include "CentralReturnsMgr.h"                      // GL 05May06 Phase 8.6
#include "cmosappl.h"
#include "custdisp.h"
#include "CustomerDataCaptureMgr.h"                 // GL 01Feb06 Phase 8.5
#include "errs.h"
#include "fileio.h"
#include "flowvbld.h"
#include "gettimda.h"
#include "GiftCardMallManager.h"                    // GL 06Jul10 Gift Card Mall Phase 12
#include "hdprmreq.h"
#include "helpsys.h"
#include "i_error.h"			                    // GL 28Apr09 Phase 11 VPOSD Migration
#include "InvokeHelpEvent.h"                        // GL 19Mar03 SQA 1856
#include "LoyaltyManager.h"
#include "NCRSmartSwitchManager.h"                  // GL 28Apr08 Phase 10.0
#include "NonPOSMgr.h"                              // GL 03Jan02 Internet Explorer i/f
#include "params.h"
#include "POS2000Parameters.h"
#include "posconst.h"
#include "posinit.h"
#include "pos_wind.h"
#include "PromotionsManager.h"			            // GL 11Jan11 Phase 14.5 Promotions
#include "relinfo.h"
#include "ReturnsManager.h"				            // GL 14Apr05 Phase 8.2.5
#include "sr_mgr.h"                                 // GL 21May09 Phase 11 VPOSD Upgrade
#include "sysvars.h"
#include "tones.h"						            // SQA 488
#include "VoucherManager.h"				            // GL 24Jun04 Phase 7.7
#include "DebServiceManager.h"
#include "ComplexConcessionsManager.h"
#include "tprntobj.h"
#include "admsuobj.h"
#include "tone.h"	
// DS 27Jul09 - Release 11 - PCI - need a new include file
#include "EncryptionFuncs.h"
#include <Focuser.h>                                // DS 28Mar12 - Loss of Focus 
#include "EnableDisableTransaction.h"

extern DebServiceManager *debSrvcMgr;
extern RCSTrace          *g_pTrace;                 // GL 08Apr09 Phase 11 Migration
extern NonPOSManager     *NonPOSMgr;                // GL 03Jan02 Internet Explorer i/f
extern PromotionsManager *promotionsMgr;            // GL 11Jan11 Phase 14.5 Promotions    
extern Tone              *tone;

// GL 05Mar03 - new RCS tracing functionality
#include "RCSTrace.h"
// GL 23Jan04 - moved to swconfig.cpp
//RCSTrace Trace( C_POSTRACE_NAME, NULL, C_POSTRACE_FILE_NAME );
extern RCSTrace Trace;

//#ifdef SM4AS
//#include "SM4AS_Interface.h"
//#endif

Transaction         *WorkingTransaction = NULL;
BOOL                _PC_Mode            = FALSE;
BOOL                _Supervisor_Mode    = FALSE;
POS2000Parameters   FileParameters;
BOOL                _SaveBarcodeLookups = FALSE;
BOOL                _Use_Store_Logo     = FALSE;
BOOL                ComplexConcessionsRegister = FALSE;
void                ListboxItemChangedEventCallback( ListBox *pListBox,	int nItemNumber );  // GL 20Sep13 DEBEN-191 forward declaration for callback    

// KH - EMU Processing - 7156
int OKToLoadEuroFont = 0; // 0=No 1=Yes 2=Already loaded for this receipt.

int UpctnSeqLastDigit = 0;

extern CashDrawer *cashdrawer;
extern CRManager            *CentralReturnsMgr;	        // GL 01Feb06 Phase 8.5
extern AppMenuPage          *CurrentAppPage;
extern CDCManager           *CustomerDataCaptureMgr;    // GL 01Feb06 Phase 8.5
extern EPOS2000AppMonitor *EP2KAppMon;
extern EPOS2000Help *EP2KHelp;
extern GiftCardMallManager  *GiftCardMallMgr;           // GL 06Jul10 Gift Card Mall Phase 12.0
extern LoyaltyManager       *LoyaltyMgr;
extern ParameterList         Parameters;
extern ReturnsManager       *ReturnsMgr;				// GL 14Apr05 Phase 8.25
extern VoucherManager       *VoucherMgr;				// GL 24Jun04 Phase 7.7
extern ComplexConcessionsManager *ComplexConcessionsMgr;

// KB - SQA 782 - Foreign Tender Cleanup
extern void InitializeForeignTenderParameters( void );
extern void RcsClearBufferedInput();			// GL 14Mar01
extern void WriteEJTmpFile( char *, unsigned int );

// GL VPOSD 4.9.1 TCPSERVER - do not try and reconnect - leave this to TCP Heartbeat
#ifdef TCPSERVER
#include <TCPServer.h>
// #include <TcpConMgr.h>
// extern IRcsTcp      *rcstcp;
// GL 11May09 Phase 11. Replace the above with the following...
extern RCSComPtr<IRcsTcp>    rcstcp;
extern IRcsTcpDiag          *rcstcpdiag;
// extern RCSComPtr<IRCSTcpConMgr> _tcp_con_mgr;
#endif

extern bool g_LogoFileExists;       // GL 12Sep03 SQA 2071

#include "NCREAppsManager.h"        // GL 02Sep03 EApps for GE Capital
extern EAppsManager *EAppsMgr;

extern bool ValidateAsNumeric( const char *OriginalData, bool AllowDecimal, bool AllowSpaces );
extern long PrePostRepeatKeyTimeout;	 

extern bool	g_KioskMode;
extern bool	g_MobileAppServerMode;          // GL 15Feb13 MobileApp
extern void DisableGrid();                  // GP 08Nov10 SQA 207
extern bool EnableGrid();               
extern BOOL IsFoodRegister();
extern bool IsGridEnabled();
extern BOOL IsPOSTouchRegister();                       
extern BOOL IsTouchRegister();                          

// DS 3Jun09 Phase 11 - Need this function for the check face
void CheckFace2Func( int nField, LPTSTR pBuffer, int nFieldLen,	long nOther );

// GL 05Mar03 - Adopt RCSTrace functionality inplace of homegrown variety !
// Trace levels : TLvl_Normal, TLvl_Verbose, TLvl_VeryVerbose - see RCSTrace.h for info

// Use the following instead
// Trace.Trace( TLvl_Verbose, line );



extern int nAppFrameWidth;
extern int nAppFrameHeight;

// GL 03May01 - only used for debug.
void RegisterApplicationErrHander( long plError )
{
	LPVOID lpMsgBuf;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				   FORMAT_MESSAGE_FROM_SYSTEM | 
				   FORMAT_MESSAGE_IGNORE_INSERTS,
				   NULL,
				   plError,				// GetLastError(),
				   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				   (LPTSTR) &lpMsgBuf,
				   0, NULL );

	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
}
    
void RegisterApplication()
{
    char				sClass[ 32 ];
    HKEY				phkRes;
    unsigned long		plDisposition;
    long				lLen=0;
    unsigned char		sType[ 32 ];
    unsigned char		sVersion[ 32 ];
	long                lRes=0;
    
    // Open the subkey (create if it doesn't exist)
    strcpy( sClass, "POS 2000" );
    lRes = RegCreateKeyEx( HKEY_LOCAL_MACHINE, 
						   C_APP_SUB_KEY, 
						   0, 
						   sClass, 
						   REG_OPTION_NON_VOLATILE, 
						   KEY_ALL_ACCESS, 
						   NULL, 
						   &phkRes, 
						   &plDisposition );
    
//	if( lRes != ERROR_SUCCESS )
//		RegisterApplicationErrHander( lRes );
		
    // Create the key for the version information
    strcpy( (char *)sVersion, POS_RELEASE_NUMBER_STR );
    lLen = strlen( (char *)sVersion ) + 1;
    
    strcpy( sClass, "CurrentVersion" );
    lRes = RegSetValueEx( phkRes, 
						  sClass, 
						  0, 
						  REG_SZ, 
						  sVersion, 
						  lLen );

//	if( lRes != ERROR_SUCCESS )
//		RegisterApplicationErrHander( lRes );
    
    strcpy( sClass, "Resolution" );
    // And the current resolution
	sprintf( (char *)sType, "%dX%d", nAppFrameWidth, nAppFrameHeight );
    lLen = strlen( (char *)sType ) + 1;
    
    lRes = RegSetValueEx( phkRes, 
						  sClass, 
						  0, 
						  REG_SZ, 
						  sType, 
						  lLen );

//	if( lRes != ERROR_SUCCESS )
//		RegisterApplicationErrHander( lRes );
    
    lRes = RegCloseKey( phkRes );

//	if( lRes != ERROR_SUCCESS )
//		RegisterApplicationErrHander( lRes );
}

BOOL DetermineTCPIPNumber( char *pszTCPIPNumber )
{
	WORD	wVersionRequested;
	WSADATA	wsaData;
	int		err; 
	char	UserMsg[ C_MaxStringLength ];
	char	hostname[ C_MaxStringLength ];
	HOSTENT *host;
//	byte	temp_data[ 16 ];        // GL 04Dec01 - no longer required
	byte	hex_ip_address[ 16 ];
	char	ip_address[ 20 ];
	char	szIP[ 4 ];
	unsigned int utmp;                                                 
	
	wVersionRequested = MAKEWORD( 2, 2 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
	if( err != 0 ) 
	{
		// Tell the user that we could not find a usable 
		// WinSock DLL.                                  
		return FALSE;
	} 

	// Confirm that the WinSock DLL supports 2.2.
	// Note that if the DLL supports versions greater    
	// than 2.2 in addition to 2.2, it will still return 
	// 2.2 in wVersion since that is the version we      
	// requested.                                         

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		// Tell the user that we could not find a usable 
		// WinSock DLL.                                      
		WSACleanup();
		return FALSE; 
	}

    if( gethostname( hostname, C_MaxStringLength ) == SOCKET_ERROR )
	{
		sprintf( UserMsg,  "Windows Sockets error %ld",  WSAGetLastError() );
		fldebug( UserMsg );
		return FALSE;
	}

    // GL 04Dec01 - Original code
	//host = gethostbyname( hostname ); 
	
	//memcpy( temp_data, host->h_addr_list, 16 );
	//memcpy( hex_ip_address, temp_data+12, 4 );

    memset( hex_ip_address, '\0', 16 );                         // GL 15Apr02 - Additional safety checks

    // GL 04Dec01 - New code suggested by Curtiss Monroe
    host = gethostbyname( hostname );

    if( host != NULL )                                          // GL 15Apr02 - Additional safety checks
    {
        memcpy( hex_ip_address, host->h_addr, 4 );
        // New code ends

	    memset( ip_address, '\0', 20 );

	    for( int i = 0; i < 4; i++ )
	    {
		    utmp = (unsigned char) hex_ip_address[ i ];
		    
		    sprintf( szIP, "%d",  utmp );

		    strcat( ip_address, szIP ); 

		    if( i < 3 )
			    strcat( ip_address, "." );
	    }

    	strcpy( pszTCPIPNumber, ip_address );
    }
    else
    	strcpy( pszTCPIPNumber, "<UNKNOWN>" );

	return TRUE;
}

// Requires OPOS 01.06.02 - does not work on PCs
void DisablePowerSwitch()
{
	// Set the following keys as indicated
	// HKEY_LOCAL_MACHINE/CurrentControlSet/Services/NCRKMPR/Parameters
	
	// PowerOff = 1    - enables power off at "OK to power off your PC" screen
	// PowerSwitch = 0 - disables the power switch

	// If the current tone control is using OPOS 01.06.02 then we should be fine
	if( CURRENT_TONE_VERSION == OPOS_TONE_CONTROL_01_06_02 )
	{
		Trace.Trace( TLvl_Verbose, "Power Switch Disabled!" );
		// Code goes here
	}
	else
	{
		if( CURRENT_TONE_VERSION == OPOS_TONE_CONTROL_01_04_00 ) 
			Trace.Trace( TLvl_Verbose, "Power Switch : OPOS 01.04.00 - no support !" );

		if( getenv("POSPC") != NULL)
			Trace.Trace( TLvl_Verbose, "Power Switch : Running on PC !" );
	}
}

void RefreshSystemVariables()
{
	RefreshVariable( SYSTEM_DATE );
	RefreshVariable( SYSTEM_TIME );
	
	SetTimedEventTimer( RefreshSystemVariables, 3 );
}

void ResetSystemVariables()
{
	ClearTimedEventTimer( RefreshSystemVariables );
	RefreshSystemVariables();
}

void HandleSpecialParameters(void)
{
	char buffer[ C_MaxCommBufferLength+1 ];
	char buffer2[ C_MaxCommBufferLength+1 ];
	char parm_val[ C_MaxCommBufferLength+1 ];
	int parm_len = 0;
	int left_pad_len = 0;
	int right_pad_len = 0;
	int i = 0;
	int receiptWidth = 42;
	int useDoubleWide = 0;

	i = 0;

	// the first special parameter is 
	// P_RCPT_HEADER_2 which is the P_STORE_NAME
	// parameter.  This is always the default value
	// from the file or POS parameters
	Parameters.Get( P_STORE_NAME, buffer );

    // GL 12Sep03 - SQA 2071
    // If the logo file exists then blank out any value in P_STORE_NAME 
    if( g_LogoFileExists == true )
        strcpy( buffer, "" );

	Parameters.Set( P_RCPT_HEADER2, buffer );
	Parameters.Set( P_RCPT_HEADER2_ORIGINAL, buffer );

	// the next phase will center and choose format for the parameters
	// the above parameter is special in that it is not loaded 
	// anywhere else in the application.

	while( SpecialParameters[ i ].parameter != -1 )
	{
		receiptWidth = 42;
		useDoubleWide = 0;
		memset( buffer, '\0', C_MaxCommBufferLength+1 );
		memset( buffer2, '\0', C_MaxCommBufferLength+1 );
		memset( parm_val, '\0', C_MaxCommBufferLength+1 );

		parm_len = Parameters.Get( SpecialParameters[ i ].parameterOriginal, parm_val );

		// 42 is the width of the receipt and journal for single wide
		// 21 is the width of the receipt and journal for double wide strings

		// eat the leading and trailing white space in case the parameter
		// was saved as its centered value

		char *ptr1 = parm_val;
		while( ptr1 != '\0' && isspace( *ptr1 ) && (ptr1-parm_val < parm_len) )
			ptr1++;

		char *ptr2 = &parm_val[ strlen( parm_val ) ];
		if( strlen( ptr1 ) )
		{
			while( isspace( *( ptr2-1 )) )
				ptr2--;
		}

		// put the new data string back into parm_val and adjust length
		strncpy( parm_val, ptr1, ptr2-ptr1 );
		parm_val[ ptr2-ptr1 ] = 0;
		parm_len = strlen( parm_val );
		char *ptr = parm_val;

		// the first non-whitespace character determines width of the string
		// if lower - single wide - 42 width receipt
		// if upper - double wide - 21 width receipt

		if( *ptr && isupper( *ptr ) )
			useDoubleWide = 1;

		_strupr( parm_val );

		receiptWidth = 42;
		if( parm_len > receiptWidth )
		{
			parm_val[ receiptWidth ] = 0;
			parm_len = receiptWidth;
			strcpy( buffer, parm_val );
		}
		else
		{
			left_pad_len =  (receiptWidth - parm_len) / 2;
			right_pad_len = receiptWidth - (parm_len + left_pad_len);
			sprintf(buffer, "%*c%*s%*c", left_pad_len, ' ', parm_len, parm_val, right_pad_len, ' ');
		}

		receiptWidth = 21;
		if( parm_len > receiptWidth )
		{
			parm_val[ receiptWidth ] = 0;
			parm_len = receiptWidth;
			strcpy( buffer2, parm_val );
		}
		else
		{
			left_pad_len =  (receiptWidth - parm_len) / 2;
			right_pad_len = receiptWidth - (parm_len + left_pad_len);
			sprintf(buffer2, "%*c%*s%*c", left_pad_len, ' ', parm_len, parm_val, right_pad_len, ' ');
		}

		Parameters.Set( SpecialParameters[ i ].parameter, buffer );
		Parameters.Set( SpecialParameters[ i ].parameterDouble, buffer2 );

		if( useDoubleWide )
			*SpecialParameters[i].useDouble = 1;
		else
			*SpecialParameters[i].useDouble = 0;

		i++;
	}

   	// KB - SQA 782 - need to clear the default values for exchange rate parameters
	// that are not needed.
	InitializeForeignTenderParameters();

	// GL 23Jan06 Phase 8.5 Ensure the returns manager settings take precedence
	// for the footers set in posparam.ini
    // KH - 27Mar06
    // These are obtained at the point they are applied in transact and are no longer part
    // of the Returns Policy code.
	//ReturnsMgr->ForceReset();
}

// KH - VAT ON CREDIT
// GL 20Apr04 CR286 vsn 1.3 This call now done directly via posparam.ini files and not
// via the parameter subsystem. See transaction::SetDCHSPrintLine
/*
void HandleDCHSParameters( void )
{
	char buffer[ C_MaxStringLength ];
	char parm_val[ C_MaxStringLength ];
	int parm_len = 0;
	int left_pad_len = 0;
	int right_pad_len = 0;
	int i = 0;
	int receiptWidth = 54; // DCHS fields print in compressed format.

	i = 0;

    // Centre the parameter and update it.
	while( DCHSParameters[ i ].parameter != -1 )
	{
		receiptWidth = 54;
		memset( buffer, '\0', C_MaxStringLength );
		memset( parm_val, '\0', C_MaxStringLength );

		parm_len = Parameters.Get( DCHSParameters[ i ].parameter, parm_val );

	    // eat the leading and trailing white space in case the parameter
		// was saved as its centered value

		char *ptr1 = parm_val;
		while( ptr1 != '\0' && isspace( *ptr1 ) && (ptr1-parm_val < parm_len) )
			ptr1++;

		char *ptr2 = &parm_val[ strlen( parm_val ) ];
		if( strlen( ptr1 ) )
		{
			while ( isspace( *( ptr2-1 )))
				ptr2--;
		}

		// put the new data string back into parm_val and adjust length
		strncpy( parm_val, ptr1, ptr2-ptr1 );
		parm_val[ ptr2-ptr1 ] = 0;
		parm_len = strlen( parm_val );
		char *ptr = parm_val;

		if( parm_len > receiptWidth )
		{
			parm_val[ receiptWidth ] = 0;
			parm_len = receiptWidth;
			strcpy( buffer, parm_val );
		}
		else
		{
			left_pad_len =  (receiptWidth - parm_len) / 2;
			right_pad_len = receiptWidth - (parm_len + left_pad_len);
			sprintf(buffer, "%*c%*s%*c", left_pad_len, ' ', parm_len, parm_val, right_pad_len, ' ');
		}

		Parameters.Set( DCHSParameters[ i ].parameter, buffer );

		i++;
	}
}
*/

// DS 18Jan12 - SQA 77 Added WriteParametersToDisk parameter
int ReadParameterFile( BOOL ForcedReload, BOOL WriteParametersToDisk )
{
	char buffer[ C_MaxStringLength ];
	char defaultValue[ C_MaxStringLength ];
	int  retrievedParams = 0;

	// reset all parameters to the default values
	Parameters.ClearAll();

	HardDriveParmRequest *hdprm = new HardDriveParmRequest( APPLICATION, 0 );

	if( !ForcedReload )
	{
		// the file contained no parameters or no file exists
		// get the last good copy from disk
		if( hdprm )
			retrievedParams = hdprm->GetParmsFromHardDrive();
	}
	
	if( retrievedParams == 0 )
	{
		// read the defaults from the file
		int i = 0;
		while( ParameterDefinitions[ i ].parameter != -1 )
		{
			Parameters.Get( ParameterDefinitions[ i ].parameter, defaultValue );

			FileParameters.Get( ParameterDefinitions[ i ].section, ParameterDefinitions[ i ].key, buffer, C_MaxStringLength, defaultValue );
			Parameters.Set( ParameterDefinitions[ i ].parameter, (const char *) buffer );

			i++;
		}

		// DS 18Jan12 - SQA 77 Only write these parameters if WriteParametersToDisk is set to TRUE
		if (WriteParametersToDisk)
			if( hdprm )
				hdprm->SaveParmsToHardDrive();

		retrievedParams = i;
	}

	if( hdprm )
	{
		delete hdprm;
		hdprm = NULL;
	}
	
	HandleSpecialParameters();

    // GL 20Apr04 CR286 - This call now integrated with the Transaction class
    // HandleDCHSParameters();

	return retrievedParams;
}

void download_logo()
{
// KH - 4.8.4+ Designer Changes - THIS IS NOW HANDLED IN THE HWCONFIG.INI FILE
#if 0
	struct _stat stbuf;
	
	if( !_stat( "store.lgo", &stbuf ) )
	{
		// Send a LOGO file to the 7156 printer
		short fh = file_open( "store.lgo", _O_RDONLY|_O_BINARY, 0 );
		if( fh > 0 )
		{
			char *buf = new char[ stbuf.st_size+1];
			if( buf )
			{
				memset( buf, '\0', stbuf.st_size+1 );

				short len = file_read( fh, buf, stbuf.st_size );

				// void SetErrorMessage(int errorno,int errormsg);
				if( FLPrinter && len )
				{
					//FLPrinter->SetErrorMessage(P7156_INSERT_SLIP_ERROR, 0);
					//FLPrinter->SetErrorMessage(P7156_REMOVE_SLIP_ERROR,0);
				//	FLPrinter->SetErrorMessage(P7156_MOTOR_JAM_ERROR,0);
					//FLPrinter->SetErrorMessage(P7156_PAPER_OUT_ERROR,0);
					//FLPrinter->SetErrorMessage(P7156_DOOR_OPEN_ERROR,0);
					//FLPrinter->SetErrorMessage(P7156_COMM_ERROR,0);

					FLPrinter->Write(RECEIPT,buf,len);

					//FLPrinter->SetErrorMessage(P7156_INSERT_SLIP_ERROR,INSERT_SLIP_ERROR);
					// FLPrinter->SetErrorMessage(P7156_REMOVE_SLIP_ERROR,REMOVE_SLIP_ERROR);
					//FLPrinter->SetErrorMessage(P7156_MOTOR_JAM_ERROR,PRINTER_MOTOR_JAM_ERROR);
					//FLPrinter->SetErrorMessage(P7156_PAPER_OUT_ERROR,PAPER_OUT_ERROR);
					//FLPrinter->SetErrorMessage(P7156_DOOR_OPEN_ERROR,PRINTER_DOOR_OPEN_ERROR);
				//	FLPrinter->SetErrorMessage(P7156_COMM_ERROR,PRINTER_COMM_ERROR);
				}

				_Use_Store_Logo = TRUE;

				delete [] buf;
				buf = NULL;
			}

			file_close( fh );
		}
	}
#endif
}

// KH - EMU Processing - 7156
// NB. There are separate loads for the receipt/slip printer as combining them causes serious problems with printing.

void download_euro_to_receipt()
{
    // KH - Define the Euro symbol in charater set 437.
    // This data is sent at the point when the first amount AttributeWithIndicator is printed on the receipt.
    // This is currently OK as the EURO summary info appears AFTER this.
    char EuroBuffer[ C_MaxStringLength ];

    char EuroBufferA[ 8 ] = "\x1B\x26\x03\xD5\xD5\x0A\x01";
    char EuroBufferB[ 8 ] = "\x40\x00\x0F\xFC\x00\x1F\xFE";
    char EuroBufferC[ 8 ] = "\x00\x39\x47\x00\x31\x43\x00";
    char EuroBufferD[ 8 ] = "\x31\x43\x00\x31\x43\x00\x18";
    char EuroBufferE[ 8 ] = "\x06\x00\x08\x04\x00\x00\x00";
    char EuroBufferF[ 5 ] = "\x00\x1B\x25\x03";

    strcpy( EuroBuffer, EuroBufferA );
    memcpy( &EuroBuffer[ 7 ], EuroBufferB, 7 );
    memcpy( &EuroBuffer[ 14 ], EuroBufferC, 7 );
    memcpy( &EuroBuffer[ 21 ], EuroBufferD, 7 );
    memcpy( &EuroBuffer[ 28 ], EuroBufferE, 7 );
    memcpy( &EuroBuffer[ 35 ], EuroBufferF, 4 );
    EuroBuffer[ 39 ] = '\0';  

    FLPrinter->Write( RECEIPT, EuroBuffer, 39 );
}

void download_euro_to_slip()
{
    // This data is sent at the point when the first amount AttributeWithIndicator is printed on the slip.
    char EuroBuffer[ C_MaxStringLength ];
    
    char EuroBufferG[ 9 ] = "\x1B\x26\x00\xD5\xD5\x00\x14";
    char EuroBufferH[ 8 ] = "\x08\x36\x41\x14\x41\x00\x22";
    char EuroBufferI[ 7 ] = "\x00\x00\x00\x1B\x25\x03";
                           
    memcpy( EuroBuffer, EuroBufferG, 7 ); 
    memcpy( &EuroBuffer[ 7 ], EuroBufferG, 7 ); 
    memcpy( &EuroBuffer[ 14 ], EuroBufferI, 6 ); 
    EuroBuffer[ 20 ] = '\0'; 

    FLPrinter->Write( SLIP, EuroBuffer, 20 ); 
}

// SQA 488 This function is called from within DisplayError.  FLBeepFunction is a function
// pointer that we can override.   See ApplicationStart below
// GL 31Mar09 VPOSD 2008.09.08 Migration Must remove ellipses
void NCRDisplayErrorBeep( int tone_type)
{
    // GL - Let silence rain during resumes.   Ahhhh, bliss !
    if( SRMgr && SRMgr->DoingResume() )
        return;

	// Play a low tone
	POSTone tc;

	if(tone_type == Tone_Click)
    {   if(tone)
		    tone->PlayPresetSound(Tone_Click);
        else
            tc.HighTone();
    }
	else
		tc.ErrorTone();
}

int NCRDisplayError( int iErrMsg )
{
	int rval = 0;
    bool restartMonitoringRequired = false;

	// Kiosk Chip & PIN
	// KH 28Jul11 if any specific changes are required in the code then those will need to be made as will based
	// on this default.
	if( g_KioskMode == true || g_MobileAppServerMode == true )
		return CLEAR_KEY;

    if( iErrMsg != -1 )
        Trace.Printf( TLvl_Normal, "POS Error (errs.h) %d", iErrMsg );

	// GL 14Mar01 - If we come across any errors at all then clear the input buffer
	RcsClearBufferedInput();

    // GL 20Nov08 SQA 123 Check if we're monitoring as we need to suspend if we pop any error message up to the screen
    if( NCRSmartSwitchMgr && NCRSmartSwitchMgr->SecurityAlertMonitoringInProgress() )
    {
        NCRSmartSwitchMgr->MonitorSecurityAlertKey( false );
        restartMonitoringRequired = true;
    }

    if(IsFoodRegister()) //sqa 207
    {
        bool reEnable = IsGridEnabled(); //sqa 286
        DisableGrid();
        rval = DisplayError( iErrMsg );
        if(reEnable) //sqa 286
            EnableGrid();
    }
    else
        rval = DisplayError( iErrMsg );

    // GL 20Nov08 SQA 123 If we were monitoring then restart that process now that the error has been cleared by the user
    if( restartMonitoringRequired == true && NCRSmartSwitchMgr )
        NCRSmartSwitchMgr->MonitorSecurityAlertKey( true );

    // KH - 10Apr07 - Make this a development parameter...
	char szAction[ 10 ];
    memset( szAction, '\0', 10 );

	GetPrivateProfileString( "Development Only", "SkipClearBufferAfterError", "no", szAction, 10, ".\\posparam.ini" );
	if( stricmp( "true", szAction ) == 0 || stricmp( "yes", szAction ) == 0 )
    {
        Trace.Trace( TLvl_Verbose, "Skipping Clear Buffer After Error" );
        return rval; // OK at present...
    }
    else
    {    
        // GL 14Mar01 - Discard any keyed input when the error is displayed
	    RcsClearBufferedInput();
    }
    // KH - 10Apr07 - End

    // return the term key !!
    return rval;
}

//GL 27Apr00 - Start external applications
void StartExternalApplications()
{
    // EP2K Help
    if( EP2KHelp )
    {
        EP2KHelp->Init();

        // GL 19Mar03 SQA 1856 - As a security measure always reset the Visible Key to false incase something 
        // happened to Help Subsystem and this key was accidentally left set to True.
        WritePrivateProfileString( C_EP2KHelpMainSection, 
           						   C_EP2KHelpVBAppVisibleKey,
	        					   "False",
		        				   C_EP2KHelpIniFilename );
    }

    // EP2K App Monitor
    if( EP2KAppMon )
    {
        EP2KAppMon->Init();
    }

    // GL - Initialise the Smartcard subsystem
// GL 22Jul08 NCRSmartcardManagerStart();

    // 28Apr08 GL Phase 10. Initialise the interface to SmartSwitch
    NCRSmartSwitchManagerStart();
}

void StopExternalApplications()
{
    // EP2K App Monitor
    if( EP2KAppMon )
        EP2KAppMon->Shutdown();

    // EP2K Help
    if( EP2KHelp )
        EP2KHelp->Shutdown();

    // Shutdown the Smartcard subsystem
// GL 22Jul08 NCRSmartcardManagerStop();

    // GL 28Apr08 Shutdown the interface to SmartSwitch
    NCRSmartSwitchManagerStop();
}

// DS 2Apr12 - SQA 193 - Loss of Focus Issue
char g_AppTitle[ _MAX_FNAME ];
char g_AppSearchTitle[ _MAX_FNAME ];
HWND g_AppHandle = NULL;
bool g_AppHideActive = true;

BOOL CALLBACK EnumWindowsBat(HWND hwnd, long lParam)
{
/*++

  Routine Description:
  
	Callback function for window enumeration.
	
	  Arguments:
	  
		hwnd			 - window handle
		lParam			 - ** not used **
		
		  Return Value:
		  
			TRUE  - continues the enumeration
			
	--*/
	
	char buf[1000];
	memset( buf, 0, sizeof(buf) );
	
	if (GetWindowText( hwnd, buf, sizeof(buf) )) 
	{
		Trace.Printf( TLvl_Normal, "Checking process %s against %s", buf, g_AppSearchTitle );

		if ( strstr( strlwr(buf), strlwr(g_AppSearchTitle) ) )
		{
			//
			// got it, so lets save it
			//

			Trace.Printf( TLvl_Normal, "Checking process %s against %s - Found a match", buf, g_AppSearchTitle );
			strcpy( g_AppTitle, buf );
			g_AppHandle = hwnd;
			
			// stop the enumeration
			return FALSE;
		}
	}
	
	//
	// continue the enumeration
	//
	return TRUE;
}

// DS 2Apr12 - SQA 193 - Loss of Focus Issue - END

void ApplicationStart()
{
    // GL 20Jul04 - No sig capture
	// void StartSigCap();
	// void StopSigCap();
    //void DoSysCredits();
	BOOL EJBackup();

	// DS 28Mar12 - Loss of Focus Issue this should have been included before
    g_Focuser.Init();                   // Initialize auto-focuser.

	// DS 2Apr12 - SQA 193 - Loss of Focus Issue
	char cBuffer[10];
	memset (cBuffer, 0, 10);
	GetPrivateProfileString( "Application Hiding", "APPLICATION_HIDING_ACTIVE", "true", cBuffer, 10, ".\\posparam.INI" );
    
    if( stricmp(cBuffer, "yes") == 0 || stricmp(cBuffer, "true") == 0 )
        g_AppHideActive = true;
	else
        g_AppHideActive = false;

	if ( g_AppHideActive == true )
	{
		memset( g_AppTitle, 0, _MAX_FNAME );
		memset( g_AppSearchTitle, 0, _MAX_FNAME );
		
		strcpy( g_AppSearchTitle, "debenhams.bat" );
		
		
		//
		// enumerate all windows
		//
		EnumWindows( EnumWindowsBat, (DWORD) 0 );
		
		if (strlen( g_AppTitle ) && g_AppHandle )
		{
			ShowWindow (g_AppHandle, SW_HIDE);
		}
	}

	// DS 2Apr12 - SQA 193 - Loss of Focus Issue - END



//	extern void DoubleBeep ();
//	SetTimedEventTimer(DoubleBeep,-2);

	CmosIntAttribute	StoreNumber( STORE_NUMBER );
	CmosIntAttribute	TerminalNumber( TERMINAL_NUMBER );

    // GL 28Apr09 Phase 11. Set the price input defaults to return blank if the user doesnt enter anything
    // The default (0) would currently return 000 which messes with InputNoInputEvent etc
    // GL 28Apr09 This doesnt appear to be working as were always getting 000 regardless of what we set here
    // extern DecimalInputOptions  DIOPrice;
    // DIOPrice._NoEntry = 2;
    extern int CustomErrorWindow;
    CustomErrorWindow = I_ERROR_WIN;			    // GL 28Apr09 Phase 11 VPOSD Migration

    // GL 05Mar03 - Initiate tracing
    // GL 23Jan04 - moved to swconfig.cpp
//  Trace.SetLevel( C_POSTRACE_DEFAULT_LEV );                           // '0'
//  Trace.SetFileOptions( C_POSTRACE_FILE_NAME, C_POSTRACE_MAX_FILE );
//  Trace.SetMsgBoxLevel( "801" );

    // broadcast on default port 1229 for now
	char szTermNo[ C_SmallStringLength ];
	TerminalNumber.Read();
	sprintf( szTermNo, "%04d", TerminalNumber.Get() );
    Trace.SetStatusMessengerOptions( "0", szTermNo );      

	Trace.Trace( TLvl_Verbose, "------------------------- ApplicationStart() -------------------------" );
	RegisterApplication();
	DisablePowerSwitch();

    Trace.Trace( TLvl_Verbose, "StartProcessControl(): Starting COM interface." );
	CoInitialize( NULL );
	
    Trace.Trace( TLvl_Verbose, "Initializing FLTranInfo _storeno and _termno for initial Sign On!" );
	StoreNumber.Read();
	FLTranInfo._storeno = StoreNumber.Get();
	TerminalNumber.Read();
	FLTranInfo._termno = TerminalNumber.Get();
	OfflineTryToReconnect.Reset();	// SQA 186 - Set when transaction completes
	AppFirstLoad.Reset();			// Set when application first records load

	Trace.Trace( TLvl_Verbose, "Initializing CMOS Settings ... Training Mode" );
	extern CmosFlag TrainingMode;
	TrainingMode.Initialize();

	// DS 23Feb09 - SQA353 & 417  This records if we are in training mode (defined above) and if we can talk to the
	// SmartSwitch training server. 
	Trace.Trace( TLvl_Verbose, "Initializing CMOS Settings ... Central Server Alert Sent" );
	extern CmosFlag CentralServerAlertSent;
	CentralServerAlertSent.Initialize();

// DS 30Mar09 - SQA450  This records if we have sent an alert to the central server for a SmartSwitch or Solve failure. 
	Trace.Trace( TLvl_Verbose, "Initializing CMOS Settings ... Training Host Available" );
	extern CmosFlag TrainingHostAvailable;
	TrainingHostAvailable.Initialize();

	// Reentry Mode - KH
	Trace.Trace( TLvl_Verbose, "Initializing CMOS Settings ... Reentry Mode" );
	extern CmosFlag ReentryMode;
	ReentryMode.Initialize();	

	Trace.Trace( TLvl_Verbose, "Initializing CMOS Settings ... Service Desk Mode" );
	extern CmosFlag ServiceDeskMode;
	ServiceDeskMode.Initialize();
	
	Trace.Trace( TLvl_Verbose, "Initializing CMOS Settings ... Terminal Open" );
	extern CmosFlag TerminalOpen;
	TerminalOpen.Initialize();

	Trace.Trace( TLvl_Verbose, "Initializing CMOS Settings ... Cashier Mode" );
	extern CmosFlag CashierMode;
	CashierMode.Initialize();
	CashierMode.Reset();

	// GL 10Mar10 SQA 195 (Issue 206737) Must initialise this CMOSFlag as well!!
	Trace.Trace( TLvl_Verbose, "Initializing CMOS Settings ... Float Xmit Required" );
	extern CmosFlag FloatXmitRequiredFlag;
	FloatXmitRequiredFlag.Initialize();				// Dont reset here !!!

	Trace.Trace( TLvl_Verbose, "Setting Transaction Defaults ..." );
	Transaction::SetDefaults();
	
	Trace.Trace( TLvl_Verbose, "Setting Force Close Callback Function ... ForceCloseTerminal()" );
	extern void ForceCloseTerminal();
    // GL 31Mar09 VPOSD 2008.09.08 Migration
	// SetForcedStoreClose(ForceCloseTerminal);
	g_FlibCallbacks.m_fnForcedStoreClosureCallback = &ForceCloseTerminal;

    // GL 20Sep13 DEBEN-191 From Curtiss Monroe
    // register a callback to handle listbox with multiple lines
    g_FlibCallbacks.m_fnListBoxFocusedItemChangedEventCallback = &ListboxItemChangedEventCallback;


//	extern CmosFlag ServiceDeskMode;
//	ServiceDeskMode.Reset();
	
	Trace.Trace( TLvl_Verbose, "Setting WinInitFunc ... WinInitFunc()" );
    // GL 31Mar09 VPOSD 2008.09.08 Migration
	// SetWinInitFunc(WinInitFunc); 
    g_FlibCallbacks.m_fnSystemVariableInitialization = &WinInitFunc;

	// SQA 488 - see above
	Trace.Trace( TLvl_Verbose, "Initializing Error Tone" );
    // GL 31Mar09 VPOSD 2008.09.08 Migration
	// FLBeepFunction = NCRDisplayErrorBeep;
    g_FlibCallbacks.m_fnBeepFunctionCallback = &NCRDisplayErrorBeep;

#ifdef SM4AS
	Trace.Trace( TLvl_Verbose, "Store Minder ... Initialising Event Log" );

	char szTermNo[ SM4AS_MISC_STRING_MAX_LEN ];
	sprintf( szTermNo, "%d", FLTranInfo._termno );
	SM4AS_EventLogInit( "", szTermNo );		// Opens the Event Log

	// Initialise tallies
	if( SUCCEEDED( SM4AS_TallyInit( SM4AS_TALLY_SOURCE )))
	{
		SM4AS_TallyCreate( SM4AS_TALLY_S_KEYLOCK_FLICKS, &TallySKeyFlick );
		SM4AS_TallyReset( TallySKeyFlick );
		SM4AS_TallyCreate( SM4AS_TALLY_N_KEYLOCK_FLICKS, &TallyNKeyFlick );
		SM4AS_TallyReset( TallyNKeyFlick );
	}
#endif

	Trace.Printf( TLvl_Normal, "%s", POS_RELEASE_NUMBER_STR );
	Trace.Printf( TLvl_Normal, "%s", POS_RELEASE_DATE_STR );

    // KH - 4.8.4+ Designer changes - THIS IS NOW HANDLED BY OPOS IN THE HWCONFIG.INI
#if 0
    Trace.Trace( TLvl_Verbose, "Setting 7156 Error Messages ... " );
	// Customize the 7156 serial printer errors.
	if( FLPrinter )
	{
		FLPrinter->SetErrorMessage( P7156_INSERT_SLIP_ERROR,INSERT_SLIP_ERROR );
//		FLPrinter->SetErrorMessage( P7156_REMOVE_SLIP_ERROR,REMOVE_SLIP_ERROR );
		FLPrinter->SetErrorMessage( P7156_MOTOR_JAM_ERROR,PRINTER_MOTOR_JAM_ERROR );
		FLPrinter->SetErrorMessage( P7156_PAPER_OUT_ERROR,PAPER_OUT_ERROR );
		FLPrinter->SetErrorMessage( P7156_DOOR_OPEN_ERROR,PRINTER_DOOR_OPEN_ERROR );
		FLPrinter->SetErrorMessage( P7156_COMM_ERROR,PRINTER_COMM_ERROR );
	}
#endif

	if( FLPrinter )
	{
		char A_TCPIPNumberStr[ 20 ];
		char buffer[ C_MediumStringLength ];
		char L_SystemDate[ C_SystemDateStrLength ];
		char L_SystemTime[ C_SystemTimeStrLength ];
		char A_CurrDateStr[ C_SystemDateStrLength ];
		char A_CurrTimeStr[ C_SystemTimeStrLength ];

        // GL SQA 64 - P2 - Bug with WriteEJTmp that messes with buffer.
        // Removed multiple blanks in buffer
        memset( buffer, '\0', C_MediumStringLength );
        memset( A_TCPIPNumberStr, '\0', 20 );
		DetermineTCPIPNumber( A_TCPIPNumberStr );
		
		// NT 17June03 - P7 - Writing IP Address in the NCRHelp.ini file
		WritePrivateProfileString( C_EP2KHelpMainSection, 
       						       C_EP2KHelpIPaddressKey,
	    					       A_TCPIPNumberStr,
		    				       C_EP2KHelpIniFilename );			
		GetSystemDate( L_SystemDate );
		DateDisplay L_Date( L_SystemDate );
		L_Date.Display( A_CurrDateStr, 8 );

		GetSystemTime( L_SystemTime );
		ShortTimeDisplay L_Time( L_SystemTime );
		L_Time.Display( A_CurrTimeStr, 8 );

		// KH - 4.8.4+ Designer changes
        // new with the OPOS Printer Modules and new FLIB
		// required of all direct printer writing - KB 19/05/00
		//sprintf(buffer, "\n\n\n\n\n");
        FLPrinter->StartReceipt( RECEIPT, FALSE );
		FLPrinter->MovePaper( RECEIPT, 5 );
		
		// GL 8Mar01 - Attempt to clear didgy characters
		sprintf( buffer, "\n\n" );
		FLPrinter->Write( RECEIPT, buffer, strlen( buffer ) );

		// GL SQA 1160 - 20DEC00 - Remove this extra call which may be producing duff characters
        //FLPrinter->Write(RECEIPT,buffer,strlen(buffer));
		sprintf( buffer, "Release Number : %s\n", POS_RELEASE_NUMBER_STR );
        WriteEJTmpFile( buffer, strlen( buffer ) );
		FLPrinter->Write( RECEIPT, buffer, strlen( buffer ) );
		sprintf( buffer, "Release Date : %s\n", POS_RELEASE_DATE_STR );
        WriteEJTmpFile( buffer, strlen( buffer ) );
		FLPrinter->Write( RECEIPT,buffer,strlen( buffer ) );
		sprintf( buffer, "Store No : %04ld", FLTranInfo._storeno );

        // KH - ROI_Store
        // Append the store type - If the parameter exists in the .ini file 0/1 or still -1 if not found
        char ROIbuffer[ C_MaxStringLength ];
        int StoreType = -1;

        memset( ROIbuffer, '\0', C_MaxStringLength );
    
        GetPrivateProfileString( C_ROI_STORE_MAIN_SECTION, 
						         C_ROI_STORE_ROI_STORE, 
						         "-1", 
						         ROIbuffer,
                                 C_MaxStringLength,
						         C_ROI_STORE_POSPARAMINI_FILE_NAME );

        // Allow for any combination of "true/false" + 1/0 to establish existance and setting of parameter. 
        if( stricmp(ROIbuffer, "true") == 0 )
            StoreType = 1; // ROI
        else if( stricmp(ROIbuffer, "false") == 0 )
            StoreType = 0; // UK Store
        else
	        StoreType = atoi( ROIbuffer ); // Should be either -1, 0 or 1 

        // At this stage we may not have a parameter file, therefore, display no store type.
        if( StoreType == 0 || StoreType == 1  ) // Erroneous values treated as (-1) no parameter found.
        {
            if( StoreType == 0 )
                strcat( buffer, " (UK store)" );
            else
                strcat( buffer, " (ROI store)" ); 
        }

		Trace.Printf( TLvl_Normal, "%s", buffer );		// DS 19Apr12 - Added so we can check where a trace is from
        WriteEJTmpFile( buffer, strlen( buffer ) );
		FLPrinter->Write( RECEIPT, buffer, strlen( buffer ) );
		sprintf(buffer, "Terminal No : %04d", FLTranInfo._termno);
		Trace.Printf( TLvl_Normal, "%s", buffer );		// DS 19Apr12 - Added so we can check where a trace is from
        WriteEJTmpFile( buffer, strlen( buffer ) );
		FLPrinter->Write( RECEIPT, buffer, strlen( buffer ) );
		sprintf(buffer, "Till Date : %s", A_CurrDateStr);
		Trace.Printf( TLvl_Normal, "%s", buffer );		// DS 19Apr12 - Added so we can check where a trace is from
        WriteEJTmpFile( buffer, strlen( buffer ) );
		FLPrinter->Write( RECEIPT, buffer, strlen( buffer ) );
		sprintf(buffer, "Till Time : %s", A_CurrTimeStr);
		Trace.Printf( TLvl_Normal, "%s", buffer );		// DS 19Apr12 - Added so we can check where a trace is from
        WriteEJTmpFile( buffer, strlen( buffer ) );
		FLPrinter->Write( RECEIPT, buffer, strlen( buffer ) );
		sprintf(buffer, "TCP/IP Number : %s", A_TCPIPNumberStr);
		Trace.Printf( TLvl_Normal, "%s", buffer );		// DS 19Apr12 - Added so we can check where a trace is from
        WriteEJTmpFile( buffer, strlen( buffer ) );
		FLPrinter->Write( RECEIPT, buffer, strlen( buffer ) );
        if (ReentryMode.IsSet())
        {
            memset( buffer, '\0', C_MediumStringLength );
		    sprintf( buffer, " **TILL RE-ENTRY MODE ACTIVE**" );
			Trace.Printf( TLvl_Normal, "%s", buffer );		// DS 19Apr12 - Added so we can check where a trace is from
            WriteEJTmpFile( buffer, strlen( buffer ) );
		    FLPrinter->Write( RECEIPT, buffer, strlen( buffer ) );
        }

		// KH - 4.8.4+ Designer changes
        //memset( buffer, '\0', C_MediumStringLength );
		//sprintf(buffer, "\n\n\n\n\n\n\n\n\n\n\n");
		//FLPrinter->Write(RECEIPT,buffer,strlen(buffer));
        FLPrinter->MovePaper( RECEIPT, 10 );        
        
        FLPrinter->CutReceipt();
        memset( buffer, '\0', C_MediumStringLength );
		sprintf( buffer, "\n\n\n" );
        WriteEJTmpFile( buffer, strlen( buffer ) );

		// KH - 4.8.4+ Designer changes
        FLPrinter->ReceiptDone();
	}

	// DS 26Jun12 - Section added to write data to the registry.
	long				lRes = 0;
	unsigned long		plDisposition;	
	HKEY				phkRes;

	lRes = RegCreateKeyEx( HKEY_LOCAL_MACHINE, 
					   C_APP_SUB_KEY, 
					   0, 
					   "POS 2000", 
					   REG_OPTION_NON_VOLATILE, 
					   KEY_ALL_ACCESS, 
					   NULL, 
					   &phkRes, 
					   &plDisposition );

	if( lRes == ERROR_SUCCESS )
	{
		long	lLen = 0;
		unsigned char	sTerminal[10];
		unsigned char	sStore[10];
		unsigned char	sPOSMode[20];
		unsigned char	sTCPIPNumber[ 20 ];
		HOSTENT *host;
		byte	hex_ip_address[ 16 ];
		char	ip_address[ 20 ];
		char	szIP[ 4 ];
		unsigned int utmp;                                                 

		memset( sTerminal, '\0', 10 );
		memset( sStore, '\0', 10 );
		memset( sPOSMode, '\0', 20 );
		memset( sTCPIPNumber, '\0', 20 );
		memset( hex_ip_address, '\0', 16 );                      
		memset( ip_address, '\0', 20 );                      
		memset( szIP, '\0', 4 );                      

		sprintf( (char *)sTerminal, "%04d", FLTranInfo._termno ); 
		lLen = strlen( (char *)sTerminal ) + 1;

		lRes = RegSetValueEx( phkRes, 
							  "Terminal", 
							  0, 
							  REG_SZ, 
							  sTerminal, 
							  lLen );

		sprintf( (char *)sStore, "%04d", FLTranInfo._storeno ); 
		lLen = strlen( (char *)sStore ) + 1;

		lRes = RegSetValueEx( phkRes, 
							  "Store", 
							  0, 
							  REG_SZ, 
							  sStore, 
							  lLen );

		if( g_KioskMode == true )
		{
			strcpy ( (char *)sPOSMode, "Kiosk" );
		}
		else
		{
            // GL 15Feb13 MobileApp
            if( g_MobileAppServerMode == true )
            {
            	strcpy ( (char *)sPOSMode, "MobileAppServer" );
		    }
			else if( IsFoodRegister() )
			{
				strcpy ( (char *)sPOSMode, "Touchscreen Food" );
			}
			else
			{
				if( IsTouchRegister() )
				{
					strcpy ( (char *)sPOSMode, "Touchscreen GM" );
				}
				else
				{
					strcpy ( (char *)sPOSMode, "Dynakey GM" );
				}
			}
		}

		lLen = strlen( (char *)sPOSMode ) + 1;

		lRes = RegSetValueEx( phkRes, 
							  "POS Mode", 
							  0, 
							  REG_SZ, 
							  sPOSMode, 
							  lLen );

		host = gethostbyname( "primary" );

		if( host != NULL )                                        
		{
			memcpy( hex_ip_address, host->h_addr, 4 );

			memset( ip_address, '\0', 20 );

			for( int i = 0; i < 4; i++ )
			{
				utmp = (unsigned char) hex_ip_address[ i ];
				
				sprintf( szIP, "%d",  utmp );

				strcat( ip_address, szIP ); 

				if( i < 3 )
					strcat( ip_address, "." );
			}

    		strcpy( (char *)sTCPIPNumber, ip_address );
		}
		else
    		strcpy( (char *)sTCPIPNumber, "<UNKNOWN>" );

		lLen = strlen( (char *)sTCPIPNumber ) + 1;

		lRes = RegSetValueEx( phkRes, 
							  "Server IP", 
							  0, 
							  REG_SZ, 
							  sTCPIPNumber, 
							  lLen );
	}	

	// DS 26Jun12 - End of section.


	// GP 23Sept14 Changes for ComplexConcessions
    ComplexConcessionsRegister = FALSE;
	char szTemp[ C_EnableDisableMaxStrLen ];
    memset( szTemp, '\0', C_EnableDisableMaxStrLen );
	GetPrivateProfileString( C_EnableDisableComplexConcessionsSection, C_EnableDisableItemEnabledKey,
        C_EnableDisableUnknown, szTemp, C_EnableDisableMaxStrLen, C_AppMenuIniFilename );

	if( (stricmp( szTemp, "yes" ) == 0) || (stricmp( szTemp, "true" ) == 0) )
		ComplexConcessionsRegister = TRUE;

	Trace.Trace( TLvl_Verbose, "Checking the command line arguments ... " );
	// check for Command Line Arguments (PC Mode)
	extern char *__command_line;  // declared in the tools
	BOOL _DoSysCredits = FALSE;

	if( __command_line )
	{
		unsigned int iCurPos = 0;
		char *lpCurPos;
		BOOL done  = FALSE;
		unsigned int iCmdLineLen = strlen( __command_line );
		lpCurPos = __command_line;

		done = ( iCmdLineLen > 0 ) ? FALSE : TRUE;

		while( !done )
		{
			if( iCurPos + strlen(lpCurPos) <= iCmdLineLen )
			{
				if( *lpCurPos == '/' || *lpCurPos == '-' )
				{
					if( (strnicmp(lpCurPos, "/SB", strlen("/SB")) == 0) ||
						(strnicmp(lpCurPos, "-SB", strlen("-SB")) == 0) )
					{
						_SaveBarcodeLookups = TRUE;
						lpCurPos += strlen("-SB");
						iCurPos += strlen("-SB");
					}
					else if ((strnicmp(lpCurPos, "/SX", strlen("/SX")) == 0) ||
						(strnicmp(lpCurPos, "-SX", strlen("-SX")) == 0))
					{
						lpCurPos += strlen("/SX");
						iCurPos += strlen("/SX");
						_DoSysCredits = TRUE;
					}
					else if ((strnicmp(lpCurPos, "/S", strlen("/S")) == 0) ||
						(strnicmp(lpCurPos, "-S", strlen("-S")) == 0))
					{
						_Supervisor_Mode = TRUE;
						_PC_Mode = TRUE;
						lpCurPos += strlen("/S");
						iCurPos += strlen("/S");
					}
					else if ((strnicmp(lpCurPos, "/NC", strlen("/NC")) == 0) ||
						(strnicmp(lpCurPos, "-NC", strlen("-NC")) == 0))
					{
						_PC_Mode = TRUE;
						lpCurPos += strlen("/NC");
						iCurPos += strlen("/NC");

						if (cashdrawer != NULL)
						{
							RemoveDevice(cashdrawer);
							delete cashdrawer;
							cashdrawer = NULL;
						}
					}
					else
					{
						lpCurPos += 1;
						iCurPos += 1;
					}
				}
				else
				{
					lpCurPos += 1;
					iCurPos += 1;
				}
			}
			else
			{
				done = TRUE;
			}
		}
	}

//	if( _DoSysCredits == TRUE )
//		DoSysCredits();

	Trace.Trace( TLvl_Verbose, "Refreshing System Variables ... RefreshSystemVariables()" );
	RefreshSystemVariables();

    // GL 20Jul04 - No sig capture
	// Trace.Trace( TLvl_Verbose, "Starting the Signature Capture ... StartSigCap()" );
	// StartSigCap();

	Trace.Trace( TLvl_Verbose, "Checking for Remote Customer Display ... DoesRemoteDisplayExist()" );
	if( DoesRemoteDisplayExist() )
	{
		Trace.Trace( TLvl_Verbose, "Device detected on customer display port" );

		// SQA 244
		ClearRemoteDisplayLine( 1 );
		ClearRemoteDisplayLine( 2 );
	}
	else
		Trace.Trace( TLvl_Verbose, "Remote Display NOT attached or NOT functioning." );

	Trace.Trace( TLvl_Verbose, "Getting the current working directory for the POS2000.INI file" );
	// char *_getcwd( char *buffer, int maxlen );
	char buffer[C_MaxStringLength];
	if (_getcwd(buffer, C_MaxStringLength) != NULL)
	{
		if (buffer[strlen(buffer)-1] != '\\')
			strcat(buffer, "\\");
		
		strcat(buffer, C_POS2000INIFilenameNoPath);
	}
	Trace.Trace( TLvl_Verbose, buffer );
	Trace.Trace( TLvl_Verbose, "Setting the POS2000.INI FIlename ... " );
	FileParameters.SetFilename(buffer);

    Trace.Trace( TLvl_Verbose, "Create Application Monitor" );
    if( EP2KAppMon )
    {
        delete EP2KAppMon;
        EP2KAppMon = NULL;
    }

  	// DS May11 - KIOSK CHIP & PIN
    // GL 15Feb13 Note that MobileApp may still need the AppMonitor to send info to NN4M
	if( g_KioskMode == false )      // && g_MobileAppServerMode == false )
	{
		EP2KAppMon = new EPOS2000AppMonitor;
	}

    Trace.Trace( TLvl_Verbose, "Create Help Subsystem" );
    if( EP2KHelp )
    {
        delete EP2KHelp;
        EP2KHelp = NULL;
    }

    // DS May11 - KIOSK CHIP & PIN
    // GL 15Feb13 MobileApp
	if( g_KioskMode == false && g_MobileAppServerMode == false )
	{
		EP2KHelp = new EPOS2000Help;
	}

    if( LoyaltyMgr )
    {
        delete LoyaltyMgr;
        LoyaltyMgr = NULL;
    }
    LoyaltyMgr = new LoyaltyManager();
    LoyaltyMgr->SetLoyaltyDeptReReadInterval( 1, 1 );         // For now force to re-read every trans

    // GL 24Jun04 Phase 7.7 Nectar 
    if( VoucherMgr )
    {
        delete VoucherMgr;
        VoucherMgr = NULL;
    }
    VoucherMgr = new VoucherManager();

    if( ComplexConcessionsMgr )
    {
        delete ComplexConcessionsMgr;
        ComplexConcessionsMgr = NULL;
    }
    ComplexConcessionsMgr = new ComplexConcessionsManager();

    // GL 14Apr05 Phase 8.2.5
    if( ReturnsMgr )
    {
        delete ReturnsMgr;
        ReturnsMgr = NULL;
    }
    ReturnsMgr = new ReturnsManager();

    // GL 01Feb06 Phase 8.5
    if( CustomerDataCaptureMgr )
    {
        delete CustomerDataCaptureMgr;
        CustomerDataCaptureMgr = NULL;
    }
    CustomerDataCaptureMgr = new CDCManager();

    if( EAppsMgr )
    {
        delete EAppsMgr;
        EAppsMgr = NULL;
    }

	// DS May11 - KIOSK CHIP & PIN
    // GL 15Feb13 MobileApp
	if( g_KioskMode == false || g_MobileAppServerMode == false )
	{
		EAppsMgr = new EAppsManager;
	}

    // GL 05Mar06 Phase 8.6
    if( CentralReturnsMgr )
    {
        delete CentralReturnsMgr;
        CentralReturnsMgr = NULL;
    }

    CentralReturnsMgr = new CRManager();
    CentralReturnsMgr->Initialise();

	// GL 06Jul10 Gift Card Mall Manager Phase 12.0
    if( GiftCardMallMgr )
    {
        delete GiftCardMallMgr;
        GiftCardMallMgr = NULL;
    }
    
    GiftCardMallMgr = new GiftCardMallManager();

    // GL 11Jan11 Promotions
    if( promotionsMgr )
    {
        delete promotionsMgr;
        promotionsMgr = NULL;
    }

    promotionsMgr = new PromotionsManager();

	// DS 13Aug07 - SQA 3492 - Read the timeout time in milliseconds
	// Note this has been updated to set the default to zero which will disable the function
	// so that it will only be active if they have set the parameter.
    char repeat_buffer[ C_MaxStringLength ];
    memset( repeat_buffer, '\0', C_MaxStringLength );

	GetPrivateProfileString( "Repeat Key", 
                             "Timeout", 
                             "0", 
                             repeat_buffer, 
                             C_MaxStringLength, 
                             C_POSCONFIG_INI_FILENAME );

	if( ValidateAsNumeric( repeat_buffer, false, false ) == true )
		PrePostRepeatKeyTimeout = atol( repeat_buffer );
	else
		PrePostRepeatKeyTimeout = 0;

    //GL - 27Apr00 - Start any external applications - see posparam.ini
    StartExternalApplications();

 	// DS May11 - KIOSK CHIP & PIN
    // GL 15Feb13 MobileApp
	if( g_KioskMode == false && g_MobileAppServerMode == false )
	{
		// AG 03Sep03 - Must be done after external applications have been launched.
		if( !NonPOSMgr )
		{
			NonPOSMgr= new NonPOSManager;
		}

		if( NonPOSMgr )
		{
			NonPOSMgr->Init();
		}
	}

	debSrvcMgr= new DebServiceManager();	// CM - TLG Software Mgmt
	debSrvcMgr->StartTimer_DebServiceTimer();

    // GL VPOSD 4.9.1 TCPSERVER - Start RCSTCPDiag interface for Offline indicator
    // GL 31Mar09 VPOSD 2008.09.08 Migration No longer required
#ifdef TCPSERVER
	if( rcstcp )
		rcstcp->QueryInterface( IID_IRcsTcpDiag, (void **)&rcstcpdiag );
#endif

	// KH - 18/05/2004 - XP Issues 2/3 - Set the background grey to original colour...
	// Window elements to change. 
	// int aiElements[1] = {COLOR_ACTIVEBORDER}; 

   	// DS 3Jun09 Phase 11 
	// Set the COLOR_BTNFACE
	int aiElements[1] = {COLOR_BTNFACE}; 

	// Array of RGB values. 
	DWORD aColors[1]; 
 
	// Define the new colors. 
	aColors[0] = RGB(0xC0, 0xC0, 0xC0);  // Original grey 
 
	// Set the window elements in aiElements to the colors specified in aColors. 
	SetSysColors(1, aiElements, aColors);
	//KH - End colour change...

    //KEH - Added 7-8-09 - new key management for encryption

	BOOL loadedEncryption = ReloadEncFiles();

	// KH 30Dec10 SQA 31 PHASE 14 - Set this new field accordingly...
	if( loadedEncryption == TRUE )
		bEncryptionFileOK = TRUE;
	else
		bEncryptionFileOK = FALSE;

	Trace.Trace( TLvl_Verbose, "Starting Process Control ... StartProcessControl()" );
	StartProcessControl();           // Run the application  
	Trace.Trace( TLvl_Verbose, "Starting Process Control ... StartProcessControl() -> COMPLETE" );

	// GL VPOSD 4.9.1 TCPSERVER - Stop RCSTCPDiag interface
    // GL 31Mar09 VPOSD 2008.09.08 Migration No longer required
#ifdef TCPSERVER
	if( rcstcpdiag )
	{
    	Trace.Trace( TLvl_Verbose, "TCPServer -> Release" );
		// GL 11May09 Phase 11
        // rcstcpdiag->Release();
		rcstcpdiag = NULL;
    	Trace.Trace( TLvl_Verbose, "TCPServer -> Release - DONE" );
	}
#endif

    // AG 03Sep03 - Must be done before external applications are closed.
    if( NonPOSMgr )
    {
    	Trace.Trace( TLvl_Verbose, "NonPOSMgr->Shutdown" );
        NonPOSMgr->Shutdown();
    	Trace.Trace( TLvl_Verbose, "NonPOSMgr->Shutdown - DONE" );
    }

    //GL - 27Apr00 - stop any running applications that we started
    StopExternalApplications();
	
	// GL Backup any data from ejtmp.pos written to w/o a transaction
	EJBackup();

    // GL 20Jul04 - No Sig capture
	// Trace.Trace( TLvl_Verbose, "Stopping the Signature Capture ... StopSigCap()" );
	// StopSigCap();

    // GL 31Mar09 VPOSD 2008.09.08 Migration No longer need to do this
	// SetWinInitFunc();
	
#ifdef SM4AS
	// Close the Store Minder Event Log
	Trace.Trace( TLvl_Verbose, "Store Minder ... Closing the event log" );
	SM4AS_EventLogClose();

	// Close the Tally system
	SM4AS_TallyClose();
#endif

	// clear the remote display when the app exits.
	Trace.Trace( TLvl_Verbose, "Clearing the remote display ... ClearRemoteDisplay()" );
	ClearRemoteDisplay();

    // Clean up the Application Monitor
	Trace.Trace( TLvl_Verbose, "Stopping Application Monitor" );
    if( EP2KAppMon )
    {
        delete EP2KAppMon;
        EP2KAppMon = NULL;
    }

	Trace.Trace( TLvl_Verbose, "Stopping POS Help" );
    // Clean up the Help Subsystem
    if( EP2KHelp )
    {
        delete EP2KHelp;
        EP2KHelp = NULL;
    }

    // Clean up the loyalty manager
	Trace.Trace( TLvl_Verbose, "Cleaning up Loyalty Manager" );
    if( LoyaltyMgr )
    {
        delete LoyaltyMgr;
        LoyaltyMgr = NULL;
    }

    Trace.Trace( TLvl_Verbose, "Cleaning up Voucher Manager" );
    if( VoucherMgr )
    {
        delete VoucherMgr;
        VoucherMgr = NULL;
    }

    Trace.Trace( TLvl_Verbose, "Cleaning up Concessions Manager" );
    if( ComplexConcessionsMgr )
    {
        delete ComplexConcessionsMgr;
        ComplexConcessionsMgr = NULL;
    }

    Trace.Trace( TLvl_Verbose, "Cleaning up Returns Manager" );
    if( ReturnsMgr )
    {
        delete ReturnsMgr;
        ReturnsMgr = NULL;
    }

	// GL 01Feb06 Phase 8.5
    Trace.Trace( TLvl_Verbose, "Cleaning up Customer Details Capture Manager" );
    if( CustomerDataCaptureMgr )
    {
        delete CustomerDataCaptureMgr;
        CustomerDataCaptureMgr = NULL;
    }

    // GL 02Sep03 Clean up
	Trace.Trace( TLvl_Verbose, "Cleaning up EApps Manager" );
    if( EAppsMgr )
    {
        delete EAppsMgr;
        EAppsMgr = NULL;
    }

    Trace.Trace( TLvl_Verbose, "Cleaning up Gift Card Mall Manager" );
    if( GiftCardMallMgr )
    {
        delete GiftCardMallMgr;
        GiftCardMallMgr = NULL;
    }

	// GL 01Feb06 Phase 8.6
    Trace.Trace( TLvl_Verbose, "Cleaning up Central Returns Manager" );
    if( CentralReturnsMgr )
    {
        delete CentralReturnsMgr;
        CentralReturnsMgr = NULL;
    }

    // GL 11Jan11 Phase 14.5 Promotions
    Trace.Trace( TLvl_Verbose, "Cleaning up Promotions Manager" );
    if( promotionsMgr )
    {
        delete promotionsMgr;
        promotionsMgr = NULL;
    }

    // GL 26Aug08 Cleanup winsock
    Trace.Trace( TLvl_Verbose, "Cleaning up Winsock" );
    WSACleanup();

    Trace.Trace( TLvl_Verbose, "StartProcessControl(): Stopping COM interface." );
	CoUninitialize();

	// GL - BOUNDS CHECKER - Memory leak
	if( CurrentAppPage )
	{
		delete CurrentAppPage;
		CurrentAppPage = NULL;
	}

	if(debSrvcMgr)
	{	// CM - TLG Software Mgmt
		delete debSrvcMgr;
		debSrvcMgr= NULL;
	}

	// DS 2Apr12 - SQA 193 - Loss of Focus Issue
	if ( g_AppHideActive == true )
	{
		if (strlen( g_AppTitle ) && g_AppHandle )
		{
			ShowWindow (g_AppHandle, SW_SHOWNORMAL);
		}
	}
	// DS 2Apr12 - SQA 193 - Loss of Focus Issue - END

	Trace.Trace( TLvl_Verbose, "------------------------- ApplicationStop() --------------------------" );

    // GL 05Mar03 - Destroy tracing
    Trace.Close();
}

void SCPrintMsg( const char *msg )
{
	FLPrinter->Write( RECEIPT, msg, strlen( msg ) );
}

// NT 1July03 - P7 - Need to return Error condition
int SCPrintMsgSLIP( const char *msg )
{
	return FLPrinter->Write( SLIP, msg, strlen( msg ) );
}

static const TCHAR CheckFace2DefaultFormat[] = _T("L. 0=D:0;4:0. 20=1;36. 23=1-34. 26=1-30. ~ 23");
// Pointer to default format string.
extern LPCTSTR pCheckFace2Default;

// Maximum lines allowed on check.
#define MAX_CHECK_LINES 30
static void PCF2_Insert( CmString* psLines, int nRow, int nStartChar, CmString sInsert );



// Added function
// AG 25Sep02 - VPOSD Migration 
int TransPrintManager::PrintCheckFace2( LPCTSTR pFormat,
	LPCTSTR pPayee, Amount* const pAmount, LPCTSTR pMemo,
	Date_Display* pDate, int nInsertError, int nRemoveError )
{
	// Parse the format string and do a bit of validation.
	//{Rotation}. {DateRow}={MDType}:{MDCol};{YType}:{YCol}. {PayRow}={WhoCol};{AmtCol}. {TAmtRow}={TAmtStartCol}-{TAmtEndCol}. {MemoRow}={MemoStartCol}-{MemoEndCol}. ~ {AmtRow}
	TCHAR cRotation;
	int nDateRow; TCHAR cMDType; int nMDCol; TCHAR cYType; int nYCol;
	int nPayRow, nWhoCol, nAmtCol;
	int nTAmtRow, nTAmtStartCol, nTAmtEndCol;
	int nMemoRow = 25, nMemoStartCol = 119, nMemoEndCol = 120; // Set to maxes in case not present, to pass validation.
	int nAmtRow = -1;										   // Numeric amount row defaults to same as payee row, unless this is set >= 0.
	const int nWhoToAmt = 8;								   // Blank columns to leave between who and amount.

	// AG 25Sep02 - VPOSD Migration - Added this initialisation. pCheckFace2Default defined as extern.
	pCheckFace2Default = CheckFace2DefaultFormat;

	if( !FLPrinter )
		return 1;

	// If no format string passed in, then use default.
	if ( pFormat == 0 )
		pFormat = pCheckFace2Default;

	// Parse out all the fields. The first 12 are required.
	int nFields = _stscanf( pFormat, _T("%c.%u=%c:%u;%c:%u.%u=%u;%u.%u=%u-%u.%u=%u-%u. ~%u"),
		&cRotation,
		&nDateRow, &cMDType, &nMDCol, &cYType, &nYCol,
		&nPayRow, &nWhoCol, &nAmtCol,
		&nTAmtRow, &nTAmtStartCol, &nTAmtEndCol,
		&nMemoRow, &nMemoStartCol, &nMemoEndCol,
		&nAmtRow );
	if ( nFields < 12 )
		return 10;

	// Perform some field validation.
	if ( cRotation != 'N' && cRotation != 'R' && cRotation != 'L' && cRotation != 'U' ||
		 cMDType != 'M' && cMDType != 'D' ||
		 cYType != '2' && cYType != '4' )
		return 10;

	// If numeric amount row not specified, then use same as payee.
	if ( nAmtRow < 0 )
		nAmtRow = nPayRow;

	// Prepare the "variables".
	CmString sMonthDay, sYear, sWho, sAmount, sAmountText;
	int nIdxNL;

	//  - Month/Day and Year.
	if ( !pDate )
	{
		int nYear, nMonth, nDay;
		TCHAR pszDate[20];
		if ( ReadSystemDate(pszDate) < 0 )
			return 11;
		_stscanf( pszDate, _T("%4d%2d%2d"), &nYear, &nMonth, &nDay );

		if ( cMDType == 'M' )
			sMonthDay.Format( _T("%s %d"), GetMonth(nMonth), nDay );
		else
			sMonthDay.Format( _T("%d %s"), nDay, GetMonth(nMonth) );

		sYear.Format( _T("%.4d"), nYear );
		if ( cYType == '2' )
			sYear = sYear.Mid(3);
	}
	else
	{
		sMonthDay.SetAllocChars(200);
		pDate->Display( sMonthDay.GetBuffer(), sMonthDay.GetChars() );
	}

	//  - Amount (numeric).
	sAmount.SetAllocChars(200);
	pAmount->Display( sAmount.GetBuffer(), sAmount.GetChars() );

	//  - Amount (text).
	sAmountText.SetAllocChars(200);
	int nAmountTextSize =  nTAmtEndCol - nTAmtStartCol + 1;
	// AG 25Sep02 - VPOSD Migration 
	//MakeAmountText( *pAmount, sAmountText.GetBuffer(), nAmountTextSize );
	MakeAmountText( *pAmount, (LPTSTR) sAmountText.GetBuffer(), nAmountTextSize );


	// DS 29Feb08 - VPOSD UPGRADE
	// This is now always done.
	sMonthDay.SetAllocChars(200);
	CheckFace2Func( CF2F_MonthDay, sMonthDay.GetBuffer(), 200, 0 );
	sYear.SetAllocChars(200);
	CheckFace2Func( CF2F_Year, sYear.GetBuffer(), 200, 0 );
	//sAmount.SetAllocChars(200);		// Not needed. Already set above.
	CheckFace2Func( CF2F_AmountNum, sAmount.GetBuffer(), 200, *pAmount );
	//sAmountText.SetAllocChars(200);	// Not needed. Already set above.
	CheckFace2Func( CF2F_AmountText, sAmountText.GetBuffer(), nAmountTextSize, *pAmount );
	// DS 29Feb08 - VPOSD UPGRADE - END

	// Set up an array of strings to print.
	int nMaxLen;
	CmString sTmp;
	CmString sLines[MAX_CHECK_LINES + 1];
	//  - Month & day: Adjust string to formatted length. Insert into array.
	sMonthDay.SetChars( tstrlen(sMonthDay) );
	PCF2_Insert( sLines, nDateRow, nMDCol, sMonthDay );
	//  - Year: Adjust string to formatted length. Insert into array.
	sYear.SetChars( tstrlen(sYear) );
	PCF2_Insert( sLines, nDateRow, nYCol, sYear );
	//  - Payee: If on same line as payee, truncate payee if too long. Insert into array.
	sTmp = pPayee;
	if ( nPayRow == nAmtRow )
	{
		nMaxLen = nAmtCol - nWhoToAmt - nWhoCol;
		sTmp = sTmp.Left( nMaxLen );
	}
	PCF2_Insert( sLines, nPayRow, nWhoCol, sTmp );
	//  - Numeric amount: Adjust string to formatted length. Insert into array.
	sAmount.SetChars( tstrlen(sAmount) );
	PCF2_Insert( sLines, nAmtRow, nAmtCol, sAmount );
	//  - Text amount: Insert newline-separated lines into array. Truncate each if needed.
	sAmountText.SetChars( tstrlen(sAmountText) );
	nMaxLen = nTAmtEndCol + 1 - nTAmtStartCol;
	do
	{
		nIdxNL = sAmountText.Find('\n');				// Find first newline.
		if ( nIdxNL < 0 )								// If none (the most common case),
			nIdxNL = sAmountText.GetChars();			//   then use rest of line.
		PCF2_Insert( sLines, nTAmtRow,					// Insert into array line:
			nTAmtStartCol,								//   Start column.
			sAmountText.Left( min(nIdxNL, nMaxLen) ) ); //   Text (shorter of newline or maximum allowed length).
		sAmountText = sAmountText.Mid( nIdxNL + 1 );	// Remove the line just inserted from our string.
		nTAmtRow += ( cRotation == 'U' ) ? -1 : +1;		// Adjust row for next line. Go up if upside down, else down.
	} while ( sAmountText.NotEmpty() );					// Process till text amount all done.
	//  - Memo. Truncate memo if too long. Insert into array.
	sTmp = CmString(pMemo).Left( nMemoEndCol + 1 - nMemoStartCol );
	PCF2_Insert( sLines, nMemoRow, nMemoStartCol, sTmp );

	// Set specified rotation.
	TCHAR cPrevRotation = FLPrinter->GetRotation(SLIP);	  // Get current rotation.
	if ( 0 != FLPrinter->SetRotation( SLIP, cRotation ) ) // Set rotation for subsequent receipts.
		return 12;

	// Perform the printing.
	int nRC;

	//  - Set up error messages.
	BOOL bRestoreErrors = FALSE;
	int nOrigInsertError, nOrigRemoveError;
	if ( nInsertError < -1 || nRemoveError < -1 )
	{
		bRestoreErrors = TRUE;
		nOrigInsertError = FLPrinter->ErrorMessage(0);
		nOrigRemoveError = FLPrinter->ErrorMessage(1);
		FLPrinter->SetErrorMessage( 0, nInsertError );
		FLPrinter->SetErrorMessage( 1, nRemoveError );
	}

	//  - Start the slip.
	FLPrinter->StartReceipt(SLIP);

	//  - Determine number of lines to output.
	for ( int nLastLine = MAX_CHECK_LINES; --nLastLine >= 0 && sLines[nLastLine].IsEmpty(); )
		;

	//  - Output the lines.
	//     If upside down, then send in inverse order, since we want to print first line first
	//       (which will be last when reading the printed lines).
	for ( int nLine = 0; nLine <= nLastLine; nLine++ )
	{
		int n = ( cRotation == 'U' ) ? nLastLine - nLine : nLine;
		if ( sLines[n].TrimRight().IsEmpty() )						// Remove trailing whitespace. If empty,
			sLines[n] = " ";										//   set to a space. (FLIB needs at least a space to print the lines.)
		if ( ( nRC = FLPrinter->Write( SLIP, sLines[n] ) ) < 0 )	// Output the line.
			break;													//   If error, then end early.
	}

	//  - End the slip.
	int nRC2 = FLPrinter->SlipDone();
	if ( nRC >= 0 )
		nRC = nRC2;

	//  - Restore original error messages.
	if (bRestoreErrors)
	{
		FLPrinter->SetErrorMessage( 0, nOrigInsertError );
		FLPrinter->SetErrorMessage( 1, nOrigRemoveError );
	}

	// Restore original rotation.
	FLPrinter->SetRotation( SLIP, cPrevRotation );

	return nRC;
}



// AG 25Sep02 - VPOSD Migration - Added function
static void PCF2_Insert( CmString* psLines, int nRow, int nStartChar, CmString sInsert )
{
	if ( nRow < 1 || nRow > MAX_CHECK_LINES )	// Ensure that row is not too big or small.
		return;

	if ( sInsert.IsEmpty() )
		return;

	nStartChar--;								// Change start character from 1-based to 0-based.
	CmString* psLine = &psLines[nRow];			// Get pointer to the line to update.
	if ( psLine->GetChars() < nStartChar )		// If line is smaller than start column, then pad it with spaces.
		*psLine += CmString( (TCHAR) ' ', nStartChar - psLine->GetChars() );
	*psLine = psLine->Left( nStartChar ) +		// Set line to previous part before insertion point +
		sInsert +								//   the insertion string +
		psLine->Mid( nStartChar + sInsert.GetChars() ); // the previous part after insertion string.
}

// GL 01Apr06 SQA 2952 Centralise this call
void OpenCashDrawer()
{
    if( FLCashdrawer )
		FLCashdrawer->Write( "1", 1 );

    Trace.Trace( TLvl_Verbose, "!!CASH DRAWER OPEN!!" );
}

// GL 08Apr09 Phase 11 Migration
// Following from RCS Base application
void TraceOutputV( LPCTSTR szFormat, ... )
{
	if (!g_pTrace)
		return;

	const int PrintfMax = 4096;
	TCHAR buffer[PrintfMax + 1];
	tmemset(buffer, 0, PrintfMax + 1);

	// Use the "list of arguments" version of printf.
	va_list va;
	va_start( va, szFormat );
	_vsntprintf( buffer, PrintfMax, szFormat, va );
	va_end(va);
	buffer[PrintfMax] = '\0';

	TraceOutputL(TLvl_Verbose, buffer);
}

void TraceOutput( LPCTSTR szFormat, ... )
{
	if (!g_pTrace)
		return;

	const int PrintfMax = 4096;
	TCHAR buffer[PrintfMax + 1];
	tmemset(buffer, 0, PrintfMax + 1);

	// Use the "list of arguments" version of printf.
	va_list va;
	va_start( va, szFormat );
	_vsntprintf( buffer, PrintfMax, szFormat, va );
	va_end(va);
	buffer[PrintfMax] = '\0';

	TraceOutputL(TLvl_Normal, buffer);
}

void TraceOutputL( int nLevel, LPCTSTR szFormat, ... )
{
	if (!g_pTrace)
		return;

	const int PrintfMax = 4096;
	TCHAR buffer[PrintfMax + 1];
	tmemset(buffer, 0, PrintfMax + 1);

	// Use the "list of arguments" version of printf.
	va_list va;
	va_start( va, szFormat );
	_vsntprintf( buffer, PrintfMax, szFormat, va );
	va_end(va);
	buffer[PrintfMax] = '\0';

	g_pTrace->Printf( nLevel, buffer );
}


void TraceOutputL( int nLevel, LPCTSTR szFormat )
{
	if (!g_pTrace)
		return;

	g_pTrace->Printf( nLevel, szFormat );
}

void TraceDump( const void *pData, int nLength )
{
	if (!g_pTrace)
		return;

	g_pTrace->Dump(TLvl_Normal, pData, nLength);
}

void TraceDumpV( const void *pData, int nLength )
{
	if (!g_pTrace)
		return;

	g_pTrace->Dump(TLvl_Verbose, pData, nLength);
}


// CRM 25Sep13 DEBEN-191: Begin listbox handling updates.

// TRUE if scroll in progress, so that callback should do nothing.
BOOL ScrollInProgress = FALSE;

// Get number of lines currently in listbox.
int GetLinesInListbox( ListBox *pListBox )
{
	int nLBLastItemLine		 = pListBox->GetLineNo( pListBox->GetCount() ); // Get zero-based line# of last group.
	int nLBLastItemLineCount = pListBox->ItemLineCount( nLBLastItemLine );  // Get lines in last group.

	if( nLBLastItemLineCount > 0 )
		return nLBLastItemLine + nLBLastItemLineCount;

	return 0;
}

// Set during each call to GetItemForLine, to NULL or Merchandise Transaction.
MerchTransaction *MerchTran_ScrollGIFL = NULL;

// For specified line# (nLine), determine and set:
//   * associated item (nItem).
//   * item's number of lines (nLineCount).
//   * if line# (nItem) not first line of item, adjust.
//   * if caller wants item count, set it (*pItemCount).
//  -> Returns: TRUE if listbox has items and "nLine" >= 0.
//              FALSE if no items or "nLine" negative.

BOOL GetItemForLine( ListBox *pListBox, int &nLine, int &nLineCount, int &nItem, int *pItemCount /*= NULL*/ )
{
	MerchTran_ScrollGIFL = NULL;

	// If invalid line or no items, fail.
	int nLBItems = pListBox->GetCount();
	if( nLine < 0 || nLBItems <= 0 )
		return FALSE;

	// Find item# for the specified line#.
	nItem = pListBox->GetGroupNo( nLine );                    // Get the item associated with this line.
	if( nItem <= 0 )                                          // If invalid line#,
		nItem = nLBItems;                                   //   then assume too big and use last item.

	// Get item's line information.
	nLine	   = pListBox->GetLineNo( nItem );                // Get first line of item.
	nLineCount = pListBox->ItemLineCount( nLine );            // Get number of lines in item.
	if( pItemCount != NULL )                                  // If caller wants item count,
		*pItemCount = 1;                                      //   set to 1.

    // Special handling is needed for sale and central returns listbox if item is PRF Base or Charge.
	if( WorkingTransaction != NULL && WorkingTransaction->IsKindOf( E_MerchTransaction ) )
	{
        int nItemList = -1;

        // If sale listbox, then itemlist matches up with listbox.
        if ( pListBox == AccessListBox( L_SALE_DETAIL ) )
            nItemList = nItem;

        // If central returns listbox, then hidden items may precede the itemlist that matches up with listbox.
        else if ( pListBox == AccessListBox( L_CENTRAL_RETURNS ) )
            nItemList = nItem + CentralReturnsMgr->HiddenSaleItems;

        // If sale or central returns listbox...
        if ( nItemList >= 0 )
        {
		    // Get the item.
		    MerchTran_ScrollGIFL		= (MerchTransaction *)WorkingTransaction;
		    ItemMgrList      *ItemList	= (ItemMgrList *)( MerchTran_ScrollGIFL->ItemListAccess() );
		    Item             *pItem		= (Item *)ItemList->GetNth( nItemList );

		    // If present...
		    if( pItem != NULL )
		    {
			    // If item is PRF Base, then PRF Charge item follows it.
			    if( pItem->PRFBaseItem.IsSet() )
				{
					// Get next item's line count. If present (should be unless in process of being created)...
					int nNextLineCount = pListBox->ItemLineCount( nLine + nLineCount );
					if( nNextLineCount > 0 )
					{
						nLineCount += nNextLineCount;           // Add the number of lines in item to line count.
						if( pItemCount != NULL )                // If caller wants item count,
							*pItemCount = 2;                    //   set to 2.
					}
				}

			    // If item is PRF Charge, then PRF Base item precedes it.
			    else if( pItem->PRFChargeItem.IsSet() )
			    {
				    // Go to the previous item.
				    if( --nItem <= 0 )
					    return FALSE;

				    // Get previous item's line information.
				    nLine		= pListBox->GetLineNo( nItem );       // Get first line of item.
				    nLineCount += pListBox->ItemLineCount( nLine );   // Add the number of lines in item to line count.
				    if( pItemCount != NULL )                          // If caller wants item count,
					    *pItemCount = 2;                              //   set to 2.
			    }

		    }
        }
	}

	return TRUE;
}

// CRM 25Sep13 DEBEN-191
void SaleListboxAdjustments( ListBox *pListBox )
{
	// DEBEN-191: Final listbox focus, selection, and top line adjustments.
	//   In particular, the PRFBase or PRFCharge item must be visible in listbox.
	//     A problem was that, when listbox was full, it's primary item was at the bottom of the
	//     listbox, with the PRFCharge not seen.

	// If no listbox specified, use "sale detail".
	if( pListBox == NULL )
		pListBox = AccessListBox( L_SALE_DETAIL );

	// Do nothing if...
	//  - No listbox, or listbox is not "sale detail" or "central returns".
	if( pListBox == NULL ||
        ( AccessListBox( L_SALE_DETAIL ) != pListBox &&
          AccessListBox( L_CENTRAL_RETURNS ) != pListBox ) )
		return;

    //  - Scroll is in progress.
	if( ScrollInProgress )
		return;
	
    //  - Recursive call.
	static BOOL s_bRecurse = FALSE;
	if( s_bRecurse )
		return;
	
    s_bRecurse = TRUE;

	// Get listbox information.
	int nLBSize	   = pListBox->ListboxSize();                           // Get lines that fit in listbox.
	int nLBTopLine = pListBox->GetTopLine();                            // Get current top line of listbox.

	// Get item information.
	int nLine	   = pListBox->GetCurrentLine();                        // Get zero-based line# of current group.
	int nOrigLine  = nLine;                                             // Save line.
	int nLineCount, nItem, nItemCount;

    if( GetItemForLine( pListBox, nLine, nLineCount, nItem, &nItemCount ) ) // Find item and line info.
	{
		// If GetItemForLine says current line is not start of focused item, then change focus.
		if( nLine != nOrigLine )
			pListBox->MoveTo( nItem );

		// Handle select / deselect of PRF Base or Charge. (Minimize deselections, trying to minimize screen flicker.)
		static int nSelectedItem = 0;
		int nDeselectItem = nSelectedItem;
		nSelectedItem = 0;

		//  - If had selected a PRF Charge, then deselect.
        if( MerchTran_ScrollGIFL != NULL && MerchTran_ScrollGIFL->PRFHighlightedChargeItemNumber > 0 )
		{
			// If selected item doesn't match MerchTran's saved value,
			//   must have been selected outside this function. Deselect it now.
			if( nDeselectItem != MerchTran_ScrollGIFL->PRFHighlightedChargeItemNumber )
				pListBox->DeSelect( MerchTran_ScrollGIFL->PRFHighlightedChargeItemNumber );
			// Reset MerchTran's saved value.
			MerchTran_ScrollGIFL->PRFHighlightedChargeItemNumber = 0;
		}

		//  - If GetItemForLine says 2 items were combined, then:
		//		* 1st is a PRF Base item. It will be focused.
		//		* 2nd is a PRF Charge item. It needs to be selected.
		if( nItemCount > 1 )
		{
			int nItemToSelect = nItem + 1;
			
            // If have selected item that doesn't match the item to select, deselect it now.
			if( nDeselectItem != nItemToSelect )
            {
				pListBox->DeSelect( nDeselectItem );
            }
			
            // Select item and remember.
			pListBox->SelectItem( nItemToSelect );
			nSelectedItem = nItemToSelect;

			// Save in MerchTran.
			if( MerchTran_ScrollGIFL != NULL && MerchTran_ScrollGIFL->PRFHighlightedChargeItemNumber > 0 )
				MerchTran_ScrollGIFL->PRFHighlightedChargeItemNumber = nItemToSelect;
		}
		else if( nDeselectItem != 0 )
        {
            //  - If don't need to select PRF Charge, deselect the selected item.
			pListBox->DeSelect( nDeselectItem );
        }

		// Note that FLIB has logic to set the top line, too:
		//   If some or all of the current item's lines are not visible, then FLIB will select a top line such
		//     that they are all visible. It only changes the top line when needed.
		//   So if we select a top line wisely, it will be left alone by FLIB.

		// If current item not shown because all or part is above top of listbox, then set top line to show it.
		if( nLine < nLBTopLine )
		{
            // GL test mod
            if( nLine < 0 ) 
                nLine = 0;

			pListBox->SetTopLine( nLine );
		}
		else if( nLineCount > nLBSize )
		{
    		// If very unlikely case of current item bigger than listbox size, then set top line to show first part of it.
			pListBox->SetTopLine( nLine );
		}
		else
		{
    		// Else if current item not shown because all or part is below bottom of listbox, then set top line to show it at bottom of listbox.
			int nLineAfterItem = nLine + nLineCount;
			if( nLineAfterItem > nLBTopLine + nLBSize )
				pListBox->SetTopLine( nLineAfterItem - nLBSize );
		}
	}

	s_bRecurse = FALSE;
}

void ListboxItemChangedEventCallback( ListBox *pListBox,      //	- Pointer to the listbox that changed
	                                  int	   nItemNumber )  //	- Line number now currently selected.
{
	SaleListboxAdjustments( pListBox );
}



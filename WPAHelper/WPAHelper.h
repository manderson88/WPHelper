/*--------------------------------------------------------------------------------------+
//----------------------------------------------------------------------------
// DOCUMENT ID:   
// LIBRARY:       
// CREATOR:       Mark Anderson
// DATE:          05-05-2016
//
// NAME:          WPAHelper
//
// DESCRIPTION:   
//
// REFERENCES:
//
// ---------------------------------------------------------------------------
// NOTICE
//    NOTICE TO ALL PERSONS HAVING ACCESS HERETO:  This document or
//    recording contains computer software or related information
//    constituting proprietary trade secrets of Black & Veatch, which
//    have been maintained in "unpublished" status under the copyright
//    laws, and which are to be treated by all persons having acdcess
//    thereto in manner to preserve the status thereof as legally
//    protectable trade secrets by neither using nor disclosing the
//    same to others except as may be expressly authorized in advance
//    by Black & Veatch.  However, it is intended that all prospective
//    rights under the copyrigtht laws in the event of future
//    "publication" of this work shall also be reserved; for which
//    purpose only, the following is included in this notice, to wit,
//    "(C) COPYRIGHT 1997 BY BLACK & VEATCH, ALL RIGHTS RESERVED"
// ---------------------------------------------------------------------------
/*
/* CHANGE LOG
 * $Archive: /MDL/BVSchemaChecker/WPHelper/WPAHelper.h $
 * $Revision: 1 $
 * $Modtime: 2/24/17 9:03a $
 * $History: WPAHelper.h $
 * 
 * *****************  Version 1  *****************
 * User: Mark.anderson Date: 2/24/17    Time: 9:30a
 * Created in $/MDL/BVSchemaChecker/WPHelper
 * Initial commit for the helper app
 * 
 +--------------------------------------------------------------------------------------*/
#define DIALOGID_MDLDialog	    1
#define HOOKDIALOGID_MDLDialog	    (DIALOGID_MDLDialog * 100) + 1
#define DIALOGID_HostedDialog	    2
#define HOOKDIALOGID_HostedDialog   (DIALOGID_HostedDialog * 100) + 1



#define PUSHBUTTONID_ModalDialog	2
#define PUSHBUTTONID_ModelessDialog	3
#define PUSHBUTTONID_HostedDialog	4
#define PUSHBUTTONID_MDLButton		10
#define PUSHBUTTONID_DockableDialog	5
#define PUSHBUTTONID_ToolSettingsDialog	6

#define MESSAGELISTID_Commands	    1
#define MESSAGELISTID_Messages	    2
#define MESSAGELISTID_Prompts	    3
#define MESSAGELISTID_Misc	    4
#define	MESSAGEID_ResourceLoadError 5

#define MESSAGEID_ToolSettings  1
#define MESSAGEID_abc		2
#define COMMANDID_PlaceDate	3

#define PROMPTMESSAGEID_abc	1
#define PROMPTID_EnterPoint	2

#define MISCMESSAGEID_abc	1


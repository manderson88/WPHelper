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
 * $Archive: /MDL/BVSchemaChecker/WPHelper/WPAHelperCmd.r $
 * $Revision: 1 $
 * $Modtime: 2/24/17 9:03a $
 * $History: WPAHelperCmd.r $
 * 
 * *****************  Version 1  *****************
 * User: Mark.anderson Date: 2/24/17    Time: 9:31a
 * Created in $/MDL/BVSchemaChecker/WPHelper
 * Initial commit for the helper app
 * 
 +--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <cmdclass.h>

/*----------------------------------------------------------------------+
|									|
|   Local Defines							|
|									|
+----------------------------------------------------------------------*/
#define	    CT_NONE	    0
#define	    CT_MAIN	    1
#define     CT_ACTION	    2
#define     CT_ACTYPE        3
#define     CT_DIALOG       4

/*----------------------------------------------------------------------+
|                                                                       |
|   Application command syntax   					|
|                                                                       |
+----------------------------------------------------------------------*/
Table CT_MAIN =
{ 
    { 1, CT_ACTION, PLACEMENT, REQ,	"WPAHELPER" }, 
}

Table CT_ACTION =
{ 
    { 1, CT_ACTYPE,       INHERIT,	NONE,      "ACTION" },
}

Table CT_ACTYPE =
{ 
    { 1, CT_NONE,       INHERIT,	NONE,      "DIALOG" },
}


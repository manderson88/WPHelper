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
 * $Archive: /MDL/BVSchemaChecker/WPHelper/WPAHelper.cpp $
 * $Revision: 3 $
 * $Modtime: 3/15/17 12:11p $
 * $History: WPAHelper.cpp $
 *
 * *****************  Version 3  *****************
 * User: Mark.anderson Date: 3/15/17    Time: 12:46p
 * Updated in $/MDL/BVSchemaChecker/WPHelper
 * updated the write to file hook toggle. Updated the imodel detection
 * code.
 *
 * *****************  Version 2  *****************
 * User: Mark.anderson Date: 3/06/17    Time: 12:23p
 * Updated in $/MDL/BVSchemaChecker/WPHelper
 * clean up comments
 *
 * *****************  Version 1  *****************
 * User: Mark.anderson Date: 2/24/17    Time: 9:30a
 * Created in $/MDL/BVSchemaChecker/WPHelper
 * Initial commit for the helper app
 *
 +--------------------------------------------------------------------------------------*/
//#define MDL
//#define winNT

/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include    <mdl.h>
#include <..\MicroStationAPI\MicroStationAPI.h>
#include    <stdio.h>
#include    <string.h>
#include    <malloc.h>
#include    <msinputq.h>
#include    <msinput.fdf>
#include    <msvar.fdf>
#include    <dlmsys.fdf>
#include    <msdialog.fdf>

#include    <msrmgr.h>
#include    <mssystem.fdf>
#include    <msparse.fdf>

#include	<toolsubs.h>
#include	<msmisc.fdf>
#include    <msdgnmodelref.fdf>
#include	<msoutput.fdf>
#include    <msclipbd.fdf>
#include    <ElementAgenda.h>

#include "WPAHelper.h"
USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_USTN
USING_NAMESPACE_BENTLEY_USTN_ELEMENT

// If the dialog is DIALOGID_CommandStatus, then the completion bar is in the status bar;
//   otherwise, it is in its own dialog.
Private DialogBox   *completionBarDbP=NULL;

Private BoolInt s_copyFlag=FALSE; //false will mean copy ok

/**
 * experimental code to work with the inputqueue.  this would allow blocking
 * commands that add schema to the file.  or for doing input monitoring.
 */
#if defined EXPERIMENTAL_CODE
static unsigned short		allowedMessages;
const int                   commandMask = 1 << (1 - 1);
const int                   dataPointMask = 1 << (2 - 1);
const int                   resetMask = 1 << (3 - 1);
const int                   keyinMask = 1 << (4 - 1);
const int                   unassignedCBMask = 1 << (6 - 1);
#endif
static bool                 s_bSilent = false;

/*------------------------------------------------------------------------------+
 | This will open the completion bar in a dialog box.                           |
 | @param messageTextP the message to display in the completion bar dialog.     |
 +------------------------------------------------------------------------------*/
extern "C" DLLEXPORT void openCompletionBarDialog(char *messageTextP)
{
    completionBarDbP = NULL;

    completionBarDbP = mdlDialog_completionBarOpen(messageTextP);
}
/*------------------------------------------------------------------------------+
 | This will open a completion bar in the main window frame                     |
 | @param  messageTextP the message to put on the completion bar.               |
 +------------------------------------------------------------------------------*/
extern "C" DLLEXPORT void     openCompletionBar
(
char *messageTextP              /* => text message to be displayed */
)
    {
    DialogBox   *dbP;

    if (messageTextP)
        mdlOutput_printf (MSG_ERROR, messageTextP);

    if (NULL != (dbP = mdlDialog_find (DIALOGID_CommandStatus, NULL)))
        {
        CompletionBarInfo       data;

        data.msgText = NULL;
        data.percentComplete = 0;
        mdlDialog_hookDialogSendUserMsg (dbP, CMPLBARID_ResetCompletionBar, &data);
        mdlDialog_hookDialogSendUserMsg (dbP, GENERICID_CompletionBar, 0);
        }

    completionBarDbP = dbP;
    mdlDialog_completionBarUpdate (completionBarDbP, NULL,  0);
    }

/*------------------------------------------------------------------------------+
 | updates the message and the percent complete on the completion bar           |
 | @param  messageText     the message to print out                             |
 | @param  percentComplete the percentage to show on the dialog.                |
 +------------------------------------------------------------------------------*/
extern "C" DLLEXPORT  void     updateCompletionBar
(
char           *messageText,       /* => Message text */
int             percentComplete    /* => % complete on bar */
)
    {
    if (NULL == completionBarDbP)
        return;

    mdlDialog_completionBarUpdate (completionBarDbP, messageText,  percentComplete);
    }

/*------------------------------------------------------------------------------+
 | closes the completion bar.  this is called at the end of the process loop    |
 +------------------------------------------------------------------------------*/
extern "C" DLLEXPORT void     closeCompletionBar
(
void
)
    {
    if (NULL == completionBarDbP)
        return;

    mdlDialog_completionBarClose (completionBarDbP);
    mdlDialog_hookDialogSendUserMsg (completionBarDbP, -GENERICID_CompletionBar, 0);

    mdlOutput_error ("");
    completionBarDbP = NULL;
    }
#if defined EXPERIMENTAL_CODE
/*
The element descripto being written to file hook function
*/
int dscrToFileHook(ElmDscrToFile_Actions action, DgnModelRefP pModel,UInt32 filePos,MSElementDescrP newEdP, MSElementDescrP oldEdP, MSElementDescrP *replacementEdP)
{
    if(!s_bSilent)
        printf("the write to file hook native \n");
    if(s_copyFlag)
    {
        return ELMDTF_STATUS_ABORT;
    }
    else
        return ELMDTF_STATUS_SUCCESS;
}

/*
The element descripto being copied hook function
*/
void elmdscrCopyHook(MSElementDescrH edPP, DgnModelRefP pOrModel,DgnModelRefP destModelP, BoolInt preCopy)
{
    if(!s_bSilent)
        printf ("the copy hook \t");

    DgnFileObjP  pFile = mdlModelRef_getDgnFile(pOrModel);
    MSWCharCP sourceFileName;
    sourceFileName = pFile->GetName();
    printf("from %S file \n",sourceFileName);
    if(pFile->IsIModel())
    {
        if(s_copyFlag==FALSE)
            mdlOutput_messageCenter(MESSAGE_ERROR,"Attempting to Copy from IMODEL","one or more elements selected are in an i-model",TRUE);

        s_copyFlag = true;
    }
   // else
   //     s_copyFlag = false;

}
/*
The reference to master file callback function
*/
void refToMaster(MSElementDescrH edPP, DgnModelRefP pModel)
{
    DgnFileP pFile = mdlModelRef_getDgnFile(pModel);
    if(pFile->IsIModel())
        printf("ref to master copy \n");
}

/*
input queue hook call back function only used for observation
*/
Private int ISpySomething
(
Inputq_element	*iqelP
)
{
   int		 gotInput = 0;
   if(!s_bSilent)
       printf ("got an event of type %ld \n",iqelP->hdr.cmdtype);


       // return INPUT_ACCEPT;

    //  Should verify that it is ones of the types we expect, etc.
    switch (iqelP->hdr.cmdtype)
        {
    case CMDNUM:
        gotInput = commandMask & allowedMessages;
        break;

    case DATAPNT:
        gotInput = dataPointMask & allowedMessages;
        break;

    case RESET:
        gotInput = resetMask & allowedMessages;
        break;

    case KEYIN:
        gotInput = keyinMask & allowedMessages;
        break;

    case UNASSIGNEDCB:
        gotInput = unassignedCBMask & allowedMessages;
        break;
        }

   // if  (gotInput || iqelP->hdr.cmdtype == CMDNUM)
     //   {
       // memcpy (&s_lastIqel, iqelP, iqelP->hdr.bytes > sizeof (s_lastIqel) ? sizeof (s_lastIqel) : iqelP->hdr.bytes);
       // haveInput = 1;
       // }
        return INPUT_ACCEPT;
    }
/*
A function to see if an element is selected and it is from a fence or imodel not used.
*/
bool IsIModelElementSelected()
{
    bool rtStatus = false;
    if(mdlFence_isDefined())
    {
        printf("Fence Processing elements n");

    }
    if(mdlSelect_isActive())
    {
        int numSelected = mdlSelect_numSelected();
        UInt32 *filePos;
        DgnModelRefP *pModels;
        mdlSelect_returnPositions(&filePos,&pModels,&numSelected);
        if(!s_bSilent)
            printf("found %d selected elements \n",numSelected);
        for (int i = numSelected-1;i>=0;i--)
        {
            DgnFileP pFile = mdlModelRef_getDgnFile(pModels[i]);
            if(pFile->IsIModel())
            {
                mdlSelect_removeElement(filePos[i],pModels[i],true);
                rtStatus = true;
            }
        }
        dlmSystem_mdlFree (filePos);
        dlmSystem_mdlFree (pModels);
    }

    return rtStatus;
}
///
//a simple command filter to echo out the commands that are run.
///
int PCKeyinMonitor_commandFilter
(
 Inputq_element *iquelP
 )
    {
    int status=INPUT_COMMAND_ACCEPT;
    char    cmdString[256];

    mdlParse_reconstructFullKeyin (cmdString,
    iquelP->u.cmd.taskId,
    iquelP->u.cmd.command,
    iquelP->u.cmd.unparsed);
    if(!s_bSilent)
        printf ("CMDNUM %s 0x%08x %s class = %ld imm = %ld\n",
    iquelP->u.cmd.taskId, iquelP->u.cmd.command, cmdString, iquelP->u.cmd.commandClass,iquelP->u.cmd.immediate);

   // if((strcmp("TRANSFRM",iquelP->u.cmd.taskId)== 0) && ((0x0e030000==iquelP->u.cmd.command)||(0x04070000==iquelP->u.cmd.command)))
   //     if(IsIModelElementSelected())
   //      status = INPUT_COMMAND_REJECT;

    //if(s_copyFlag)
    //    mdlClipboard_emptyClipboard();

    s_copyFlag = false;
    return status;
    }
#endif
/**
 * This is an implementation of the IElementAgendaEvents from Bentley::Ustn namespace
 * by implementing the methods here developers can control what is in the element
 * Agenda.  An ElementAgenda is the collection of elements that a tool gets from
 * a selection.
 * Important note: the items in this list are allocated by the host platform.
 * Therefore it must be built with the same complier tools as the host platform.
 * In this case it is VisualStudio 2005.  When Bentley updates to Connect Edition
 * it will need to be recomplied to the new toolset.
 */
struct AEvents:IElementAgendaEvents
{
  /**
   | this will look at the agenda and allow the program
   |to modify the contents.
   |in this case the contents are checked to see if it
   |is in an imodel.  if the eh is in an imodel then it
   |is set as invalid.
   | @param  pAgenda the collection of selected elements.
   | @param  opType  what type of operation is being done
   | @param  modify  if the agenda has been modified
   | @return         true to keep processing the Agenda.
   +----------------------------------------------------------------------------*/
    virtual bool DoModifyAgendaEntries(ElementAgendaP pAgenda,AgendaOperation opType,AgendaModify modify)
    {
       ElemAgendaEntry const* start = pAgenda->GetFirst ();
       ElemAgendaEntry const* end = start + pAgenda->GetCount ();
       bool invalidatedElm = false;

        for (ElemAgendaEntry const* curr = start; curr < end ; curr++)
            {
            DgnFileP pFile = curr->GetDgnFile();
            if (pFile->IsIModel())
                {
                EditElemHandleP el = (EditElemHandle*)curr;
                el->Invalidate();//this will prevent the cell from being modified.
                invalidatedElm = true;
                }
            }

        if ((!s_bSilent)&&(invalidatedElm))
            mdlOutput_messageCenter(MESSAGE_ERROR,"Attempting to Copy from IMODEL","one or more elements selected are in an i-model",TRUE);

        return true;
    }
    /**
     * called before the elements in the Agenda are modified.
     * @param agenda    the collection of elements that will be modified.
     * @param opType    the operation type
     * @param modify    is the agenda changed.
     * @param isGroupOp is this a group operation.
     */
    virtual void OnPreModifyAgenda(ElementAgendaCP agenda,AgendaOperation opType,AgendaModify modify,bool isGroupOp)
    {
        //printf("Pre modify event \n");
    }
    /**
     * called after the element is modified.
     * @param agenda    the list of elements that have been modified.
     * @param opType    the operation type.
     * @param modify    is the agenda changed.
     * @param isGroupOp is this a group operation.
     */
    virtual void OnPostModifyAgenda(ElementAgendaCP agenda,AgendaOperation opType,AgendaModify modify,bool isGroupOp)
    {
        //printf("Post modify event \n");
    }
    /**
     | called when the element is going to be copied to or from the clipboard.
     | @param agenda the list of elements that are being processed.
     | @param opType the operation.
     | @param modify is the agenda changed.
     | @param dataP  the clipboard format.
     */
    virtual void DoAddDeferredClipboardFormats(ElementAgendaP agenda,AgendaOperation opType,AgendaModify modify,GuiDataObject * dataP)
    {
        //printf("deferred clipboard formats ... \n");
    }
};
///the static instance of the agenda listener interface.
static AEvents agendaListener;
/*------------------------------------------------------------------------------+
 | this method will add the event listener(s). currently this is only an Agenda |
 | listener.  The flag is to prevent messages from being sent to the debug window |
 | or message center.                                                           |
 | @param iSilent 0 to prevent messages from being printed out to the message center |
 +------------------------------------------------------------------------------*/
extern "C" DLLEXPORT void addWriteToFileHook(int iSilent)
{
    s_bSilent = (iSilent==0);
#if defined (ALLCALLBACKS)
    mdlSystem_setFunction (SYSTEM_ELMDSCR_TO_FILE,dscrToFileHook);
    mdlSystem_setFunction (SYSTEM_ELMDSCR_COPY,elmdscrCopyHook);
    mdlSystem_setFunction (SYSTEM_ELM_REF_TO_MASTER,refToMaster);
    mdlInput_setMonitorFunction (MONITOR_ALL,ISpySomething);
    mdlInput_setFunction (INPUT_COMMAND_FILTER,PCKeyinMonitor_commandFilter);
#endif
    Bentley::Ustn::Element::ElementAgenda::AddListener(&agendaListener);
}
/**
 * removeWriteToFileHook - drops the write to file hook.
 */
extern "C" DLLEXPORT void removeWriteToFileHook()
{
    Bentley::Ustn::Element::ElementAgenda::DropListener(&agendaListener);
}
/*----------------------------------------------------------------------------+
|  isModel function to check to see if the model reference is an i-model.  This
| function was created to fill a void in the COM model.  It uses the
| MicroStatationAPI method on the DgnFileObj for detecting that the file is an
| i-model.
| @param pModel the model reference to check if it is an i-model
|
| @return -1 unknown, 1 is an i-model, 0 - not and i-model
+----------------------------------------------------------------------------*/
extern "C" DLLEXPORT int isIModel(int pModel)
{
    BoolInt bStatus = mdlModelRef_isReference((DgnModelRefP)pModel);

    DgnFileP pFile = mdlModelRef_getDgnFile((DgnModelRefP)pModel);

    if(pFile != NULL)
        if(pFile->IsIModel())
            return 1;
        else
            return 0;
    //if we get a null then try the file name way?
    if (pFile == NULL)
    {
        char fileName[MAXFILELENGTH];
        StatusInt status = mdlModelRef_getFileName((DgnModelRefP)pModel, fileName, MAXFILELENGTH);
        if (SUCCESS == status)
        {
            Bentley::WString wstrName(fileName);
            int pos = wstrName.find_last_of(L".i.dgn");
            if (pos == Bentley::WString::npos)
                return 1;
            else
                return 0;
        }
    }
    return -1;
}

/*---------------------------------------------------------------------------------**//**
* @description  MdlMain
* @param 	argc      The number of command line parameters sent to the application.
* @param 	argv[]    The array of strings sent to the application on the command line
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT  int MdlMain
(
int         argc,
char        *argv[]
)
    {
    RscFileHandle   rfHandle;

    mdlResource_openFile (&rfHandle,NULL,RSC_READONLY);

    return SUCCESS;
    }

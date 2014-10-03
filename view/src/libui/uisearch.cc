/*
** Generated by X-Designer
*/
/*
**LIBS: -lXm -lXt -lX11
*/
// #pragma GCC diagnostic ignored "-Wwrite-strings"

#include <stdlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Protocols.h>  
#include "flags.h"
#include "uisearch.h"

search_shell_p search_shell = (search_shell_p) NULL;



void search_shell_c::create (Widget parent, char *widget_name)
{
	Widget children[20];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget drawingArea1 = (Widget)NULL;
	Widget drawingArea2 = (Widget)NULL;
	Widget form1 = (Widget)NULL;
	Widget frame7 = (Widget)NULL;
	Widget frame6 = (Widget)NULL;
	Widget frame5 = (Widget)NULL;
	Widget frame4 = (Widget)NULL;
	Widget frame1 = (Widget)NULL;
	Widget toggle59 = (Widget)NULL;
	Widget toggle60 = (Widget)NULL;
	Widget toggle61 = (Widget)NULL;
	Widget toggle62 = (Widget)NULL;
	Widget toggle63 = (Widget)NULL;
	Widget toggle64 = (Widget)NULL;
	Widget toggle65 = (Widget)NULL;
	Widget frame2 = (Widget)NULL;
	Widget toggle21 = (Widget)NULL;
	Widget toggle22 = (Widget)NULL;
	Widget toggle23 = (Widget)NULL;
	Widget toggle24 = (Widget)NULL;
	Widget toggle25 = (Widget)NULL;
	Widget toggle26 = (Widget)NULL;
	Widget toggle27 = (Widget)NULL;
	Widget toggle28 = (Widget)NULL;
	Widget toggle29 = (Widget)NULL;
	Widget toggle30 = (Widget)NULL;
	Widget toggle31 = (Widget)NULL;
	Widget toggle32 = (Widget)NULL;
	Widget toggle2 = (Widget)NULL;
	Widget toggle3 = (Widget)NULL;
	Widget frame3 = (Widget)NULL;
	Widget toggle41 = (Widget)NULL;
	Widget toggle42 = (Widget)NULL;
	Widget toggle1 = (Widget)NULL;
	Widget toggle43 = (Widget)NULL;
	Widget toggle44 = (Widget)NULL;
	Widget toggle45 = (Widget)NULL;
	Widget toggle66 = (Widget)NULL;
	toggle11_ = (Widget)NULL;
	toggle12_ = (Widget)NULL;
	toggle13_ = (Widget)NULL;
	fname_ = (Widget)NULL;
	icase_ = (Widget)NULL;
	if ( !widget_name )
	  widget_name = (char*) "search_shell";

	XtSetArg(al[ac], XmNallowShellResize, TRUE); ac++;
	search_shell = XmCreateDialogShell ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = search_shell;
	XmAddWMProtocolCallback( search_shell, XmInternAtom(XtDisplay(search_shell), (char*) "WM_DELETE_WINDOW", False),&search_shell_c:: closeCB, (XtPointer) this);
	XtSetArg(al[ac], XmNresizePolicy, XmRESIZE_ANY); ac++;
	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	form_ = XmCreateForm ( search_shell, (char*) "form_", al, ac );
	ac = 0;
	button_search = XmCreatePushButton ( form_, "button_search", al, ac );
	drawingArea1 = XmCreateDrawingArea ( form_, "drawingArea1", al, ac );
	button_close = XmCreatePushButton ( form_, "button_close", al, ac );
	drawingArea2 = XmCreateDrawingArea ( form_, "drawingArea2", al, ac );
	form1 = XmCreateForm ( form_, "form1", al, ac );
	frame5 = XmCreateFrame ( form1, "frame5", al, ac );
	what_text_ = XmCreateTextField ( frame5, "what_text_", al, ac );
	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	XtSetArg(al[ac], XmNset, TRUE); ac++;
	what_ = XmCreateToggleButton ( frame5, "what_", al, ac );
	ac = 0;
	frame4 = XmCreateFrame ( form1, "frame4", al, ac );
	XtSetArg(al[ac], XmNnumColumns, 3); ac++;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_COLUMN); ac++;
	where_rowcol_ = XmCreateRowColumn ( frame4, "where_rowcol_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	where_ = XmCreateToggleButton ( frame4, "where_", al, ac );
	ac = 0;
	frame1 = XmCreateFrame ( form1, "frame1", al, ac );
	XtSetArg(al[ac], XmNnumColumns, 2); ac++;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_COLUMN); ac++;
	status_rowcol_ = XmCreateRowColumn ( frame1, "status_rowcol_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new statusFlag(STATUS_UNKNOWN)); ac++;
	toggle59 = XmCreateToggleButton ( status_rowcol_, "Unknown", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new statusFlag(STATUS_SUSPENDED)); ac++;
	toggle60 = XmCreateToggleButton ( status_rowcol_, "Suspended", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new statusFlag(STATUS_COMPLETE)); ac++;
	toggle61 = XmCreateToggleButton ( status_rowcol_, "Complete", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new statusFlag(STATUS_QUEUED)); ac++;
	toggle62 = XmCreateToggleButton ( status_rowcol_, "Queued", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new statusFlag(STATUS_SUBMITTED)); ac++;
	toggle63 = XmCreateToggleButton ( status_rowcol_, "Submitted", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new statusFlag(STATUS_ACTIVE)); ac++;
	toggle64 = XmCreateToggleButton ( status_rowcol_, "Active", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new statusFlag(STATUS_ABORTED)); ac++;
	toggle65 = XmCreateToggleButton ( status_rowcol_, "Aborted", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	status_ = XmCreateToggleButton ( frame1, "Status:", al, ac );
	ac = 0;
	frame2 = XmCreateFrame ( form1, "frame2", al, ac );
	XtSetArg(al[ac], XmNnumColumns, 3); ac++;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_COLUMN); ac++;
	type_rowcol_ = XmCreateRowColumn ( frame2, "type_rowcol_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_SUITE)); ac++;
	toggle21 = XmCreateToggleButton ( type_rowcol_, "toggle21", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_FAMILY)); ac++;
	toggle22 = XmCreateToggleButton ( type_rowcol_, "toggle22", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_TASK)); ac++;
	toggle23 = XmCreateToggleButton ( type_rowcol_, "toggle23", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_ALIAS)); ac++;
	toggle24 = XmCreateToggleButton ( type_rowcol_, "toggle24", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_LABEL)); ac++;
	toggle25 = XmCreateToggleButton ( type_rowcol_, "toggle25", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_METER)); ac++;
	toggle26 = XmCreateToggleButton ( type_rowcol_, "toggle26", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_EVENT)); ac++;
	toggle27 = XmCreateToggleButton ( type_rowcol_, "toggle27", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_REPEAT)); ac++;
	toggle28 = XmCreateToggleButton ( type_rowcol_, "toggle28", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_TIME)); ac++;
	toggle29 = XmCreateToggleButton ( type_rowcol_, "toggle29", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_DATE)); ac++;
	toggle30 = XmCreateToggleButton ( type_rowcol_, "toggle30", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_TRIGGER)); ac++;
	toggle31 = XmCreateToggleButton ( type_rowcol_, "toggle31", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_VARIABLE)); ac++;
	toggle32 = XmCreateToggleButton ( type_rowcol_, "toggle32", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_LIMIT)); ac++;
	toggle2 = XmCreateToggleButton ( type_rowcol_, "toggle2", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_INLIMIT)); ac++;
	toggle3 = XmCreateToggleButton ( type_rowcol_, "toggle3", al, ac );

	ac = 0;
	XtSetArg(al[ac], XmNuserData, new typeFlag(NODE_SUPER)); ac++;
	toggle66 = XmCreateToggleButton ( type_rowcol_, "toggle66", al, ac );

	ac = 0;
	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	type_ = XmCreateToggleButton ( frame2, "Type:", al, ac );
	ac = 0;
	frame3 = XmCreateFrame ( form1, "frame3", al, ac );
	XtSetArg(al[ac], XmNnumColumns, 2); ac++;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_COLUMN); ac++;
	special_rowcol_ = XmCreateRowColumn ( frame3, "special_rowcol_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new procFlag(&node::isTimeDependent)); ac++;
	toggle41 = XmCreateToggleButton ( special_rowcol_, "toggle41", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new procFlag(&node::isLate)); ac++;
	toggle42 = XmCreateToggleButton ( special_rowcol_, "toggle42", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new procFlag(&node::isWaiting)); ac++;
	toggle1 = XmCreateToggleButton ( special_rowcol_, "Waiting nodes", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new procFlag(&node::isZombie)); ac++;
	toggle43 = XmCreateToggleButton ( special_rowcol_, "toggle43", al, ac ); 
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new procFlag(&node::isRerun)); ac++;
	toggle44 = XmCreateToggleButton ( special_rowcol_, "toggle44", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNuserData, new procFlag(&node::hasMessages)); ac++;
	toggle45 = XmCreateToggleButton ( special_rowcol_, "toggle45", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	special_ = XmCreateToggleButton ( frame3, "Special:", al, ac );

	ac = 0;
	frame6 = XmCreateFrame ( form1, "frame6", al, ac );
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_COLUMN); ac++;
	timed_rowcol_ = XmCreateRowColumn ( frame6, "timed_rowcol_", al, ac );
	ac = 0;
	timed_text_since_ = XmCreateTextField(timed_rowcol_,"timed_text_since_",al,ac);
	timed_text_from_ = XmCreateTextField(timed_rowcol_,"timed_text_from_",al,ac);
	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	timed_ = XmCreateToggleButton ( frame6, "Status time:", al, ac );

	ac = 0;
	frame7 = XmCreateFrame ( form1, "frame7", al, ac );
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_COLUMN); ac++;
	misc_rowcol_ = XmCreateRowColumn ( frame7, "misc_rowcol_", al, ac );
	ac = 0;
	icase_ = XmCreateToggleButton(misc_rowcol_, "ignore case",al,ac);

	ac = 0;
	fname_ = XmCreateRadioBox ( misc_rowcol_, "search_kind", al, ac );
	toggle11_ = XmCreateToggleButtonGadget ( fname_, "reg. exp.", al, ac );
	toggle12_ = XmCreateToggleButtonGadget ( fname_, "glob", al, ac );
	toggle13_ = XmCreateToggleButtonGadget ( fname_, "substring", al, ac );

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( form_,al, ac );
	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	misc_ = XmCreateToggleButton ( frame7, "Misc.:", al, ac );

	ac = 0;
	XtSetArg(al[ac], XmNcancelButton, button_close); ac++;
	XtSetArg(al[ac], XmNdefaultButton, button_search); ac++;
	XtSetValues ( form_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 3); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetValues ( button_search,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, button_close); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 88); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightWidget, button_search); ac++;
	XtSetValues ( drawingArea1,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 3); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_NONE); ac++;
	XtSetValues ( button_close,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomWidget, button_close); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 98); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightWidget, button_search); ac++;
	XtSetValues ( drawingArea2,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomWidget, drawingArea2); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 0); ac++;
	XtSetValues ( form1,al, ac );
	ac = 0;
	XtAddCallback (form_, XmNmapCallback,&search_shell_c:: mapCB, (XtPointer) this);
	XtAddCallback (button_search, XmNactivateCallback,&search_shell_c:: searchCB, (XtPointer) this);
	XtAddCallback (button_close, XmNactivateCallback,&search_shell_c:: closeCB, (XtPointer) this);

	XtAddCallback (toggle13_, XmNvalueChangedCallback, &search_shell_c::radioCB, (XtPointer) this);
	XtAddCallback (toggle11_, XmNvalueChangedCallback, &search_shell_c::radioCB, (XtPointer) this);
	XtAddCallback (toggle12_, XmNvalueChangedCallback, &search_shell_c::radioCB, (XtPointer) this);
	XtAddCallback (fname_, XmNsimpleCallback,&search_shell_c::radioCB,(XtPointer)this);
	/* http://www.cs.cf.ac.uk/Dave/X_lecture/node8.html */

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 3); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetValues ( frame5,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 3); ac++;
	XtSetArg(al[ac], XmNtopWidget, frame5); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetValues ( frame4,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 3); ac++;
	XtSetArg(al[ac], XmNtopWidget, frame4); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetValues ( frame1,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 3); ac++;
	XtSetArg(al[ac], XmNtopWidget, frame1); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetValues ( frame2,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 3); ac++;
	XtSetArg(al[ac], XmNtopWidget, frame2); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetValues ( frame3,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 3); ac++;
	XtSetArg(al[ac], XmNtopWidget, frame3); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetValues ( frame6,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 3); ac++;
	XtSetArg(al[ac], XmNtopWidget, frame6); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetValues ( frame7,al, ac );
	ac = 0;

	XtAddCallback (what_, XmNvalueChangedCallback,&search_shell_c:: whatCB, (XtPointer) this);
	children[ac++] = what_text_;
	children[ac++] = what_;
	XtManageChildren(children, ac);
	ac = 0;
	XtAddCallback (where_, XmNvalueChangedCallback,&search_shell_c:: whereCB, (XtPointer) this);
	children[ac++] = where_;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = toggle59;
	children[ac++] = toggle60;
	children[ac++] = toggle61;
	children[ac++] = toggle62;
	children[ac++] = toggle63;
	children[ac++] = toggle64;
	children[ac++] = toggle65;
	XtManageChildren(children, ac);
	ac = 0;
	XtAddCallback (status_, XmNvalueChangedCallback,&search_shell_c:: statusCB, (XtPointer) this);
	children[ac++] = status_;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = toggle21;
	children[ac++] = toggle22;
	children[ac++] = toggle23;
	children[ac++] = toggle24;
	children[ac++] = toggle25;
	children[ac++] = toggle26;
	children[ac++] = toggle27;
	children[ac++] = toggle28;
	children[ac++] = toggle29;
	children[ac++] = toggle30;
	children[ac++] = toggle31;
	children[ac++] = toggle32;
	children[ac++] = toggle2;
	children[ac++] = toggle3;
	children[ac++] = toggle66;
	XtManageChildren(children, ac);
	ac = 0;
	XtAddCallback (type_, XmNvalueChangedCallback,&search_shell_c:: typeCB, (XtPointer) this);
	children[ac++] = type_;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = toggle41;
	children[ac++] = toggle42;
	children[ac++] = toggle1;
	children[ac++] = toggle43;
	children[ac++] = toggle44;
	children[ac++] = toggle45;
	XtManageChildren(children, ac);
	ac = 0;
	XtAddCallback (special_, XmNvalueChangedCallback,
		       &search_shell_c::specialCB, (XtPointer) this);
	children[ac++] = special_;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = timed_text_from_;
	children[ac++] = timed_text_since_;
	XtManageChildren(children, ac);
	ac = 0;
	XtAddCallback (timed_, XmNvalueChangedCallback,
		       &search_shell_c::timedCB, (XtPointer) this);
	children[ac++] = timed_;
	XtManageChildren(children, ac);

	ac = 0;
	children[ac++] = fname_;
	children[ac++] = icase_;
	// children[ac++] = radio_;
	XtManageChildren(children, ac);


	ac = 0;
	children[ac++] = toggle11_;
	children[ac++] = toggle12_;
	children[ac++] = toggle13_;
	XtManageChildren(children, ac);

	ac = 0;
	XtAddCallback (misc_, XmNvalueChangedCallback,
		       &search_shell_c::miscCB, (XtPointer) this);
	children[ac++] = misc_;
	XtManageChildren(children, ac);

	ac = 0;
	children[ac++] = frame5;
	children[ac++] = frame4;
	children[ac++] = frame1;
	children[ac++] = frame2;
	children[ac++] = frame3;
	children[ac++] = frame7;
	children[ac++] = frame6;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = button_search;
	children[ac++] = drawingArea1;
	children[ac++] = button_close;
	children[ac++] = drawingArea2;
	children[ac++] = form1;
	XtManageChildren(children, ac);
	ac = 0;
}

void search_shell_c::specialCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->specialCB ( widget, call_data );
}

void search_shell_c::typeCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->typeCB ( widget, call_data );
}

void search_shell_c::statusCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->statusCB ( widget, call_data );
}

void search_shell_c::whereCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->whereCB ( widget, call_data );
}

void search_shell_c::whatCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->whatCB ( widget, call_data );
}

void search_shell_c::mapCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->mapCB ( widget, call_data );
}

void search_shell_c::closeCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->closeCB ( widget, call_data );
}

void search_shell_c::searchCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->searchCB ( widget, call_data );
}

void search_shell_c::timedCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->timedCB ( widget, call_data );
}

void search_shell_c::miscCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->miscCB ( widget, call_data );
}

void search_shell_c::radioCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	search_shell_p instance = (search_shell_p) client_data;
	instance->radioCB ( widget, call_data );
}

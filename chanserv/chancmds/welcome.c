/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: welcome
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 2
 * CMDDESC: Shows or changes the welcome message on a channel.
 * CMDFUNC: csc_dowelcome
 * CMDPROTO: int csc_dowelcome(void *source, int cargc, char **cargv);
 * CMDHELP: Usage: WELCOME <channel> [<message>]
 * CMDHELP: This shows the current welcome message set on a channel and allows it to be
 * CMDHELP: changed.  In order to be displayed to users, the feature must be enabled
 * CMDHELP: by the +w chanflag (see CHANFLAGS).  Where:
 * CMDHELP: channel - channel to use
 * CMDHELP: message - welcome message to set.  If this is not provided the existing welcome
 * CMDHELP:           message is displayed.
 * CMDHELP: Displaying the message requires operator (+o) access on the named channel.
 * CMDHELP: Changing the message requires master (+m) access on the named channel.
 */

#include "../chanserv.h"
#include "../../nick/nick.h"
#include "../../lib/flags.h"
#include "../../lib/irc_string.h"
#include "../../channel/channel.h"
#include "../../parser/parser.h"
#include "../../irc/irc.h"
#include "../../localuser/localuserchannel.h"
#include <string.h>
#include <stdio.h>

int csc_dowelcome(void *source, int cargc, char **cargv) {
  nick *sender=source;
  chanindex *cip;
  regchan *rcp;
  sstring *oldwelcome;

  if (cargc<1) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "welcome");
    return CMD_ERROR;
  }

  if (!(cip=cs_checkaccess(sender, cargv[0], CA_OPPRIV, NULL, "welcome",
			   QPRIV_VIEWWELCOME, 0)))
    return CMD_ERROR;

  rcp=cip->exts[chanservext];

  if (cargc>1) {
    if (!cs_checkaccess(sender, NULL, CA_MASTERPRIV, cip, "welcome",
			QPRIV_CHANGEWELCOME, 0))
      return CMD_ERROR;

    oldwelcome=rcp->welcome;
    
    rcp->welcome=getsstring(cargv[1], 500);
    csdb_updatechannel(rcp);

    cs_log(sender,"WELCOME %s %s (was %s)",cip->name->content,rcp->welcome->content,oldwelcome?oldwelcome->content:"unset");
    freesstring(oldwelcome);
    chanservstdmessage(sender, QM_DONE);
  }

  chanservstdmessage(sender, QM_WELCOMEMESSAGEIS, rcp->index->name->content, 
		     rcp->welcome?rcp->welcome->content:"(none)");

  return CMD_OK;
}

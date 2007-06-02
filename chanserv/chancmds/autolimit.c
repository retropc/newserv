/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: autolimit
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 2
 * CMDDESC: Shows or changes the autolimit threshold on a channel.
 * CMDFUNC: csc_doautolimit
 * CMDPROTO: int csc_doautolimit(void *source, int cargc, char **cargv);
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

int csc_doautolimit(void *source, int cargc, char **cargv) {
  nick *sender=source;
  chanindex *cip;
  regchan *rcp;
  int oldlimit;

  if (cargc<1) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "autolimit");
    return CMD_ERROR;
  }

  if (!(cip=cs_checkaccess(sender, cargv[0], CA_OPPRIV,
			   NULL, "autolimit", QPRIV_VIEWAUTOLIMIT, 0)))
    return CMD_ERROR;

  rcp=cip->exts[chanservext];

  if (cargc>1) {
    if (!cs_checkaccess(sender, NULL, CA_MASTERPRIV, 
			cip, "autolimit", QPRIV_CHANGEAUTOLIMIT, 0))
      return CMD_ERROR;

    oldlimit=rcp->autolimit;
    rcp->autolimit=strtol(cargv[1],NULL,10);

    if (rcp->autolimit<1)
      rcp->autolimit=1;
    
    csdb_updatechannel(rcp);
    
    cs_log(sender,"AUTOLIMIT %s %s (%d -> %d)",cip->name->content,cargv[1],oldlimit,rcp->autolimit);
    chanservstdmessage(sender, QM_DONE);
    rcp->limit=0;
    cs_timerfunc(cip);
  }

  chanservstdmessage(sender, QM_CHANAUTOLIMIT, cargv[0], rcp->autolimit);
  return CMD_OK;
}

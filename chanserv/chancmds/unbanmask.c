/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: unbanmask
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 2
 * CMDDESC: Removes bans matching a particular mask from a channel.
 * CMDFUNC: csc_dounbanmask
 * CMDPROTO: int csc_dounbanmask(void *source, int cargc, char **cargv);
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

int csc_dounbanmask(void *source, int cargc, char **cargv) {
  nick *sender=source;
  chanindex *cip;
  regban **rbh, *rbp;
  chanban **cbh, *cbp;
  regchan *rcp;
  chanban *theban;
  modechanges changes;
  char *banstr;

  if (cargc<2) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "unbanmask");
    return CMD_ERROR;
  }

  if (!(cip=cs_checkaccess(sender, cargv[0], CA_OPPRIV, NULL, "unbanmask", 0, 0)))
    return CMD_ERROR;

  rcp=cip->exts[chanservext];
  theban=makeban(cargv[1]);

  if (cip->channel)
    localsetmodeinit(&changes, cip->channel, chanservnick);
    
  for (rbh=&(rcp->bans); *rbh; ) {
    rbp=*rbh;
    if (banoverlap(theban, rbp->cbp)) {
      banstr=bantostring(rbp->cbp);
      /* Check perms and remove */
      if (!cs_checkaccess(sender, NULL, CA_MASTERPRIV, cip, NULL, 0, 1)) {
	chanservstdmessage(sender, QM_NOTREMOVEDPERMBAN, banstr, cip->name->content);
	rbh=&(rbp->next);
      } else {
	chanservstdmessage(sender, QM_REMOVEDPERMBAN, banstr, cip->name->content);
	if (cip->channel)
	  localdosetmode_ban(&changes, banstr, MCB_DEL);
	/* Remove from database */
	csdb_deleteban(rbp);
	/* Remove from list */
	(*rbh)=rbp->next;
	/* Free ban/string and update setby refcount, and free actual regban */
	freesstring(rbp->reason);
	freechanban(rbp->cbp);
	freeregban(rbp);
      }
    } else {
      rbh=&(rbp->next);
    }
  }

  if (cip->channel) {
    for (cbh=&(cip->channel->bans); *cbh; ) {
      cbp=*cbh;
      if (banoverlap(theban, cbp)) {
	/* Remove */
	banstr=bantostring(cbp);
	chanservstdmessage(sender, QM_REMOVEDCHANBAN, banstr, cip->name->content);
	localdosetmode_ban(&changes, banstr, MCB_DEL);
      } else {
	cbh=&(cbp->next);
      }
    }
    localsetmodeflush(&changes,1);
  }
  
  
  chanservstdmessage(sender, QM_DONE);
  return CMD_OK;
}

/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: voice
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 20
 * CMDDESC: Voices you or other users on channel(s).
 * CMDFUNC: csc_dovoice
 * CMDPROTO: int csc_dovoice(void *source, int cargc, char **cargv);
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

int csc_dovoice(void *source, int cargc, char **cargv) {
  nick *sender=source, *np;
  reguser *rup=getreguserfromnick(sender);
  chanindex *cip;
  regchan *rcp=NULL;
  regchanuser *rcup;
  channel **ca;
  unsigned long *lp;
  int i;
  modechanges changes;

  if (!rup)
    return CMD_ERROR;
  
  if (cargc==0) {
    /* No args: "voice me on every channel you can */
    ca=sender->channels->content;
    for (i=0;i<sender->channels->cursi;i++) {
      if ((rcp=ca[i]->index->exts[chanservext]) && !CIsSuspended(rcp)) {
	/* It's a Q channel */
	if (!(*(getnumerichandlefromchanhash(ca[i]->users, sender->numeric)) & 
	      (CUMODE_OP|CUMODE_VOICE))) {
	  /* They're not opped or voiced */
	  rcup=findreguseronchannel(rcp, rup);
	  if ((!rcup || !CUIsQuiet(rcup)) && 
	      ((rcup && CUHasVoicePriv(rcup)) ||
	      (CIsVoiceAll(rcp)))) {
	    /* And they have voice priv on the chan (or it's autovoice): 
	     * voice them */
	    localsetmodeinit(&changes, ca[i], chanservnick);
	    localdosetmode_nick(&changes, sender, MC_VOICE);
	    localsetmodeflush(&changes,1);
	  }
	}
      }
    }
    
    chanservstdmessage(sender, QM_DONE);
    return CMD_OK;
  }

  /* If there is at least one arg, the first is a channel */

  if (!(cip=cs_checkaccess(sender, cargv[0], CA_VOICEPRIV, NULL, "voice", 0, 0)))
    return CMD_ERROR;

  if (cargc==1) {
    /* Only one arg: "voice me" */
    if (!cs_checkaccess(sender, NULL, CA_VOICEPRIV | CA_DEVOICED, cip,
			"voice", 0, 0))
      return CMD_ERROR;

    localsetmodeinit(&changes, cip->channel, chanservnick);
    localdosetmode_nick(&changes, sender, MC_VOICE);
    localsetmodeflush(&changes,1);

    chanservstdmessage(sender, QM_DONE);
    return CMD_OK;
  }

  /* You've got to be a master to 'silently' voice other people */
  if (!cs_checkaccess(sender, NULL, CA_MASTERPRIV, cip, "voice", 0, 0))
    return CMD_ERROR;

  rcp=cip->exts[chanservext];

  /* Set up the modes */
  localsetmodeinit(&changes, cip->channel, chanservnick);

  for(i=1;i<cargc;i++) {
    if (!(np=getnickbynick(cargv[i]))) {
      chanservstdmessage(sender, QM_UNKNOWNUSER, cargv[i]);
      continue;
    }

    if (!(lp=getnumerichandlefromchanhash(cip->channel->users, np->numeric))) {
      chanservstdmessage(sender, QM_USERNOTONCHAN, np->nick, cip->name->content);
      continue;
    }

    if (*lp & CUMODE_VOICE) {
      chanservstdmessage(sender, QM_USERVOICEDONCHAN, np->nick, cip->name->content);
      continue;
    }
   
    if ((rup=getreguserfromnick(np)) && (rcup=findreguseronchannel(rcp, rup)) && CUIsQuiet(rcup)) {
      chanservstdmessage(sender, QM_CANTVOICE, np->nick, cip->name->content);
      continue;
    }

    localdosetmode_nick(&changes, np, MC_VOICE);
  }

  localsetmodeflush(&changes, 1);
  chanservstdmessage(sender, QM_DONE);

  return CMD_OK;
}

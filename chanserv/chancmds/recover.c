/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: recover
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 1
 * CMDDESC: Recovers a channel (same as deopall, unbanall, clearchan).
 * CMDFUNC: csc_dorecover
 * CMDPROTO: int csc_dorecover(void *source, int cargc, char **cargv);
 * CMDHELP: Usage: RECOVER <channel>
 * CMDHELP: This command resets the named channel if undesired modes, bans or ops have been
 * CMDHELP: set.  This has the same effect as DEOPALL followed by UNBANALL followed by
 * CMDHELP: CLEARCHAN on the channel, where:
 * CMDHELP: channel - channel to recover
 * CMDHELP: RECOVER requires master (+m) access on the named channel.
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

int csc_dorecover(void *source, int cargc, char **cargv) {
  nick *sender=source,*np;
  reguser *rup;
  regchanuser *rcup;
  regchan *rcp;
  chanindex *cip;
  unsigned long *lp;
  int i;
  modechanges changes;
  
  if (cargc<1) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "recover");
    return CMD_ERROR;
  }

  if (!(cip=cs_checkaccess(sender, cargv[0], CA_MASTERPRIV, NULL, "recover",0, 0)))
    return CMD_ERROR;

  rcp=cip->exts[chanservext];

  if (cip->channel) {
    localsetmodeinit(&changes, cip->channel, chanservnick);

    /* clearchan */
    localdosetmode_key(&changes, NULL, MCB_DEL);
    localdosetmode_simple(&changes, 0, cip->channel->flags);
    cs_docheckchanmodes(cip->channel, &changes);

    /* unbanall */
    while (cip->channel->bans) {
      localdosetmode_ban(&changes, bantostring(cip->channel->bans), MCB_DEL);
    }

    /* deopall */
    for (i=0,lp=cip->channel->users->content;
	 i<cip->channel->users->hashsize;i++,lp++) {
      if (*lp!=nouser && (*lp & CUMODE_OP)) {
	if (!(np=getnickbynumeric(*lp)) || 
	    (!IsService(np) && (!(rup=getreguserfromnick(np)) || 
	    !(rcup=findreguseronchannel(rcp, rup)) || !(CUHasOpPriv(rcup)) ||
	    !(CUIsProtect(rcup) || CIsProtect(rcp))))) {
	  localdosetmode_nick(&changes, np, MC_DEOP);
	}
      }
    }

    localsetmodeflush(&changes, 1);
  }

  cs_log(sender,"RECOVER %s",cip->name->content);
  chanservstdmessage(sender, QM_DONE);
  return CMD_OK;
}     

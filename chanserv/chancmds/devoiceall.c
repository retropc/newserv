/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: devoiceall
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 1
 * CMDDESC: Devoices all users on a channel.
 * CMDFUNC: csc_dodevoiceall
 * CMDPROTO: int csc_dodevoiceall(void *source, int cargc, char **cargv);
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

int csc_dodevoiceall(void *source, int cargc, char **cargv) {
  nick *sender=source,*np;
  reguser *rup;
  regchanuser *rcup;
  regchan *rcp;
  chanindex *cip;
  unsigned long *lp;
  int i;
  modechanges changes;
  
  if (cargc<1) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "devoiceall");
    return CMD_ERROR;
  }

  if (!(cip=cs_checkaccess(sender, cargv[0], CA_MASTERPRIV, NULL, "devoiceall",0, 0)))
    return CMD_ERROR;

  rcp=cip->exts[chanservext];

  if (cip->channel) {
    localsetmodeinit(&changes, cip->channel, chanservnick);

    for (i=0,lp=cip->channel->users->content;
	 i<cip->channel->users->hashsize;i++,lp++) {
      if (*lp!=nouser && (*lp & CUMODE_VOICE)) {
	if (!(np=getnickbynumeric(*lp)) || 
	    (!IsService(np) && (!(rup=getreguserfromnick(np)) || 
	    !(rcup=findreguseronchannel(rcp, rup)) || !(CUHasVoicePriv(rcup)) ||
	    !(CUIsProtect(rcup) || CIsProtect(rcp))))) {
	  localdosetmode_nick(&changes, np, MC_DEVOICE);
	}
      }
    }

    localsetmodeflush(&changes, 1);
  }

  cs_log(sender,"DEVOICEALL %s",cip->name->content);
  chanservstdmessage(sender, QM_DONE);
  return CMD_OK;
}     

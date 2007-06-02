/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: settopic
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 2
 * CMDDESC: Changes the topic on a channel.
 * CMDFUNC: csc_dosettopic
 * CMDPROTO: int csc_dosettopic(void *source, int cargc, char **cargv);
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

int csc_dosettopic(void *source, int cargc, char **cargv) {
  nick *sender=source;
  chanindex *cip;
  regchan *rcp;

  if (cargc<1) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "settopic");
    return CMD_ERROR;
  }

  if (!(cip=cs_checkaccess(sender, cargv[0], CA_TOPICPRIV, 
			   NULL, "settopic", 0, 0)))
    return CMD_ERROR;

  rcp=cip->exts[chanservext];

  if (cargc>1) {
    if (rcp->topic)
      freesstring(rcp->topic);
    rcp->topic=getsstring(cargv[1],TOPICLEN);
  } 
  
  if (rcp->topic && cip->channel) {
    localsettopic(chanservnick, cip->channel, rcp->topic->content);
  }

  chanservstdmessage(sender, QM_DONE);
  csdb_updatechannel(rcp);
  return CMD_OK;
}

/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: requestmasterpassword
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 1
 * CMDDESC: Requests a new master password by email.
 * CMDFUNC: csa_doreqmasterpw
 * CMDPROTO: int csa_doreqmasterpw(void *source, int cargc, char **cargv);
 */

#include "../chanserv.h"
#include "../authlib.h"
#include "../../lib/irc_string.h"
#include <stdio.h>
#include <string.h>

int csa_doreqmasterpw(void *source, int cargc, char **cargv) {
  reguser *rup;
  nick *sender=source;

  if (!(rup=getreguserfromnick(sender)))
    return CMD_ERROR;

  if (csa_checkthrottled(sender, rup, "REQUESTMASTERPASSWORD"))
    return CMD_ERROR;

  csa_createrandompw(rup->masterpass, PASSLEN);
  chanservstdmessage(sender, QM_MASTERPWCHANGED);
  cs_log(sender,"REQUESTMASTERPASSWORD OK username %s",rup->username);
  csdb_updateuser(rup);
  csdb_createmail(rup, QMAIL_NEWMASTERPW);

  return CMD_OK;
}

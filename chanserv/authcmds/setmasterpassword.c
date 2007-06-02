/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: setmasterpassword
 * CMDLEVEL: QCMD_OPER
 * CMDARGS: 2
 * CMDDESC: Set the master password.
 * CMDFUNC: csa_dosetmasterpw
 * CMDPROTO: int csa_dosetmasterpw(void *source, int cargc, char **cargv);
 */

#include "../chanserv.h"
#include "../authlib.h"
#include "../../lib/irc_string.h"
#include <stdio.h>
#include <string.h>

int csa_dosetmasterpw(void *source, int cargc, char **cargv) {
  reguser *rup;
  nick *sender=source;

  if (cargc<1) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "setmasterpassword");
    return CMD_ERROR;
  }

  if (!(rup=findreguser(sender, cargv[0])))
    return CMD_ERROR;

  csa_createrandompw(rup->masterpass, PASSLEN);
  chanservstdmessage(sender, QM_MASTERPWCHANGED);
  cs_log(sender,"SETMASTERPASSWORD OK username %s",rup->username);
  csdb_updateuser(rup);

  return CMD_OK;
}

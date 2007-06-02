/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: newpass
 * CMDLEVEL: QCMD_AUTHED
 * CMDARGS: 3
 * CMDDESC: Change your password.
 * CMDFUNC: csa_donewpw
 * CMDPROTO: int csa_donewpw(void *source, int cargc, char **cargv);
 */

#include "../chanserv.h"
#include "../authlib.h"
#include "../../lib/irc_string.h"
#include <stdio.h>
#include <string.h>

int csa_donewpw(void *source, int cargc, char **cargv) {
  reguser *rup;
  nick *sender=source;

  if (cargc<3) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "newpass");
    return CMD_ERROR;
  }

  if (!(rup=getreguserfromnick(sender)))
    return CMD_ERROR;

  if (!checkmasterpassword(rup, cargv[0])) {
    chanservstdmessage(sender, QM_AUTHFAIL);
    cs_log(sender,"NEWPASS FAIL username %s bad masterpassword %s",rup->username,cargv[0]);
    return CMD_ERROR;
  }

  if (strcmp(cargv[1],cargv[2])) {
    chanservstdmessage(sender, QM_PWDONTMATCH); /* Sorry, passwords do not match */
    cs_log(sender,"NEWPASS FAIL username %s new passwords don't match (%s vs %s)",rup->username,cargv[1],cargv[2]);
    return CMD_ERROR;
  }

  if (strlen(cargv[1]) < 6) {
    chanservstdmessage(sender, QM_PWTOSHORT); /* new password to short */
    cs_log(sender,"NEWPASS FAIL username %s password to short %s (%d characters)",rup->username,cargv[1],strlen(cargv[1]));
    return CMD_ERROR;
  }

  setpassword(rup, cargv[1]);
  rup->lastauth=time(NULL);
  chanservstdmessage(sender, QM_PWCHANGED);
  cs_log(sender,"NEWPASS OK username %s", rup->username);
  csdb_updateuser(rup);
  csdb_createmail(rup, QMAIL_NEWPW);

  return CMD_OK;
}

/* Automatically generated by refactor.pl.
 *
 *
 * CMDNAME: spewpass
 * CMDLEVEL: QCMD_OPER
 * CMDARGS: 1
 * CMDDESC: Search for a password in the database.
 * CMDFUNC: csu_dospewpass
 * CMDPROTO: int csu_dospewpass(void *source, int cargc, char **cargv);
 */

#include "../chanserv.h"
#include "../../lib/irc_string.h"
#include <stdio.h>
#include <string.h>

int csu_dospewpass(void *source, int cargc, char **cargv) {
  nick *sender=source;
  reguser *rup=getreguserfromnick(sender);
  reguser *dbrup;
  int i;
  unsigned int count=0;
  
  if (!rup)
    return CMD_ERROR;
  
  if (cargc < 1) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "spewpass");
    return CMD_ERROR;
  }
  
  chanservstdmessage(sender, QM_SPEWHEADER);
  for (i=0;i<REGUSERHASHSIZE;i++) {
    for (dbrup=regusernicktable[i]; dbrup; dbrup=dbrup->nextbyname) {
      if (!match(cargv[0], dbrup->password)) {
        chanservsendmessage(sender, "%-15s %-10s %-30s %s", dbrup->username, UHasSuspension(dbrup)?"yes":"no", dbrup->email?dbrup->email->content:"none set", dbrup->lastuserhost?dbrup->lastuserhost->content:"none");
        count++;
        if (count >= 2000) {
          chanservstdmessage(sender, QM_TOOMANYRESULTS, 2000, "users");
          return CMD_ERROR;
        }
      }
    }
  }
  chanservstdmessage(sender, QM_RESULTCOUNT, count, "user", (count==1)?"":"s");
  
  return CMD_OK;
}

/* nick.c */

#include "nick.h"
#include "../lib/flags.h"
#include "../lib/irc_string.h"
#include "../lib/base64.h"
#include "../irc/irc.h"
#include "../irc/irc_config.h"
#include "../core/error.h"
#include "../core/hooks.h"
#include "../lib/sstring.h"
#include "../server/server.h"
#include "../parser/parser.h"
#include "../lib/version.h"
#include "../lib/ccassert.h"
#include "../core/nsmalloc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

MODULE_VERSION("");

CCASSERT(sizeof(host) == sizeof(realname));

const flag umodeflags[] = {
   { 'i', UMODE_INV },
   { 'w', UMODE_WALLOPS },
   { 'g', UMODE_DEBUG },
   { 'o', UMODE_OPER },
   { 'k', UMODE_SERVICE },
   { 'X', UMODE_XOPER },
   { 'd', UMODE_DEAF },
   { 'r', UMODE_ACCOUNT },
   { 'n', UMODE_HIDECHAN },
   { 'x', UMODE_HIDEHOST },
   { 'h', UMODE_SETHOST },
   { 'R', UMODE_REGPRIV },
   { 'I', UMODE_HIDEIDLE },
   { 'P', UMODE_PARANOID },
   { 'q', UMODE_COMCHANS },
   { 'Q', UMODE_COMCHANSRESTR },
   { 'C', UMODE_CLOAKED },
   { '\0', 0 } };

const flag accountflags[] = {
   { 'q', AFLAG_STAFF },
   { 'h', AFLAG_SUPPORT },
   { 'o', AFLAG_OPER },
   { 'a', AFLAG_ADMIN },
   { 'd', AFLAG_DEVELOPER },
   { '\0', 0 } };

#define nickhash(x)       ((irc_crc32i(x))%NICKHASHSIZE)

nick *nicktable[NICKHASHSIZE];
nick **servernicks[MAXSERVERS];

sstring *nickextnames[MAXNICKEXTS];

void nickstats(int hooknum, void *arg);

char *NULLAUTHNAME = "";

void _init() {
  unsigned int i;
  authname *anp;

  /* Clear up the nicks in authext */
  for (i=0;i<AUTHNAMEHASHSIZE;i++) 
    for (anp=authnametable[i];anp;anp=anp->next)
      anp->nicks=NULL;  

  initnickhelpers();
  memset(nicktable,0,sizeof(nicktable));
  memset(servernicks,0,sizeof(servernicks));

  /* If we're connected to IRC, force a disconnect.  This needs to be done
   * before we register all our hooks which would otherwise get called
   * during the disconnect. */
  if (connected) {
    irc_send("%s SQ %s 0 :Resync [adding nick support]",mynumeric->content,myserver->content); irc_disconnected(0);
  }
  
  /* Register our hooks */
  registerhook(HOOK_SERVER_NEWSERVER,&handleserverchange);
  registerhook(HOOK_SERVER_LOSTSERVER,&handleserverchange);
  registerhook(HOOK_CORE_STATSREQUEST,&nickstats);
  
  /* And our server handlers */
  registerserverhandler("N",&handlenickmsg,10);
  registerserverhandler("D",&handlekillmsg,2);
  registerserverhandler("Q",&handlequitmsg,1);
  registerserverhandler("M",&handleusermodemsg,3);
  registerserverhandler("AC",&handleaccountmsg,4);
  registerserverhandler("P",&handleprivmsg,2);
  registerserverhandler("A",&handleawaymsg,1);
  registerserverhandler("CA",&handleaddcloak,1);
  registerserverhandler("CU",&handleclearcloak,0);
  
  /* Fake the addition of our own server */
  handleserverchange(HOOK_SERVER_NEWSERVER,(void *)numerictolong(mynumeric->content,2));
}

void _fini() {
  nick *np;
  int i;

  fininickhelpers();

  for (i=0;i<NICKHASHSIZE;i++) {
    for (np=nicktable[i];np;np=np->next) {
      freesstring(np->shident);
      freesstring(np->sethost);
      freesstring(np->opername);
      if(!np->auth && np->authname && (np->authname != NULLAUTHNAME))
        free(np->authname);
    }
  }

  nsfreeall(POOL_NICK);

  /* Free the hooks */
  deregisterhook(HOOK_SERVER_NEWSERVER,&handleserverchange);
  deregisterhook(HOOK_SERVER_LOSTSERVER,&handleserverchange);
  deregisterhook(HOOK_CORE_STATSREQUEST,&nickstats);
  
  /* And our server handlers */
  deregisterserverhandler("N",&handlenickmsg);
  deregisterserverhandler("D",&handlekillmsg);
  deregisterserverhandler("Q",&handlequitmsg);
  deregisterserverhandler("M",&handleusermodemsg);
  deregisterserverhandler("AC",&handleaccountmsg); 
  deregisterserverhandler("P",&handleprivmsg);
  deregisterserverhandler("A",&handleawaymsg);
  deregisterserverhandler("CA",&handleaddcloak);
  deregisterserverhandler("CU",&handleclearcloak);
}

/*
 * This function handles servers appearing and disappearing.
 *  For a new server, the client table is allocated.
 *  For a disappearing server, all it's clients are killed and the client table is freed.
 */

void handleserverchange(int hooknum, void *arg) {
  long servernum;
  int i;
  
  servernum=(long)arg;
  
  switch(hooknum) {
    case HOOK_SERVER_NEWSERVER:
      servernicks[servernum]=(nick **)nscalloc(POOL_NICK,(serverlist[servernum].maxusernum+1),sizeof(nick **));
      break;
      
    case HOOK_SERVER_LOSTSERVER:
      for (i=0;i<=serverlist[servernum].maxusernum;i++) {
        if (servernicks[servernum][i]!=NULL) {
          deletenick(servernicks[servernum][i]);        
        }
      }
      nsfree(POOL_NICK,servernicks[servernum]);
      break;
  }
}

/*
 * deletenick:
 *
 * This function handles the removal of a nick from the network 
 */
 
void deletenick(nick *np) {
  nick **nh;

  /* Fire a pre-lostnick trigger to allow hooks to check the channels etc. of a lost nick */
  triggerhook(HOOK_NICK_PRE_LOSTNICK, np);

  /* Fire the hook.  This will deal with removal from channels etc. */
  triggerhook(HOOK_NICK_LOSTNICK, np);
  
  /* Release the realname and hostname parts */

  for (nh=&(np->realname->nicks);*nh;nh=&((*nh)->nextbyrealname)) {
    if (*nh==np) {
      *nh=np->nextbyrealname;
      break;
    }
  }

  for (nh=&(np->host->nicks);*nh;nh=&((*nh)->nextbyhost)) {
    if (*nh==np) {
      *nh=np->nextbyhost;
      break;
    }
  }
  
  releaserealname(np->realname);
  releasehost(np->host);
  
  if(IsAccount(np)) {
    if(!np->auth) {
      if(np->authname && (np->authname != NULLAUTHNAME))
        free(np->authname);
    } else {
      np->auth->usercount--;
    
      for (nh=&(np->auth->nicks);*nh;nh=&((*nh)->nextbyauthname)) {
        if (*nh==np) {
          *nh=np->nextbyauthname;
          break;
        }
      }
    
      releaseauthname(np->auth);
    }
  }
  
  freesstring(np->shident); /* freesstring(NULL) is OK */
  freesstring(np->sethost); 
  freesstring(np->opername); 
  freesstring(np->message);

  node_decrement_usercount(np->ipnode);
  derefnode(iptree, np->ipnode);
  
  /* TODO: figure out how to cleanly remove nodes without affecting other modules */

  /* Remove cloak entries for the user */
  removecloaktarget(np);
  clearcloaktargets(np);

  /* Delete the nick from the servernick table */  
  *(gethandlebynumericunsafe(np->numeric))=NULL;
  
  /* Remove the nick from the hash table */
  removenickfromhash(np);
  
  freenick(np);
}

void addnicktohash(nick *np) {
  np->next=nicktable[nickhash(np->nick)];
  nicktable[nickhash(np->nick)]=np;
}

void removenickfromhash(nick *np) {
  nick **nh;
  
  for (nh=&(nicktable[nickhash(np->nick)]);*nh;nh=&((*nh)->next)) {
    if ((*nh)==np) {
      (*nh)=np->next;
      break;
    }
  }
}

nick *getnickbynick(const char *name) {
  nick *np;
  
  for (np=nicktable[nickhash(name)];np;np=np->next) {
    if (!ircd_strcmp(np->nick,name)) 
      return np;
  }
  
  return NULL;
}

void nickstats(int hooknum, void *arg) {
  int total,maxchain,curchain,i,buckets;
  nick *np;
  char buf[200];
  
  /* Get nick stats */
  buckets=total=maxchain=curchain=0;
  for (i=0;i<NICKHASHSIZE;i++,curchain=0) {
    np=nicktable[i];
    if (np!=NULL) {
      buckets++;
      for (;np;np=np->next) {
        total++;
        curchain++;
      }
    }
    if (curchain>maxchain) {
      maxchain=curchain;
    }
  }
    
  if ((long)arg>5) {
    /* Full stats */
    sprintf(buf,"Nick    : %6d nicks    (HASH: %6d/%6d, chain %3d)",total,buckets,NICKHASHSIZE,maxchain);
  } else if ((long)arg>2) {
    sprintf(buf,"Nick    : %6d users on network.",total);
  }
  
  if ((long)arg>2) {
    triggerhook(HOOK_CORE_STATSREPLY,buf);
  }
}

int registernickext(const char *name) {
  int i;
  
  if (findnickext(name)!=-1) {
    Error("nick",ERR_WARNING,"Tried to register duplicate nick extension %s",name);
    return -1;
  }
  
  for (i=0;i<MAXNICKEXTS;i++) {
    if (nickextnames[i]==NULL) {
      nickextnames[i]=getsstring(name,100);
      return i;
    }
  }
  
  Error("nick",ERR_WARNING,"Tried to register too many nick extensions: %s",name);
  return -1;
}

int findnickext(const char *name) {
  int i;
  
  for (i=0;i<MAXNICKEXTS;i++) {
    if (nickextnames[i]!=NULL && !ircd_strcmp(name,nickextnames[i]->content)) {
      return i;
    }
  }
  
  return -1;
}

void releasenickext(int index) {
  int i;
  nick *np;
  
  freesstring(nickextnames[index]);
  nickextnames[index]=NULL;
  
  for (i=0;i<NICKHASHSIZE;i++) {
    for (np=nicktable[i];np;np=np->next) {
      np->exts[index]=NULL;
    }
  }
}

/* visiblehostmask
 *  Produces the "apparent" hostmask as seen by network users.
 */

char *visiblehostmask(nick *np, char *buf) {
  char uhbuf[USERLEN+HOSTLEN+2];
  
  visibleuserhost(np, uhbuf);
  sprintf(buf,"%s!%s",np->nick,uhbuf);

  return buf;
}

/* visibleuserhost
 *  As above without nick
 */

char *visibleuserhost(nick *np, char *buf) {
  char hostbuf[HOSTLEN+1];
  char *ident, *host;

  ident=np->ident;
  host=np->host->name->content;
  
  if (IsSetHost(np)) {
    if (np->shident) {
      ident=np->shident->content;
    }
    if (np->sethost) {
      host=np->sethost->content;
    }
  } else if (IsAccount(np) && IsHideHost(np)) {
    sprintf(hostbuf,"%s.%s", np->authname, HIS_HIDDENHOST);
    host=hostbuf;
  }

  sprintf(buf,"%s@%s",ident,host);

  return buf;
}

#if 0

/*
 * gethandlebynumeric:
 *  Given a numeric, gives the location in the servernicks table
 *  where it should be.  Does not check that the nick currently found
 *  there (if any) has the correct numeric; this is left to the
 *  calling function to figure out.
 */
 
nick **gethandlebynumeric(long numeric) {
  int servernum;
  int maskednum;
  server *serv;
  
  /* Shift off the client identifier part of the numeric to get the server ID */
  servernum=(numeric>>18);
  
  if ((serv=getserverdata(servernum))==NULL) {
    Error("nick",ERR_WARNING,"Numeric %ld refers to non-existent server %d",numeric,servernum);
    return NULL;
  }
  
  /* Compute the masked numeric */
  maskednum=numeric&(serv->maxusernum);
  
  return (servernicks[servernum])+maskednum;
}

/*
 * getnickbynumeric[str]() 
 *  These functions retrieve a nick based on it's numeric on the network
 *  Use the approriate function depending on how your numeric is expressed..
 */
 
nick *getnickbynumeric(long numeric) {
  nick **nh;
  
  nh=gethandlebynumeric(numeric);
  
  if ((*nh) && ((*nh)->numeric!=numeric)) {
    /* We found a masked numeric match, but the actual numeric 
     * is different.  This counts as a miss. */
    return NULL;
  }
  
  return (*nh);
}

nick *getnickbynumericstr(char *numericstr) {
  return getnickbynumeric(numerictolong(numericstr,5));
}


#endif

int canseeuser(nick *np, nick *cloaked)
{
  return (np == cloaked ||
          !IsCloaked(cloaked) ||
          np->cloak_extra == cloaked);
}

void addcloaktarget(nick *cloaked, nick *target)
{
  removecloaktarget(target);

  target->cloak_extra = cloaked;
  cloaked->cloak_count++;
}

void removecloaktarget(nick *target)
{
  if (target->cloak_extra) {
    target->cloak_extra->cloak_count--;
    target->cloak_extra = NULL;
  }
}

void clearcloaktargets(nick *cloaked)
{
  nick *tnp;
  int j;

  if (cloaked->cloak_count == 0)
    return;

  for(j=0;j<NICKHASHSIZE;j++)
    for(tnp=nicktable[j];tnp;tnp=tnp->next)
      if (tnp->cloak_extra == cloaked)
        tnp->cloak_extra = NULL;

  cloaked->cloak_count = 0;
}


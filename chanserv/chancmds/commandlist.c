/* Automatically generated by mkcommandlist.pl, do not edit. */

#include "../chanserv.h"

/* Prototypes */
int csc_doaddchan(void *source, int cargc, char **cargv);
int csc_doadduser(void *source, int cargc, char **cargv);
int csc_doautolimit(void *source, int cargc, char **cargv);
int csc_dobanclear(void *source, int cargc, char **cargv);
int csc_dobandel(void *source, int cargc, char **cargv);
int csc_dobanlist(void *source, int cargc, char **cargv);
int csc_dobantimer(void *source, int cargc, char **cargv);
int csc_dochanflags(void *source, int cargc, char **cargv);
int csc_dochanlev(void *source, int cargc, char **cargv);
int csc_dochanmode(void *source, int cargc, char **cargv);
int csc_dochannelcomment(void *source, int cargc, char **cargv);
int csc_dochanstat(void *source, int cargc, char **cargv);
int csc_dochantype(void *source, int cargc, char **cargv);
int csc_doclearchan(void *source, int cargc, char **cargv);
int csc_dodelchan(void *source, int cargc, char **cargv);
int csc_dodeopall(void *source, int cargc, char **cargv);
int csc_dodevoiceall(void *source, int cargc, char **cargv);
int csc_doinvite(void *source, int cargc, char **cargv);
int csc_doop(void *source, int cargc, char **cargv);
int csc_dopermban(void *source, int cargc, char **cargv);
int csc_dorecover(void *source, int cargc, char **cargv);
int csc_dorejoin(void *source, int cargc, char **cargv);
int csc_doremoveuser(void *source, int cargc, char **cargv);
int csc_dorenchan(void *source, int cargc, char **cargv);
int csc_dosettopic(void *source, int cargc, char **cargv);
int csc_dosuspendchan(void *source, int cargc, char **cargv);
int csc_dosuspendchanlist(void *source, int cargc, char **cargv);
int csc_dotempban(void *source, int cargc, char **cargv);
int csc_dounbanall(void *source, int cargc, char **cargv);
int csc_dounbanmask(void *source, int cargc, char **cargv);
int csc_dounbanme(void *source, int cargc, char **cargv);
int csc_dounsuspendchan(void *source, int cargc, char **cargv);
int csc_dovoice(void *source, int cargc, char **cargv);
int csc_dowelcome(void *source, int cargc, char **cargv);

void _init() {
  chanservaddcommand("addchan", QCMD_OPER, 4, csc_doaddchan, "Adds a new channel to the bot.");
  chanservaddcommand("adduser", QCMD_AUTHED, 20, csc_doadduser, "Adds one or more users to a channel as +aot.");
  chanservaddcommand("autolimit", QCMD_AUTHED, 2, csc_doautolimit, "Shows or changes the autolimit threshold on a channel.");
  chanservaddcommand("banclear", QCMD_AUTHED, 1, csc_dobanclear, "Removes all bans from a channel including persistent bans.");
  chanservaddcommand("bandel", QCMD_AUTHED, 2, csc_dobandel, "Removes a single ban from a channel.");
  chanservaddcommand("banlist", QCMD_AUTHED, 1, csc_dobanlist, "Displays all persistent bans on a channel.");
  chanservaddcommand("bantimer", QCMD_AUTHED, 2, csc_dobantimer, "Shows or changes the time after which bans are removed.");
  chanservaddcommand("chanflags", QCMD_AUTHED, 2, csc_dochanflags, "Shows or changes the flags on a channel.");
  chanservaddcommand("chanlev", QCMD_AUTHED, 3, csc_dochanlev, "Shows or modifies user access on a channel.");
  chanservaddcommand("chanmode", QCMD_AUTHED, 4, csc_dochanmode, "Shows which modes are forced or denied on a channel.");
  chanservaddcommand("channelcomment", QCMD_OPER, 2, csc_dochannelcomment, "Shows or changes the staff comment for a channel.");
  chanservaddcommand("chanstat", QCMD_AUTHED, 1, csc_dochanstat, "Displays channel activity statistics.");
  chanservaddcommand("chantype", QCMD_OPER, 2, csc_dochantype, "Shows or changes a channel's type.");
  chanservaddcommand("clearchan", QCMD_AUTHED, 1, csc_doclearchan, "Removes all modes from a channel.");
  chanservaddcommand("delchan", QCMD_OPER, 2, csc_dodelchan, "Removes a channel from the bot.");
  chanservaddcommand("deopall", QCMD_AUTHED, 1, csc_dodeopall, "Deops all users on channel.");
  chanservaddcommand("devoiceall", QCMD_AUTHED, 1, csc_dodevoiceall, "Devoices all users on a channel.");
  chanservaddcommand("invite", QCMD_AUTHED, 1, csc_doinvite, "Invites you to a channel.");
  chanservaddcommand("op", QCMD_AUTHED, 20, csc_doop, "Ops you or other users on channel(s).");
  chanservaddcommand("permban", QCMD_AUTHED, 3, csc_dopermban, "Permanently bans a hostmask on a channel.");
  chanservaddcommand("recover", QCMD_AUTHED, 1, csc_dorecover, "Recovers a channel (same as deopall, unbanall, clearchan).");
  chanservaddcommand("rejoin", QCMD_OPER, 1, csc_dorejoin, "Makes the bot rejoin a channel.");
  chanservaddcommand("removeuser", QCMD_AUTHED, 20, csc_doremoveuser, "Removes one or more users from a channel.");
  chanservaddcommand("renchan", QCMD_OPER, 2, csc_dorenchan, "Renames a channel on the bot.");
  chanservaddcommand("settopic", QCMD_AUTHED, 2, csc_dosettopic, "Changes the topic on a channel.");
  chanservaddcommand("suspendchan", QCMD_OPER, 2, csc_dosuspendchan, "Suspends a channel from the bot.");
  chanservaddcommand("suspendchanlist", QCMD_HELPER, 1, csc_dosuspendchanlist, "Lists suspended channels.");
  chanservaddcommand("tempban", QCMD_AUTHED, 4, csc_dotempban, "Bans a hostmask on a channel for a specified time period.");
  chanservaddcommand("unbanall", QCMD_AUTHED, 1, csc_dounbanall, "Removes all bans from a channel.");
  chanservaddcommand("unbanmask", QCMD_AUTHED, 2, csc_dounbanmask, "Removes bans matching a particular mask from a channel.");
  chanservaddcommand("unbanme", QCMD_AUTHED, 1, csc_dounbanme, "Removes any bans affecting you from a channel.");
  chanservaddcommand("unsuspendchan", QCMD_OPER, 1, csc_dounsuspendchan, "Unsuspends a channel from the bot.");
  chanservaddcommand("voice", QCMD_AUTHED, 20, csc_dovoice, "Voices you or other users on channel(s).");
  chanservaddcommand("welcome", QCMD_AUTHED, 2, csc_dowelcome, "Shows or changes the welcome message on a channel.");
}

void _fini() {
  chanservremovecommand("addchan", csc_doaddchan);
  chanservremovecommand("adduser", csc_doadduser);
  chanservremovecommand("autolimit", csc_doautolimit);
  chanservremovecommand("banclear", csc_dobanclear);
  chanservremovecommand("bandel", csc_dobandel);
  chanservremovecommand("banlist", csc_dobanlist);
  chanservremovecommand("bantimer", csc_dobantimer);
  chanservremovecommand("chanflags", csc_dochanflags);
  chanservremovecommand("chanlev", csc_dochanlev);
  chanservremovecommand("chanmode", csc_dochanmode);
  chanservremovecommand("channelcomment", csc_dochannelcomment);
  chanservremovecommand("chanstat", csc_dochanstat);
  chanservremovecommand("chantype", csc_dochantype);
  chanservremovecommand("clearchan", csc_doclearchan);
  chanservremovecommand("delchan", csc_dodelchan);
  chanservremovecommand("deopall", csc_dodeopall);
  chanservremovecommand("devoiceall", csc_dodevoiceall);
  chanservremovecommand("invite", csc_doinvite);
  chanservremovecommand("op", csc_doop);
  chanservremovecommand("permban", csc_dopermban);
  chanservremovecommand("recover", csc_dorecover);
  chanservremovecommand("rejoin", csc_dorejoin);
  chanservremovecommand("removeuser", csc_doremoveuser);
  chanservremovecommand("renchan", csc_dorenchan);
  chanservremovecommand("settopic", csc_dosettopic);
  chanservremovecommand("suspendchan", csc_dosuspendchan);
  chanservremovecommand("suspendchanlist", csc_dosuspendchanlist);
  chanservremovecommand("tempban", csc_dotempban);
  chanservremovecommand("unbanall", csc_dounbanall);
  chanservremovecommand("unbanmask", csc_dounbanmask);
  chanservremovecommand("unbanme", csc_dounbanme);
  chanservremovecommand("unsuspendchan", csc_dounsuspendchan);
  chanservremovecommand("voice", csc_dovoice);
  chanservremovecommand("welcome", csc_dowelcome);
}

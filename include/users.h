/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#ifndef USERS_H
#define USERS_H

/* Hash maps used for users. Note UserListByUID will not be used on non-TS6 IRCds, and should never
 * be assumed to have users
 */
typedef unordered_map_namespace::unordered_map<Anope::string, User *, hash_compare_ci_string> user_map;
typedef unordered_map_namespace::unordered_map<Anope::string, User *, hash_compare_std_string> user_uid_map;

extern CoreExport user_map UserListByNick;
extern CoreExport user_uid_map UserListByUID;

struct ChannelContainer
{
	Channel *chan;
	Flags<ChannelModeName, CMODE_END> *Status;

	ChannelContainer(Channel *c) : chan(c) { }
	virtual ~ChannelContainer() { }
};

typedef std::list<ChannelContainer *> UChannelList;

/* Online user and channel data. */
class CoreExport User : public Extensible
{
 protected:
	Anope::string vident;
	Anope::string ident;
	Anope::string uid;
	bool OnAccess; /* If the user is on the access list of the nick theyre on */
	Flags<UserModeName, UMODE_END> modes; /* Bitset of mode names the user has set on them */
	std::map<UserModeName, Anope::string> Params; /* Map of user modes and the params this user has */
	NickCore *nc; /* NickCore account the user is currently loggged in as */

 public: // XXX: exposing a tiny bit too much
	Anope::string nick;		/* User's current nick */

	Anope::string host;		/* User's real hostname */
	Anope::string hostip;	/* User's IP number */
	Anope::string vhost;	/* User's virtual hostname */
	Anope::string chost;	/* User's cloaked hostname */
	Anope::string realname;	/* Realname */
	Server *server;			/* Server user is connected to */
	time_t timestamp;		/* Timestamp of the nick */
	time_t my_signon;		/* When did _we_ see the user? */

	int isSuperAdmin;		/* is SuperAdmin on or off? */

	/* Channels the user is in */
	UChannelList chans;

	unsigned short invalid_pw_count;	/* # of invalid password attempts */
	time_t invalid_pw_time;				/* Time of last invalid password */

	time_t lastmemosend;	/* Last time MS SEND command used */
	time_t lastnickreg;		/* Last time NS REGISTER cmd used */
	time_t lastmail;		/* Last time this user sent a mail */

	/****************************************************************/

	/** Create a new user object, initialising necessary fields and
	 * adds it to the hash
	 *
	 * @param nick The nickname of the user.
	 * @param uid The unique identifier of the user.
	 */
	User(const Anope::string &nick, const Anope::string &uid);

	/** Destroy a user.
	 */
	virtual ~User();

	/** Update the nickname of a user record accordingly, should be
	 * called from ircd protocol.
	 */
	virtual void SetNewNick(const Anope::string &newnick);

	/** Update the displayed (vhost) of a user record.
	 * This is used (if set) instead of real host.
	 * @param host The new displayed host to give the user.
	 */
	void SetDisplayedHost(const Anope::string &host);

	/** Get the displayed vhost of a user record.
	 * @return The displayed vhost of the user, where ircd-supported, or the user's real host.
	 */
	const Anope::string GetDisplayedHost() const;

	/** Update the cloaked host of a user
	 * @param host The cloaked host
	 */
	void SetCloakedHost(const Anope::string &newhost);

	/** Get the cloaked host of a user
	 * @return The cloaked host
	 */
	const Anope::string &GetCloakedHost() const;

	/** Retrieves the UID of the user, where applicable, if set.
	 * This is not used on some IRCds, but is for a lot e.g. P10, TS6 protocols.
	 * @return The UID of the user.
	 */
	const Anope::string &GetUID() const;

	/** Update the displayed ident (username) of a user record.
	 * @param ident The new ident to give this user.
	 */
	void SetVIdent(const Anope::string &ident);

	/** Get the displayed ident (username) of this user.
	 * @return The displayed ident of this user.
	 */
	const Anope::string &GetVIdent() const;

	/** Update the real ident (username) of a user record.
	 * @param ident The new ident to give this user.
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivilants.
	 */
	void SetIdent(const Anope::string &ident);

	/** Get the real ident (username) of this user.
	 * @return The displayed ident of this user.
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivilants.
	 */
	const Anope::string &GetIdent() const;

	/** Get the full mask ( nick!ident@realhost ) of a user
	 */
	const Anope::string GetMask();

	/** Updates the realname of the user record.
	 */
	void SetRealname(const Anope::string &realname);

	/**
	 * Send a message (notice or privmsg, depending on settings) to a user
	 * @param source Sender nick
	 * @param fmt Format of the Message
	 * @param ... any number of parameters
	 */
	virtual void SendMessage(const Anope::string &source, const char *fmt, ...);
	virtual void SendMessage(const Anope::string &source, const Anope::string &msg);

	/** Collide a nick
	 * See the comment in users.cpp
	 * @param na The nick
	 */
	void Collide(NickAlias *na);

	/** Check if the user should become identified because
	 * their svid matches the one stored in their nickcore
	 * @param svid Services id
	 */
	void CheckAuthenticationToken(const Anope::string &svid);

	/** Auto identify the user to the given accountname.
	 * @param account Display nick of account
	 */
	void AutoID(const Anope::string &account);

	/** Login the user to a NickCore
	 * @param core The account the user is useing
	 */
	void Login(NickCore *core);

	/** Logout the user
	 */
	void Logout();

	/** Get the account the user is logged in using
	 * @reurn The account or NULL
	 */
	virtual NickCore *Account();

	/** Check if the user is identified for their nick
	 * @param CheckNick True to check if the user is identified to the nickname they are on too
	 * @return true or false
	 */
	virtual bool IsIdentified(bool CheckNick = false) const;

	/** Check if the user is recognized for their nick (on the nicks access list)
	 * @param CheckSecure Only returns true if the user has secure off
	 * @return true or false
	 */
	virtual bool IsRecognized(bool CheckSecure = false) const;

	/** Update the last usermask stored for a user, and check to see if they are recognized
	 */
	void UpdateHost();

	/** Check if the user has a mode
	 * @param Name Mode name
	 * @return true or false
	 */
	bool HasMode(UserModeName Name) const;

	/** Set a mode internally on the user, the IRCd is not informed
	 * @param um The user mode
	 * @param Param The param, if there is one
	 */
	void SetModeInternal(UserMode *um, const Anope::string &Param = "");

	/** Remove a mode internally on the user, the IRCd is not informed
	 * @param um The user mode
	 */
	void RemoveModeInternal(UserMode *um);

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 * @param Param Optional param for the mode
	 */
	void SetMode(BotInfo *bi, UserMode *um, const Anope::string &Param = "");

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param Name The mode name
	 * @param Param Optional param for the mode
	 */
	void SetMode(BotInfo *bi, UserModeName Name, const Anope::string &Param = "");

	/* Set a mode on the user
	 * @param bi The client setting the mode
	 * @param ModeChar The mode char
	 * @param param Optional param for the mode
	 */
	void SetMode(BotInfo *bi, char ModeChar, const Anope::string &Param = "");

	/** Remove a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 */
	void RemoveMode(BotInfo *bi, UserMode *um);

	/** Remove a mode from the user
	 * @param bi The client setting the mode
	 * @param Name The mode name
	 */
	void RemoveMode(BotInfo *bi, UserModeName Name);

	/** Remove a mode from the user
	 * @param bi The client setting the mode
	 * @param ModeChar The mode char
	 */
	void RemoveMode(BotInfo *bi, char ModeChar);

	/** Set a string of modes on a user
	 * @param bi The client setting the mode
	 * @param umodes The modes
	 */
	void SetModes(BotInfo *bi, const char *umodes, ...);

	/** Find the channel container for Channel c that the user is on
	 * This is preferred over using FindUser in Channel, as there are usually more users in a channel
	 * than channels a user is in
	 * @param c The channel
	 * @return The channel container, or NULL
	 */
	ChannelContainer *FindChannel(Channel *c);

	/** Check if the user is protected from kicks and negative mode changes
	 * @return true or false
	 */
	bool IsProtected() const;
};

#endif // USERS_H

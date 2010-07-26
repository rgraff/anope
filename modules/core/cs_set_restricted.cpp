/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */
/*************************************************************************/

#include "module.h"

class CommandCSSetRestricted : public Command
{
 public:
	CommandCSSetRestricted(const Anope::string &cname, const Anope::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_RESTRICTED);
			if (ci->levels[CA_NOJOIN] < 0)
				ci->levels[CA_NOJOIN] = 0;
			notice_lang(Config.s_ChanServ, u, CHAN_SET_RESTRICTED_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_RESTRICTED);
			if (ci->levels[CA_NOJOIN] >= 0)
				ci->levels[CA_NOJOIN] = -2;
			notice_lang(Config.s_ChanServ, u, CHAN_SET_RESTRICTED_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "RESTRICTED");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_RESTRICTED, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET RESTRICTED", CHAN_SET_RESTRICTED_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_RESTRICTED);
	}
};

class CommandCSSASetRestricted : public CommandCSSetRestricted
{
 public:
	CommandCSSASetRestricted(const Anope::string &cname) : CommandCSSetRestricted(cname, "chanserv/saset/restricted")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_RESTRICTED, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SASET RESTRICTED", CHAN_SASET_RESTRICTED_SYNTAX);
	}
};

class CSSetRestricted : public Module
{
 public:
	CSSetRestricted(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetRestricted("RESTRICTED"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetRestricted("RESTRICTED"));
	}

	~CSSetRestricted()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("RESTRICTED");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("RESTRICTED");
	}
};

MODULE_INIT(CSSetRestricted)

/* NickServ core functions
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

class CommandNSSetGreet : public Command
{
 public:
	CommandNSSetGreet(const Anope::string &cname) : Command(cname, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		if (!params.empty())
		{
			u->Account()->greet = params[0];
			notice_lang(Config.s_NickServ, u, NICK_SET_GREET_CHANGED, u->Account()->greet.c_str());
		}
		else
		{
			u->Account()->greet.clear();
			notice_lang(Config.s_NickServ, u, NICK_SET_GREET_UNSET);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_GREET);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_GREET);
	}
};

class CommandNSSASetGreet : public Command
{
 public:
	CommandNSSASetGreet(const Anope::string &cname) : Command(cname, 1, 2, "nickserv/saset/greet")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (!param.empty())
		{
			nc->greet = param;
			notice_lang(Config.s_NickServ, u, NICK_SASET_GREET_CHANGED, nc->display.c_str(), nc->greet.c_str());
		}
		else
		{
			nc->greet.clear();
			notice_lang(Config.s_NickServ, u, NICK_SASET_GREET_UNSET, nc->display.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_GREET);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_GREET);
	}
};

class NSSetGreet : public Module
{
 public:
	NSSetGreet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(new CommandNSSetGreet("GREET"));

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetGreet("GREET"));
	}

	~NSSetGreet()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand("GREET");

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("GREET");
	}
};

MODULE_INIT(NSSetGreet)

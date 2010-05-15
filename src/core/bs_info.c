/* BotServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandBSInfo : public Command
{
 private:
	void send_bot_channels(User * u, BotInfo * bi)
	{
		char buf[307], *end;

		*buf = 0;
		end = buf;

		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(); it != RegisteredChannelList.end(); ++it)
		{
			ChannelInfo *ci = it->second;

			if (ci->bi == bi)
			{
				if (strlen(buf) + strlen(ci->name.c_str()) > 300)
				{
					u->SendMessage(Config.s_BotServ, "%s", buf);
					*buf = 0;
					end = buf;
				}
				end += snprintf(end, sizeof(buf) - (end - buf), " %s ", ci->name.c_str());
			}
		}

		if (*buf)
			u->SendMessage(Config.s_BotServ, "%s", buf);
		return;
	}
 public:
	CommandBSInfo() : Command("INFO", 1, 1)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		BotInfo *bi;
		ChannelInfo *ci;
		const char *query = params[0].c_str();

		int need_comma = 0;
		char buf[BUFSIZE], *end;
		const char *commastr = getstring(u, COMMA_SPACE);

		if ((bi = findbot(query)))
		{
			struct tm *tm;

			notice_lang(Config.s_BotServ, u, BOT_INFO_BOT_HEADER, bi->nick.c_str());
			notice_lang(Config.s_BotServ, u, BOT_INFO_BOT_MASK, bi->user.c_str(), bi->host.c_str());
			notice_lang(Config.s_BotServ, u, BOT_INFO_BOT_REALNAME, bi->real.c_str());
			tm = localtime(&bi->created);
			strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
			notice_lang(Config.s_BotServ, u, BOT_INFO_BOT_CREATED, buf);
			notice_lang(Config.s_BotServ, u, BOT_INFO_BOT_OPTIONS, getstring(u, (bi->HasFlag(BI_PRIVATE) ? BOT_INFO_OPT_PRIVATE : BOT_INFO_OPT_NONE)));
			notice_lang(Config.s_BotServ, u, BOT_INFO_BOT_USAGE, bi->chancount);

			if (u->Account()->HasPriv("botserv/administration"))
				this->send_bot_channels(u, bi);
		}
		else if ((ci = cs_findchan(query)))
		{
			if (!IsFounder(u, ci) && !u->Account()->HasPriv("botserv/administration"))
			{
				notice_lang(Config.s_BotServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}

			notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_HEADER, ci->name.c_str());
			if (ci->bi)
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_BOT, ci->bi->nick.c_str());
			else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_BOT_NONE);

			if (ci->botflags.HasFlag(BS_KICK_BADWORDS)) {
				if (ci->ttb[TTB_BADWORDS])
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_BADWORDS]);
				else
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags.HasFlag(BS_KICK_BOLDS)) {
				if (ci->ttb[TTB_BOLDS])
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_BOLDS]);
				else
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags.HasFlag(BS_KICK_CAPS)) {
				if (ci->ttb[TTB_CAPS])
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_CAPS], ci->capsmin,
								ci->capspercent);
				else
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_ON,
								getstring(u, BOT_INFO_ACTIVE), ci->capsmin,
								ci->capspercent);
			} else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_OFF,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags.HasFlag(BS_KICK_COLORS)) {
				if (ci->ttb[TTB_COLORS])
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_COLORS]);
				else
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags.HasFlag(BS_KICK_FLOOD)) {
				if (ci->ttb[TTB_FLOOD])
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_FLOOD], ci->floodlines,
								ci->floodsecs);
				else
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_ON,
								getstring(u, BOT_INFO_ACTIVE),
								ci->floodlines, ci->floodsecs);
			} else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_OFF,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags.HasFlag(BS_KICK_REPEAT)) {
				if (ci->ttb[TTB_REPEAT])
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_REPEAT], ci->repeattimes);
				else
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_ON,
								getstring(u, BOT_INFO_ACTIVE),
								ci->repeattimes);
			} else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_OFF,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags.HasFlag(BS_KICK_REVERSES)) {
				if (ci->ttb[TTB_REVERSES])
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_REVERSES]);
				else
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags.HasFlag(BS_KICK_UNDERLINES)) {
				if (ci->ttb[TTB_UNDERLINES])
					notice_lang(Config.s_BotServ, u,
								BOT_INFO_CHAN_KICK_UNDERLINES_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_UNDERLINES]);
				else
					notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_UNDERLINES,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_KICK_UNDERLINES,
							getstring(u, BOT_INFO_INACTIVE));

			end = buf;
			*end = 0;
			if (ci->botflags.HasFlag(BS_DONTKICKOPS)) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s",
								getstring(u, BOT_INFO_OPT_DONTKICKOPS));
				need_comma = 1;
			}
			if (ci->botflags.HasFlag(BS_DONTKICKVOICES)) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_DONTKICKVOICES));
				need_comma = 1;
			}
			if (ci->botflags.HasFlag(BS_FANTASY)) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_FANTASY));
				need_comma = 1;
			}
			if (ci->botflags.HasFlag(BS_GREET)) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_GREET));
				need_comma = 1;
			}
			if (ci->botflags.HasFlag(BS_NOBOT)) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_NOBOT));
				need_comma = 1;
			}
			if (ci->botflags.HasFlag(BS_SYMBIOSIS)) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_SYMBIOSIS));
				need_comma = 1;
			}
			notice_lang(Config.s_BotServ, u, BOT_INFO_CHAN_OPTIONS,
						*buf ? buf : getstring(u, BOT_INFO_OPT_NONE));

		} else
			notice_lang(Config.s_BotServ, u, BOT_INFO_NOT_FOUND, query);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_BotServ, u, BOT_HELP_INFO);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_BotServ, u, "INFO", BOT_INFO_SYNTAX);
	}
};

class BSInfo : public Module
{
 public:
	BSInfo(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(BotServ, new CommandBSInfo());

		ModuleManager::Attach(I_OnBotServHelp, this);
	}
	void OnBotServHelp(User *u)
	{
		notice_lang(Config.s_BotServ, u, BOT_HELP_CMD_INFO);
	}
};

MODULE_INIT(BSInfo)

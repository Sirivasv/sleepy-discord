#include "client.h"
#include "discord_object_interface.h"

namespace SleepyDiscord {
	void BaseDiscordClient::testFunction(std::string teststring) {
		request(Post, path("guilds/{guild.id}/roles", std::string("202917641101246465")), json::createJSON({
				{"color", json::UInteger(0x1000000)}
		}));
	}
	//
	//channel functions
	//
	Message BaseDiscordClient::sendMessage(std::string channel_id, std::string message, bool tts) {
		return request<Message>(Post, path("channels/{channel.id}/messages", channel_id), "{\"content\":\"" + message + (tts ? "\",\"tts\":\"true\"" : "\"") + "}");
	}

	Message BaseDiscordClient::uploadFile(std::string channel_id, std::string fileLocation, std::string message) {
		auto r = request(Post, "channels/" + channel_id + "/messages",
			{ { "content", message },
			{ "file", filePathPart{fileLocation} }
		}
		);
		return Message(&r.text);
	}

	Message BaseDiscordClient::editMessage(std::string channel_id, std::string message_id, std::string newMessage) {
		return request<Message>(Patch, path("channels/{channel.id}/messages/{message.id}", channel_id, message_id), "{\"content\": \"" + newMessage + "\"}");
	}

	bool BaseDiscordClient::deleteMessage(const std::string channel_id, const std::string * message_id, const unsigned int numOfMessages) {
		if (numOfMessages == 1) return request(Delete, path("channels/{channel.id}/messages/{message.id}", channel_id, *message_id)).statusCode == NO_CONTENT;
		std::string JSON = "{\"messages\":[";
		for (unsigned int i = 0; i < numOfMessages; ++i) {
			JSON += message_id[i];
		}
		JSON += "]}";
		return request(Post, path("channels/{channel.id}/messages/bulk-delete", channel_id), JSON).statusCode == NO_CONTENT;
	}

	Channel BaseDiscordClient::editChannel(std::string channel_id, std::string name, std::string topic) {
		std::string json = "{";
		if (name != "")
			json += "\"name\":\"" + name + "\",";
		if (topic != "")
			json += "\"topic\":\"" + topic + "\",";
		json[json.length() - 1] = '}';
		auto r = request(Patch, path("channels/{channel.id}", channel_id), json);
		return Channel(&r.text);
	}

	Channel BaseDiscordClient::editChannelName(std::string channel_id, std::string name) {
		return editChannel(channel_id, name);
	}

	Channel BaseDiscordClient::editChannelTopic(std::string channel_id, std::string topic) {
		return editChannel(channel_id, "", topic);
	}

	Channel BaseDiscordClient::deleteChannel(std::string channel_id) {
		return request<Channel>(Delete, path("channels/{channel.id}", channel_id));
	}

	Channel BaseDiscordClient::getChannel(std::string channel_id) {
		return request<Channel>(Get, path("channels/{channel.id}", channel_id));
	}

	json::RawJSONArrayWrapper<Message> BaseDiscordClient::getMessages(std::string channel_id, GetMessagesKey when, std::string message_id, uint8_t _limit) {
		const uint8_t trueLimit = 100 < _limit ? 100 : _limit;
		std::string key;
		switch (when) {
		case around: key = "?around=" + message_id; break;
		case before: key = "?before=" + message_id; break;
		case after:  key = "?after="  + message_id; break;
		case limit:  key = "?"                    ; break;
		default:     key = ""                     ; break;
		}
		if (trueLimit != 0 && when != limit) key += '&';
		return request(Get,
			path("channels/{channel.id}/messages{key}{limit}", channel_id, key,
			(trueLimit != 0 ? "limit=" + std::to_string(trueLimit) : ""))).text;
	}

	Message BaseDiscordClient::getMessage(std::string channel_id, std::string message_id) {
		return request<Message>(Get, path("channels/{channel.id}/messages/{message.id}", channel_id, message_id));
	}

	bool BaseDiscordClient::addReaction(std::string channel_id, std::string message_id, std::string emoji) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/@me", channel_id, message_id, emoji)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::removeReaction(std::string channel_id, std::string message_id, std::string emoji, std::string user_id) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/{user.id}", channel_id, message_id, emoji, user_id)).statusCode == NO_CONTENT;
	}

	std::vector<Reaction> BaseDiscordClient::getReactions(std::string channel_id, std::string message_id, std::string emoji) {
		return requestVector<Reaction>(Get, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}", channel_id, message_id, emoji));
	}

	void BaseDiscordClient::removeAllReactions(std::string channel_id, std::string message_id) {
		request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions", channel_id, message_id));
	}

	bool BaseDiscordClient::editChannelPermissions(std::string channel_id, std::string id, int allow, int deny, std::string type) {
		return request(
			Put,
			path("channels/{channel.id}/permissions/{overwrite.id}", channel_id, id),
			json::createJSON({
				{ "allow", std::to_string(allow) },
				{ "deny", std::to_string(deny) },
				{ "type", json::string(type) }
		})).statusCode == NO_CONTENT;
	}

	std::vector<Invite> BaseDiscordClient::getChannelInvites(std::string channel_id) {
		return requestVector<Invite>(Get, path("channels/{channel.id}/invites", channel_id));
	}

	Invite BaseDiscordClient::createChannelInvite(std::string channel_id, const uint64_t maxAge, const uint64_t maxUses, const bool temporary, const bool unique) {
		return request<Invite>(Post, path("channels/{channel.id}/invites"),
			json::createJSON({
				{"max_age", json::optionalUInteger(maxAge) },
				{"max_uses", json::optionalUInteger(maxUses) },
				{ "temporary", temporary ? "true" : ""},
				{ "unique", unique ? "true" : ""}
			}));
	}

	bool BaseDiscordClient::removeChannelPermission(std::string channel_id, std::string id) {
		return request(Delete, path("channels/{channel.id}/permissions/{overwrite.id}", channel_id, id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::sendTyping(std::string channel_id) {
		return request(Post, path("channels/{channel.id}/typing", channel_id)).statusCode == NO_CONTENT;
	}

	std::vector<Message> BaseDiscordClient::getPinnedMessages(std::string channel_id) {
		return requestVector<Message>(Get, path("channels/{channel.id}/pins", channel_id));
	}

	bool BaseDiscordClient::pinMessage(std::string channel_id, std::string message_id) {
		return request(Put, path("channels/{channel.id}/pins/{message.id}", channel_id, message_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::unpinMessage(std::string channel_id, std::string message_id) {
		return request(Delete, path("channels/{channel.id}/pins/{message.id}", channel_id, message_id)).statusCode == NO_CONTENT;
	}

	void BaseDiscordClient::addRecipient(std::string channel_id, std::string user_id) {
		request(Put, path("channels/{channel.id}/recipients/{user.id}", channel_id, user_id));
	}

	void BaseDiscordClient::removeRecipient(std::string channel_id, std::string user_id) {
		request(Delete, path("channels/{channel.id}/recipients/{user.id}", channel_id, user_id));
	}

	//
	//server functions
	//
	Channel BaseDiscordClient::createTextChannel(std::string server_id, std::string name) {
		return request<Channel>(Post, path("guilds/{guild.id}/channels", server_id), "{\"name\": \"" + name + "\", \"type\": \"text\"}");
	}

	std::vector<Channel> BaseDiscordClient::editChannelPositions(std::string server_id, std::vector<std::pair<std::string, uint64_t>> positions) {
		return requestVector<Channel>(Patch, path("guilds/{guild.id}/channels", server_id), getEditPositionString(positions));
	}

	ServerMember SleepyDiscord::BaseDiscordClient::getMember(std::string server_id, std::string user_id) {
		return request<ServerMember>(Get, path("guilds/{guild.id}/members/{user.id}", server_id, user_id));
	}

	std::vector<ServerMember> BaseDiscordClient::listMembers(std::string server_id, uint16_t limit, std::string after) {
		return requestVector<ServerMember>(Get, path("guilds/{guild.id}/members", server_id), json::createJSON({
			{ "limit", json::optionalUInteger(limit) },
			{ "after", json::string(after) }
		}));
	}

	ServerMember BaseDiscordClient::addMember(std::string server_id, std::string user_id, std::string accesToken, std::string nick, std::vector<Role> roles, bool mute, bool deaf) {
		std::string rolesString = "";
		if (roles.empty()){
			rolesString = "";
		} else {
			unsigned int i = -1;
			std::vector<std::string> values(roles.size());
			for (Role role : roles) {
				values[++i] = json::createJSON({
					{ "id"         , json::string  (role.id         ) },
					{ "name"       , json::string  (role.name       ) },
					{ "color"      , json::UInteger(role.color      ) },
					{ "hoist"      , json::boolean (role.hoist      ) },
					{ "position"   , json::integer (role.position   ) },
					{ "managed"    , json::boolean (role.managed    ) },
					{ "mentionable", json::boolean (role.mantionable) }
				});
			}
			rolesString = json::createJSONArray(values);
		}

		return request<ServerMember>(Put, path("guilds/{guild.id}/members/{user.id}"), json::createJSON({
			{ "access_token", json::string (accesToken) },
			{ "nick"        , json::string (nick)       },
			{ "roles"       , rolesString               },
			{ "mute"        , json::boolean(mute)       },
			{ "deaf"        , json::boolean(deaf)       }
		}));
	}

	std::vector<Role> BaseDiscordClient::editRolePosition(std::string server_id, std::vector<std::pair<std::string, uint64_t>> positions) {
		return requestVector<Role>(Patch, path("guilds/{guild.id}/roles", server_id), getEditPositionString(positions));
	}

	std::string BaseDiscordClient::editRole(std::string server_id, std::string role_id, std::string name, Permission permissions, uint32_t color, int position, uint8_t hoist, uint8_t mentionable) {
		const std::string colorString       = color       >> 24 == 0 ? std::to_string(color      ) : "";	//if over 24 bits, do not change color
		const std::string hoistString       = hoist       >> 1  == 0 ? json::boolean (hoist      ) : "";	//if larger then 1 bit, do change hoist
		const std::string mentionableString = mentionable >> 1  == 0 ? json::boolean (mentionable) : "";

		return request(Patch, path("guilds/{guild.id}/roles/{role.id}", server_id, role_id), json::createJSON({
			{ "name"       , json::string(name)         },
			{ "permissions", json::integer(permissions) },
			{ "color"      , colorString                },
			{ "hoist"      , hoistString                },
			{ "mentionable", mentionableString          }
		})).text;
	}

	bool SleepyDiscord::BaseDiscordClient::deleteRole(std::string server_id, std::string role_id) {
		return request(Delete, path("guilds/{guild.id}/roles/{role.id}", server_id, role_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::muteServerMember(std::string server_id, std::string user_id, bool mute) {
		return request(Patch, path("guilds/{guild.id}/members/{user.id}", server_id, user_id), mute ? "{\"mute\":true}" : "{\"mute\":false}").statusCode == NO_CONTENT;
	}

	Server BaseDiscordClient::getServer(std::string server_id) {
		return request<Server>(Get, path("guilds/{guild.id}", server_id));
	}

	Server BaseDiscordClient::deleteServer(std::string server_id) {
		return request<Server>(Delete, path("guilds/{guild.id}", server_id));
	}

	std::vector<Channel> SleepyDiscord::BaseDiscordClient::GetServerChannels(std::string server_id) {
		return requestVector<Channel>(Get, path("guilds/{guild.id}/channels", server_id));
	}

	bool BaseDiscordClient::editNickname(std::string server_id, std::string newNickname) {
		return request(Patch, path("guilds/{guild.id}/members/@me/nick", server_id), "{\"nick\":\"" + newNickname + "\"}").statusCode == OK;
	}

	bool BaseDiscordClient::addRole(std::string server_id, std::string member_id, std::string role_id) {
		return request(Put, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", server_id, member_id, role_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::removeRole(std::string server_id, std::string member_id, std::string role_id) {
		return request(Delete, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", server_id, member_id, role_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::kickMember(std::string server_id, std::string member_id) {
		return request(Delete, path("guilds/{guild.id}/members/{user.id}", server_id, member_id)).statusCode == NO_CONTENT;
	}

	std::vector<User> BaseDiscordClient::getBans(std::string server_id) {
		return requestVector<User>(Get, path("guilds/{guild.id}/bans", server_id));
	}

	bool BaseDiscordClient::banMember(std::string server_id, std::string member_id) {
		return request(Put, path("guilds/{guild.id}/bans/{user.id}", server_id, member_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::unbanMember(std::string server_id, std::string member_id) {
		return request(Delete, path("guilds/{guild.id}/bans/{user.id}", server_id, member_id)).statusCode == NO_CONTENT;
	}

	std::vector<Role> BaseDiscordClient::getRoles(std::string server_id) {
		return requestVector<Role>(Get, path("guilds/{guild.id}/roles", server_id));
	}

	Role BaseDiscordClient::createRole(std::string server_id, std::string name, Permission permissions, unsigned int color, bool hoist, bool mentiionable) {
		return request<Role>(Post, path("guilds/{guild.id}/roles", server_id), json::createJSON({
			{ "name"       , json::string (name        ) },
			{ "permissions", json::integer(permissions ) },
			{ "color"      , json::integer(color       ) },
			{ "hoist"      , json::boolean(hoist       ) },
			{ "mentionable", json::boolean(mentiionable) }
		}));
	}

	void BaseDiscordClient::pruneMembers(std::string server_id, const unsigned int numOfDays) {
		if (numOfDays == 0) return;
		request(Post, path("guilds/{guild.id}/prune", server_id), "{\"days\":" + numOfDays + '}');
	}

	std::vector<VoiceRegion> BaseDiscordClient::getVoiceRegions() {
		return requestVector<VoiceRegion>(Get, path("guilds/{guild.id}/regions"));
	}

	std::vector<Invite> BaseDiscordClient::getServerInvites(std::string server_id) {
		return requestVector<Invite>(Get, path("guilds/{guild.id}/invites", server_id));
	}

	std::string BaseDiscordClient::getIntegrations(std::string server_id) {
		return request(Get, path("guilds/{guild.id}/integrations", server_id)).text;
	}

	bool BaseDiscordClient::createIntegration(std::string server_id, std::string type, std::string integration_id) {
		return request(Post, path("guilds/{guild.id}/integrations", server_id), json::createJSON({
			{ "type", json::string(type) },
			{ "id", json::string(integration_id) }
		})).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::editIntergration(std::string server_id, std::string integration_id, int expireBegavior, int expireGracePeriod, bool enbleEmoticons) {
		return request(Patch, path("guilds/{guild.id}/integrations/{integration.id}", server_id, integration_id), json::createJSON({
			{ "expire_behavior", json::integer(expireBegavior) },
			{ "expire_grace_period", json::integer(expireGracePeriod) },
			{ "enable_emoticons", json::boolean(enbleEmoticons) }
		})).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::deleteIntegration(std::string server_id, std::string integration_id) {
		return request(Delete, path("guilds/{guild.id}/integrations/{integration.id}", server_id, integration_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::syncIntegration(std::string server_id, std::string integration_id) {
		return request(Post, path("guilds/{guild.id}/integrations/{integration.id}/sync", server_id, integration_id)).statusCode == NO_CONTENT;
	}
	ServerEmbed BaseDiscordClient::getServerEmbed(std::string server_id) {
		return request<ServerEmbed>(Get, path("guilds/{guild.id}/embed", server_id));
	}
	//
	//Invite functions
	//
	Invite BaseDiscordClient::inviteEndpoint(RequestMethod method, std::string inviteCode) {
		return request<Invite>(method, path("invites/{invite.code}", inviteCode));
	}

	Invite BaseDiscordClient::getInvite(std::string inviteCode) {
		return inviteEndpoint(Get, inviteCode);
	}

	Invite BaseDiscordClient::deleteInvite(std::string inviteCode) {
		return inviteEndpoint(Delete, inviteCode);
	}

	Invite BaseDiscordClient::acceptInvite(std::string inviteCode) {
		return inviteEndpoint(Post, inviteCode);
	}
	//
	//User functions
	//
	User BaseDiscordClient::getCurrentUser() {
		return request<User>(Get, "users/@me");
	}

	User BaseDiscordClient::getUser(std::string user_id) {
		return request<User>(Get, path("users/{user.id}", user_id));
	}

	UserServer BaseDiscordClient::getServers() {
		return request<UserServer>(Get, "users/@me/guilds");
	}

	bool BaseDiscordClient::leaveServer(std::string server_id) {
		return request(Delete, path("users/@me/guilds/{guild.id}", server_id)).statusCode == NO_CONTENT;
	}

	std::vector<DMChannel> BaseDiscordClient::getDirectMessageChannels() {
		return requestVector<DMChannel>(Get, "users/@me/channels");
	}

	DMChannel BaseDiscordClient::createDirectMessageChannel(std::string recipient_id) {
		return request<DMChannel>(Post, "users/@me/channels", json::createJSON({ { "recipient_id", recipient_id } }));
	}

	std::vector<Connection> BaseDiscordClient::getUserConnections() {
		return requestVector<Connection>(Get, "users/@me/connections");
	}

	//
	//Webhook functions
	//
	Webhook BaseDiscordClient::createWebhook(std::string channel_id, std::string name, std::string avatar) {
		return request<Webhook>(Post, path("channels/{channel.id}/webhooks", channel_id), json::createJSON({
			{"name", json::string(name)},
			{"avatar", avatar}
		}));
	}

	std::vector<Webhook> BaseDiscordClient::getChannelWebhooks(std::string channel_id) {
		return requestVector<Webhook>(Get, path("channels/{channel.id}/webhooks", channel_id));
	}

	std::vector<Webhook> BaseDiscordClient::getServerWebhooks(std::string server_id) {
		return requestVector<Webhook>(Get, path("guilds/{guild.id}/webhooks", server_id));
	}

	inline const char* optionalWebhookToken(std::string webhookToken) {
		return webhookToken != "" ? "webhooks/{webhook.id}/{webhook.token}" : "webhooks/{webhook.id}";
	}

	Webhook BaseDiscordClient::getWebhook(std::string webhook_id, std::string webhookToken) {
		return request<Webhook>(Get, path(optionalWebhookToken(webhookToken), webhook_id, webhookToken));
	}

	Webhook BaseDiscordClient::editWebhook(std::string webhook_id, std::string webhookToken, std::string name, std::string avatar) {
		return request<Webhook>(Patch, path(optionalWebhookToken(webhookToken), webhook_id, webhookToken), json::createJSON({
			{ "name", json::string(name) },
			{ "avatar", json::string(avatar) }
		}));
	}

	bool BaseDiscordClient::deleteWebhook(std::string webhook_id, std::string webhookToken) {
		return request(Delete, path(optionalWebhookToken(webhookToken), webhook_id, webhookToken)).statusCode == NO_CONTENT;
	}

	//excute webhook

	Webhook BaseDiscordClient::requestExecuteWebhook(std::string webhook_id, std::string webhookToken, std::pair<std::string, std::string> pair, bool wait, std::string username, std::string avatar_url, bool tts) {
		return request<Webhook>(Post, path("webhooks/{webhook.id}/{webhook.token}{wait}", webhook_id, webhookToken, (wait ? "?around=true" : "")), json::createJSON({
			pair,
			{ "username"  , json::string(username  ) },
			{ "avatar_url", json::string(avatar_url) },
			{ "tts"       , (tts ? "true" : "")      }
		}));
	}

	Webhook BaseDiscordClient::executeWebhook(std::string webhook_id, std::string webhookToken, std::string content, bool wait, std::string username, std::string avatar_url, bool tts) {
		return requestExecuteWebhook(webhook_id, webhookToken, { "content", json::string(content) }, wait, username, avatar_url, tts);
	}

	//Webhook SleepyDiscord::BaseDiscordClient::executeWebhook(std::string webhook_id, std::string webhookToken, std::vector<Embed> embeds, bool wait, std::string username, std::string avatar_url bool tts) {
	//	
	//	return requestExecuteWebhook(webhook_id, webhookToken, { "embeds", crazy stuff happens here }, wait, username, avatar_url, tts);
	//}
	
	Webhook BaseDiscordClient::executeWebhook(std::string webhook_id, std::string webhookToken, filePathPart file, bool wait, std::string username, std::string avatar_url, bool tts) {
		std::string response = request(Post, path("webhooks/{webhook.id}/{webhook.token}", webhook_id, webhookToken), {
			{ "file"      , filePathPart(file)  },
			{ "username"  , username            },
			{ "avatar_url", avatar_url          },
			{ "tts"       , (tts ? "true" : "") }
		}).text;
		return Webhook(&response);
	}
}
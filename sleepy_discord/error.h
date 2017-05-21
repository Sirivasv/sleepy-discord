#pragma once

namespace SleepyDiscord {
	enum ErrorCode {
		OTHER               = 0,

		//HTTP Response/Error Codes
		OK                  = 200, //The request completed successfully
		CREATED             = 201, //The entity was created successfully
		NO_CONTENT          = 204, //The request completed successfully but returned no content
		NOT_MODIFIED        = 304, //The entity was not modified(no action was taken)
		BAD_REQUEST         = 400, //The request was improperly formatted, or the server couldn't understand it
		UNAUTHORIZED        = 401, //The Authorization header was missing or invalid
		FORBIDDEN           = 403, //The Authorization token you passed did not have permission to the resource
		NOT_FOUND           = 404, //The resource at the location specified doesn't exist
		METHOD_NOT_ALLOWED  = 405, //The HTTP method used is not valid for the location specified
		TOO_MANY_REQUESTS   = 429, //You've made too many requests, see Rate Limiting https://discordapp.com/developers/docs/reference#rate-limiting
		GATEWAY_UNAVAILABLE = 502,  //There was not a gateway available to process your request.Wait a bit and retry
		//SERVER_ERROR	    = 5xx  //The server had an error processing your request(these are rare)

		//JSON Error Response
		//I'll add them later

		//Disconnections
		UNKNOWN_ERROR           = 4000, //We're not sure what went wrong. Try reconnecting?
		UNKNOWN_OPCODE          = 4001, //You sent an invalid Gateway OP Code.Don't do that!
		DECODE_ERROR            = 4002, //You sent an invalid payload to us.Don't do that!
		NOT_AUTHENTICATED       = 4003, //You sent us a payload prior to identifying.
		AUTHENTICATION_FAILED   = 4004, //The account token sent with your identify payload is incorrect.
		ALREADY_AUTHENTICATED   = 4005, //You sent more than one identify payload.Don't do that!
		INVALID_SEQ             = 4007, //The sequence sent when resuming the session was invalid.Reconnect and start a new session.
		RATE_LIMITED            = 4008, //Woah nelly!You're sending payloads to us too quickly. Slow it down!
		SESSION_TIMEOUT         = 4009, //Your session timed out.Reconnect and start a new one.
		INVALID_SHARD           = 4010, //You sent us an invalid shard when identifying.
		SHARDING_REQUIRED       = 4011, //The session would have handled too many guilds - you are required to shard your connection in order to connect.
	};
}
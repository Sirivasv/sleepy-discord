#include "sleepy_discord/websocketpp_websocket.h"
#include <thread>

class GameClient : public SleepyDiscord::DiscordClient {
private:
	enum State {
		NA        = 0,
		GET_READY = 1,	//game is starting
		SHOOT     = 2,	//waiting for answer
		ENDED     = 3
	};
	struct Game {
		SleepyDiscord::User player;
		std::thread thread;
		State* state;

		Game(const SleepyDiscord::User gamePlayer) :
			player(gamePlayer) {}
		Game(const Game& game) : player(game.player), state(game.state) {}	//needed when we want to copy a game
		~Game() { if (thread.joinable()) { thread.join(); } }	//notice how we don't copy the threads to copies
		
		void run(std::function<void(State**)> startGame) {
			thread = std::thread(startGame, &state);
		}
	};
	std::list<Game> games;

	enum Weapon {
		ROCK     = 0,
		PAPER    = 1,
		SCISSORS = 2
	};

	enum winState {
		WIN  = 1,
		TIE  = 0,
		LOSE = 2
	};
	/*
	ROCK     - SCISSORS = 0 - 2 = -2 -> (-2 + 3) % 3 = WIN
	PAPER    - SCISSORS = 1 - 2 = -1 -> (-1 + 3) % 3 = LOSE
	ROCK     - PAPER    = 0 - 1 = -1 -> (-1 + 3) % 3 = LOSE
	PAPER    - ROCK     = 1 - 0 =  1 -> ( 1 + 3) % 3 = WIN
	SCISSORS - PAPER    = 2 - 1 =  1 -> ( 1 + 3) % 3 = WIN
	SCISSORS - ROCK     = 2 - 0 =  2 -> ( 2 + 3) % 3 = LOSE
	*/

public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) {
		games.remove_if([](Game game) -> bool {	//remove all games that have state pointing to nullptr
			return game.state == nullptr;
		});

		if (message.startsWith("whcg hello")) {
			games.push_back(Game(message.author));	//create a game put it on the end of the list of games
			games.back().run([=](State** statePointer) {	//run the last game on another thread. Please see the run function above.
				State state = GET_READY;
				*statePointer = &state;	//needed so that we can end the game when the player picks a weapon
				SleepyDiscord::Message countMessage = sendMessage(message.channel_id, "**ROCK!**");
				sleep(1000);  //wait a second
				editMessage(countMessage, "**PAPER!**");
				sleep(1000);
				editMessage(countMessage, "**SCISSORS!**");
				state = SHOOT;
				sleep(1000);
				editMessage(countMessage, "**SHOOT!**");
				sleep(1000);
				if (state != ENDED) {
					state = ENDED;
					sendMessage(message.channel_id, "You lose.\\nWhen I say ``shoot``, you pick ``rock``, ``paper``, or ``scissors``");
				}
				*statePointer = nullptr;  //state should be removed after this line
			});
			return;
		}

		if (games.empty()) return;

		//get the player's choice
		Weapon playerChoice;
		if      (message.content == "rock"     || message.content == ":fist:"       ) playerChoice = ROCK;
		else if (message.content == "paper"    || message.content == ":raised_hand:") playerChoice = PAPER;
		else if (message.content == "scissors" || message.content == ":v:"          ) playerChoice = SCISSORS;
		else return;	//go back if no choice was detected
		games.remove_if([=](Game game) -> bool {	//remove the game from the list when the player wins or loses
			if (game.player == message.author) {
				switch (*game.state) {
				case SHOOT: {
					*game.state = ENDED; //game state is set to end here, so that "you lose" isn't sent after a weapon was picked
					Weapon botChoice = static_cast<Weapon>(rand() % 3);	//random number from 0 to 2
					switch (botChoice) {
					case ROCK:     sendMessage(message.channel_id, ":fist:"); break;
					case PAPER:    sendMessage(message.channel_id, ":raised_hand:"); break;
					case SCISSORS: sendMessage(message.channel_id, ":v:"); break;
					}
					switch ((playerChoice - botChoice + 3) % 3) {
					case LOSE: sendMessage(message.channel_id, ":smiley: I won, and you lost!");  break;
					case TIE:  sendMessage(message.channel_id, ":neutral_face: It's a tie");      break;
					case WIN:  sendMessage(message.channel_id, ":frowning: I lost, and you won"); break;
					}
					return true;	//remove the game from the list
				} break;
				case ENDED: return true; break;
				case GET_READY: sendMessage(message.channel_id, "Not yet, I didn't get to say ``shoot``."); break;
				default: sendMessage(message.channel_id, "Error: unknown state"); break;
				}
			}
			return false;	//keep the game in the list
		});
	}

	~GameClient() {
		for (Game game : games) {
			if (game.thread.joinable()) game.thread.join();
		}
	}
};

int main() {
	srand(time(0));
	GameClient client("Your Token Goes Here", 2);
	client.run();
}

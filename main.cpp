/*
**Evilzone IRC bot
**Factionwars 2013
*/

#include "main.h"
#include "irc.cpp"

int main() {
	EvilIrc irc;
	irc.connect();
	irc.authenticate();
	irc.user();
	irc.join();
	irc.say("--Evil IRC Bot Ready for commands-- (looks cool)");
	irc.say("I like big buts and kulverstukas can not lie.");
	irc.say("And ande can't deny");
	irc.say("bluechill likes c*** uhm chickens");
	irc.say("And i am really in it");
	while(1){
		irc.idle();
	}
}

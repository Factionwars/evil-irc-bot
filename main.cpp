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
	std::string sayString = "hai";
	irc = sayString;
	sayString = "Bye";
	irc = sayString;

	while(1 == 2){
		irc.idle();
	}
	irc.disconnect();
}

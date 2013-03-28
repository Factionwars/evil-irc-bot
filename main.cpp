/********************
**	Evilzone IRC bot
**	Factionwars 2013
*********************/

#include "main.h"
#include "irc.cpp"

int main() {
	Evilirc irc;
	irc.connect();	
	

	irc.authenticate();
	irc.user();
	irc.join();
	std::string strSay = "hai";
	irc = strSay;
	strSay = "Bye";
	irc = strSay;
	std::string strUsername = "Factionwars";
	strSay = "Fuck you owner";
	EvilParser parser;
	int nParseStatus;
	while(1){
		irc.idle();
		nParseStatus = parser.checkMessage(irc.buf);
		switch(nParseStatus){
			case 1:
				strSay = "Your message was ";
				strSay.append(parser.m_strMessage);
				irc = strSay;
				break;
			case 0:
			default:
				break;

		}
		if(nParseStatus == 1){			

		}
	}
}

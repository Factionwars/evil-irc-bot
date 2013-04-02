/********************
**	Evilzone IRC bot
**	Factionwars 2013
*********************/

#include "main.h"
#include "irc.cpp"
#include "commander.cpp"

int main() {
	Evilirc irc;
	irc.connect();		
	irc.authenticate();
	irc.join();
	std::string strSay = "hai";
	irc = strSay;
	std::string strRecv;
	EvilParser parser;
	bool bRunning = true;
	while(bRunning){
		strRecv = irc.idle();
		std::cout << strRecv << std::endl;
		EvilResult* parseResult = parser.checkMessage(strRecv);
		switch(parseResult->resultType){
			case 1:
				strSay = "Your message was ";
				strSay.append(parseResult->strMessage);
				irc = strSay;
				break;
			case 3:
				strSay = "You commanded me to ";
				strSay.append(parseResult->strMessage);
				irc = strSay;
				break;
			case 2:
			case 0:
			default:
				break;

		}
		delete parseResult;
	}
}

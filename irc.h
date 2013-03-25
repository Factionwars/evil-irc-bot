#ifndef _IRC_HEADER
#define _IRC_HEADER
/********************
**	Evilzone IRC bot
**	Factionwars 2013
*********************/

#define PORT "6668"
#define MAXDATASIZE 100
#define HOSTNAME "irc.evilzone.org"
#define NICKNAME "EvilTool"
#define CHANNEL "#test"
#define DEBUG 1

#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//IRC PROTOCOL
//End line
char cEndl[2] = {'\n', '\r'};
//Length of end line
#define LNDL 2

class Evilirc {
private: 
	char buf[MAXDATASIZE];	
	char s[INET6_ADDRSTRLEN];	
	
	int sockfd, numbytes;	
	
	struct addrinfo hints, *servinfo, *p;		
	
	std::string strBuf;
	
	void *get_in_addr(struct sockaddr *sa);
	void recv();
	void pong(char cPong[]);
	void send(std::string strUserMessage);
	
	bool connected;
public:
	Evilirc();
	~Evilirc();
	bool connect();
	void disconnect();
	void authenticate();
	
	void user();
	void join();
	void say(std::string& strMessage);
	void idle();

	std::string& operator= (std::string& strString);

	void kick(std::string& strUsername);
	void kick(std::string& strUsername, std::string& strReason);

	
};


#endif

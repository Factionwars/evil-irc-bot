/*
**Evilzone IRC bot
**Factionwars 2013
*/

#include "irc.h"

class EvilIrc {
private: 
	int sockfd, numbytes, rv;
	
	struct addrinfo hints, *servinfo, *p;
	
	char s[INET6_ADDRSTRLEN];
	char buf[MAXDATASIZE];
	
	std::string strBuf;
	
	void *get_in_addr(struct sockaddr *sa)
	{
		if (sa->sa_family == AF_INET) {
		    return &(((struct sockaddr_in*)sa)->sin_addr);
		}

		return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}
	
	void recv () {
		memset(&buf, 0, 100);
		if ((numbytes = ::recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			std::cout << "RECV ERROR";			
		}
		std::string strBuf = buf;
		if(strBuf.substr(0, 4) == "PING") {
			pong(buf);
		}
	
		buf[numbytes] = '\0';
		std::cout << buf << std::endl;
	}
	
	void pong(char cPong[]) {
		const char PONG[] = "PONG";
		for(int i = 0; i < 4; i++){
			cPong[i] = PONG[i];
		}		
		
		if(::send(sockfd, cPong, strlen(cPong), 0) == -1){
			std::cout << "send Error";
		}
		std::cout << cPong << std::endl;
		return;
	}
	
	void send(char cUserMessage[]) {
		//recv();
		int length = strlen(cUserMessage) + LNDL;	
		char cIrcMessage[length];
		strncpy(cIrcMessage, cUserMessage, length - LNDL);	
		cIrcMessage[length - 2] = '\r';
		cIrcMessage[length - 1] = '\n';

		std::cout << "Sending: " << cIrcMessage;
		if(::send(sockfd, cIrcMessage, length, 0) == -1){
			std::cout << "send Error";
		}		
	}
	
public:
	
	EvilIrc() {
		memset(&hints, 0, sizeof(hints));
		hints.ai_family 	= AF_UNSPEC;
		hints.ai_socktype 	= SOCK_STREAM;
	}
	
	~EvilIrc() {
		disconnect();
	}
	
	bool connect() {		
		if((rv = getaddrinfo(HOSTNAME, PORT, &hints, &servinfo)) != 0) {
			return -1;
		}
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
				std::cout << "socket error";
				continue;
			}
			if(::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				std::cout << "error connecting";
				continue;
			}
			break;
		}
		
		if(p == NULL) {
			std::cout << "failed to connect";
			return 2;
		}
		
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
		
		freeaddrinfo(servinfo);	
						
		return 0;		
	}
	
	bool disconnect() {
		close(sockfd);
	}

	void authenticate() {
		char cNickMessage[] = "NICK "  NICKNAME; 
		send(cNickMessage);
		recv();
		//std::cout << buf;			
	}
	void user() {
	
		char cUserMessage[] = "USER "NICKNAME" 0 *: eviltools";
		send(cUserMessage);
		recv();
		//std::cout << buf;
	}
	void join() {
	
		char cJoinMessage[] = "JOIN "CHANNEL;
		send(cJoinMessage);
		//std::cout << buf;
	}
	void say() {
		char cSayMessage[] = "privmsg "CHANNEL" :";
		send(cSayMessage);	
	}
	void idle() {
		std::cout << "idleLoop" << std::endl;
		recv();
		std::cout << "doAction";
	}
	
};


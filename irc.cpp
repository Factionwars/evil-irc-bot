/*
**Evilzone IRC bot
**Factionwars 2013
*/

#include "irc.h"

EvilIrc::EvilIrc() {
	memset(&hints, 0, sizeof(hints));
	hints.ai_family 	= AF_UNSPEC;
	hints.ai_socktype 	= SOCK_STREAM;
}

EvilIrc::~EvilIrc() {
	disconnect();
}


void *EvilIrc::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
	    return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void EvilIrc::recv () {
	memset(&buf, 0, 100);
	
	if ((numbytes = ::recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
#if(DEBUG)
		std::cout << "RECV ERROR";
#endif
		
	}
	std::string strBuf = buf;
	if(strBuf.substr(0, 4) == "PING") {
		pong(buf);
	}

	buf[numbytes] = '\0';
#if(DEBUG)
	std::cout << buf << std::endl;
#endif
}

void EvilIrc::pong(char cPong[]) {
	const char PONG[] = "PONG";
	for(int i = 0; i < 4; i++){
		cPong[i] = PONG[i];
	}		
	
	if(::send(sockfd, cPong, strlen(cPong), 0) == -1){
		std::cout << "send Error";
	}
#if(DEBUG)
	std::cout << cPong << std::endl;
#endif
	return;
}

void EvilIrc::send(std::string strUserMessage) {
	int length = strlen(strUserMessage.c_str()) + 2;	
   	char *cUserMessage = new char[length];
    strncpy(cUserMessage, (char*)strUserMessage.c_str(), ( length - 1 ) );
    
	cUserMessage[length - 2] = '\r';
	cUserMessage[length - 1] = '\n';
#if(DEBUG)
	std::cout << "Sending: " << cUserMessage << std::endl;
#endif
		
	if(::send(sockfd, cUserMessage, length, 0) == -1){	
#if(DEBUG)
		std::cout << "send Error";
		exit(1);
#endif
	}
	delete[] cUserMessage;	
}
	


	
bool EvilIrc::connect() {
	int rv;		
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

bool EvilIrc::disconnect() {
	close(sockfd);
}

void EvilIrc::authenticate() {
	std::string strNickMessage = "NICK "  NICKNAME""; 
	send(strNickMessage);
	recv();
	//std::cout << buf;			
}
void EvilIrc::user() {

	std::string strUserMessage = "USER "NICKNAME" 0 *: eviltools";
	send(strUserMessage);
	recv();
	//std::cout << buf;
}
void EvilIrc::join() {

	std::string strJoinMessage = "JOIN "CHANNEL"";
	send(strJoinMessage);
	//std::cout << buf;
}
void EvilIrc::say(std::string strMessage) {
	std::string strSayMessage = "privmsg "CHANNEL" :";
	strSayMessage.append(strMessage);
	send(strSayMessage);	
}
void EvilIrc::idle() {
	std::cout << "idleLoop" << std::endl;
	recv();
	std::cout << "doAction";
}
	


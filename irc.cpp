/********************
**	Evilzone IRC bot
**	Factionwars 2013
*********************/

#include "irc.h"

Evilirc::Evilirc() : connected(false) 
{
	memset(&hints, 0, sizeof(hints));
	hints.ai_family 	= AF_UNSPEC;
	hints.ai_socktype 	= SOCK_STREAM;
}

Evilirc::~Evilirc() 
{
	disconnect();
}


void *Evilirc::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
	    return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Evilirc::recv () 
{
	memset(&buf, 0, 100);
	
	if ((numbytes = ::recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
	{
#if(DEBUG)
		std::cout << "RECV ERROR";
#endif
		
	}
	std::string strBuf = buf;
	if(strBuf.substr(0, 4) == "PING") 
	{
		pong(buf);
	}
}

void Evilirc::pong(char cPong[]) 
{
	const char PONG[] = "PONG";
	for(int i = 0; i < 4; i++)
	{
		cPong[i] = PONG[i];
	}		
	
	if(::send(sockfd, cPong, strlen(cPong), 0) == -1)
	{
		std::cout << "send Error";
	}
#if(DEBUG)
	std::cout << cPong << std::endl;
#endif
	return;
}

void Evilirc::send(std::string strUserMessage) 
{
	int length = strlen(strUserMessage.c_str()) + 2;	
   	char *cUserMessage = new char[length];
    strncpy(cUserMessage, (char*)strUserMessage.c_str(), ( length - 1 ) );
    
	cUserMessage[length - 2] = '\r';
	cUserMessage[length - 1] = '\n';
#if(DEBUG)
	std::cout << "Sending: " << cUserMessage << std::endl;
#endif
		
	if(::send(sockfd, cUserMessage, length, 0) == -1)
	{	
#if(DEBUG)
		std::cout << "send Error";
		exit(1);
#endif
	}
	delete[] cUserMessage;	
}
	


	
bool Evilirc::connect() 
{
	if(connected)
	{
		return 0;
	}
	int rv;		
	if((rv = getaddrinfo(HOSTNAME, PORT, &hints, &servinfo)) != 0) 
	{		
		return -1;
	}
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cout << "socket error";
			continue;
		}
		if(::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			std::cout << "error connecting";
			continue;
		}
		break;
	}
	
	if(p == NULL) 
	{
		std::cout << "failed to connect";
		return 2;
	}
	
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	
	freeaddrinfo(servinfo);	
	connected = true;	
	return 0;		
}
/********************
/*
/*Disconnect the IRC server and socket
/*
/********************/
void Evilirc::disconnect() 
{
	if(!connected)
	{
		return;
	}
	std::string strQuit = "quit";
	send(strQuit);
	close(sockfd);
	connected = false;
}
/********************
/*
/*Authenticate with the server
/*
/********************/
void Evilirc::authenticate()
{
	std::string strNickMessage = "NICK "  NICKNAME""; 
	send(strNickMessage);
	recv();			
	std::string strUserMessage = "USER "NICKNAME" 0 *: eviltools";
	send(strUserMessage);
	recv();
}
/********************
/*
/*Join the configured channel
/*
/********************/
void Evilirc::join() 
{
	std::string strJoinMessage = "JOIN "CHANNEL"";
	send(strJoinMessage);
}
/********************
/*
/*Say something in the configured channel
/*
/********************/
void Evilirc::say(std::string& strMessage) 
{
	std::string strSayMessage = "privmsg "CHANNEL" :";
	strSayMessage.append(strMessage);
	send(strSayMessage);	
}
/********************
/*
/*Idle, look for new messages.
/*
/********************/
std::string Evilirc::idle() 
{
	recv();	
	std::string raw = buf;
	int nPos = 0;
	while( raw.find(std::string("\x0d\x0a"), 0) == std::string::npos )
	{
		recv();
		raw.append(buf);
	}
	return raw;
	
}
/********************
/*
/*Override the assignment operator when given a std::string
/*When assigned it sends it as a private message to the configured channel
/*
/********************/
std::string& Evilirc::operator= (std::string& strString) 
{
	say(strString);
}
/********************
/*
/*Kick a user with or without reason
/*
/********************/
void Evilirc::kick(std::string& strUsername) 
{
	std::string strKickMessage = "KICK "CHANNEL" ";
	strKickMessage.append(strUsername);
	send(strKickMessage);
}

void Evilirc::kick(std::string& strUsername, std::string& strReason) 
{
	std::string strKickMessage = "KICK "CHANNEL" ";
	strKickMessage.append(strUsername);
	strKickMessage.append(" :");
	strKickMessage.append(strReason);
	send(strKickMessage);
}
/********************
/*
/* Parse a IRC message
/* 0: Not a valid IRC message or supported
/* 1: Normal message
/* 2: Mode change
/* 3: ! Command
/*
/********************/
EvilResult* EvilParser::checkMessage(std::string raw)
{
	EvilResult *result = new EvilResult;
	if(raw.substr(0, 1) != ":")
	{
		result->resultType = 0;
		return result;
	}
	int nPos;
	int nNewPos;

	nPos = raw.find("!");
	if(nPos == std::string::npos)
	{
		result->resultType = 0;
		return result;
	}
	result->strNickname = raw.substr(1, nPos - 1);

	//Extract the message type (MODE|PRIVMSG)

	nPos = raw.find(" ", nPos);
	if(nPos == std::string::npos)
	{
		result->resultType = 0;
		return result;
	}
	nPos++;

	nNewPos = raw.find(" ", nPos);
	if(nNewPos == std::string::npos)
	{
		result->resultType = 0;
		return result;
	}
	std::string strType = raw.substr(nPos, (nNewPos - nPos) );

	if(strType == "MODE")
	{
		result->resultType = 2;
		return result;
	}
	
	if(strType != "PRIVMSG")
	{
		result->resultType = 0;
		return result;
	}
	//Extract the Channel/user name segment
	nPos = nNewPos;

	nPos = raw.find(" ", nPos);
	if(nPos == std::string::npos)
	{
		result->resultType = 0;
		return result;
	}
	nPos++;

	nNewPos = raw.find(" ", nPos);
	if(nNewPos == std::string::npos)
	{
		result->resultType = 0;
		return result;
	}
	result->strChannel = raw.substr(nPos, (nNewPos - nPos) );
	
	//Extract the message
	nPos = nNewPos;

	nPos = raw.find(":", nPos);
	if(nPos == std::string::npos)
	{
		result->resultType = 0;
		return result;
	}
	nPos++;
	if(raw.substr(nPos, 1) != "!")
	{
		result->strMessage = raw.substr(nPos);
		result->resultType = 1;
		return result;
	}
	nPos++;
	

	nNewPos = raw.find(" ", nPos);
	if(nNewPos == std::string::npos)
	{
		result->strMessage = raw.substr(nPos);
		result->resultType = 3;
		return result;
	}
	result->strMessage = raw.substr(nPos, (nNewPos - nPos));
	//Extract the arguments if exists
	//And assign a basic 4 arguments long strArray
	//If there are more arguments we will extend it.
	int nIndex = 0;
	int nArgvLength = 4;
	result->nArgc = 0;
	result->strArgv = new std::string[nArgvLength];
	std::string *strTemp;

	while ( (nNewPos = raw.find(" ", nPos)) != std::string::npos ) 
	{
		if(nIndex == 20)
		{
			break;
		}
		if(nIndex > (nArgvLength - 1))
		{	
			//resize the array
			strTemp = new std::string[nArgvLength + 2];
			std::copy(result->strArgv, result->strArgv + nArgvLength, strTemp);
			nArgvLength += 2;
			result->strArgv = strTemp;
			delete[] strTemp;
			strTemp = 0;
		}
		result->strArgv[nIndex] = raw.substr(nPos, (nNewPos - nPos));
				
		nNewPos++;
		result->nArgc++;
		nIndex++;
		nPos = nNewPos;
		
 	}
	result->resultType = 3;
	return result;
}
	


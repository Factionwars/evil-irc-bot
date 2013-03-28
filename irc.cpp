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

	buf[numbytes] = '\0';
#if(DEBUG)
	std::cout << buf << std::endl;
#endif
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
}
/********************
/*
/*Tell the server who i am
/*
/********************/
void Evilirc::user() 
{

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
void Evilirc::idle() 
{
	std::cout << "idleLoop" << std::endl;
	recv();	
	std::cout << "doAction";
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
int EvilParser::checkMessage(std::string raw)
{
	if(raw.substr(0, 1) != ":")
	{
		return 0;
	}
	int nPos;
	int nNewPos;

	nPos = raw.find("!");
	if(nPos == std::string::npos)
	{
		return 0;
	}
	std::string strNickname = raw.substr(1, nPos - 1);

	m_strNickname = strNickname;

	//Extract the message type (MODE|PRIVMSG)

	nPos = raw.find(" ", nPos);
	if(nPos == std::string::npos)
	{
		return 0;
	}
	nPos++;

	nNewPos = raw.find(" ", nPos);
	if(nNewPos == std::string::npos)
	{
		return 0;
	}
	std::string strType = raw.substr(nPos, (nNewPos - nPos) );

	if(strType == "MODE")
	{
		return 2;
	}
	
	if(strType != "PRIVMSG")
	{
		return 0;
	}
	//Extract the Channel/user name segment
	nPos = nNewPos;

	nPos = raw.find(" ", nPos);
	if(nPos == std::string::npos)
	{
		return 0;
	}
	nPos++;

	nNewPos = raw.find(" ", nPos);
	if(nNewPos == std::string::npos)
	{
		return 0;
	}
	m_strChannel = raw.substr(nPos, (nNewPos - nPos) );
	
	//Extract the message
	nPos = nNewPos;

	nPos = raw.find(":", nPos);
	if(nPos == std::string::npos)
	{
		return 0;
	}
	nPos++;
	if(raw.substr(nPos, 1) != "!")
	{
		m_strMessage = raw.substr(nPos);
		return 1;
	}
	
	return 1;
}
	


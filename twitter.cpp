
/*-----------------------------------------------------------------------------------

accounts Skylar, Cassie, and Hope retweet
	shutupmikeginn 
	DanMentos
	jonnysun
	BuckyIsotope
	Fred_delicious 
	pixelatedboat //NEXT

human sheep accounts
	fuckeveryword
	teamfollowback
	
-----------------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------------
//									INCLUDES
//-----------------------------------------------------------------------------------

#include <iostream>
#include <getopt.h>

#include "Twitter.h"
#include "TwitterFollowBot.h"
#include "../twitter_credentials.h"

//-----------------------------------------------------------------------------------
//									DECLARATIONS
//-----------------------------------------------------------------------------------

static struct option longopts[] = {
	{"tweet", required_argument, nullptr, 't'},
	{"usage", no_argument, nullptr, 'u'},
	{"loop", required_argument, nullptr, 'l'},
	{"record", no_argument, nullptr, 'r'},
	{"clean", no_argument, nullptr, 'c'},
	{nullptr, 0, nullptr, 0}
};

void setup(int argc, char *argv[], Twitter & t, TwitterFollowBot & bot);

//-----------------------------------------------------------------------------------
//										MAIN
//-----------------------------------------------------------------------------------

int main(int argc, char *argv[]){

	std::ios_base::sync_with_stdio(false);

	Twitter t = Twitter(CONSUMER_KEY, 
						CONSUMER_SECRET, 
						ACCESS_TOKEN, 
						ACCESS_TOKEN_SECRET);
						
	TwitterFollowBot bot = TwitterFollowBot(t);

	setup(argc, argv, t, bot);

	return 0;
}

//-----------------------------------------------------------------------------------
//									DEFENITIONS
//-----------------------------------------------------------------------------------

void setup(int argc, char *argv[], Twitter & t, TwitterFollowBot & bot){
	std::string str;
	int idx = 0;
	char c;
	while ((c = getopt_long(argc, argv, "t:ul:rc", longopts, &idx)) != -1){
		switch(c) {
			case 't': 
				str = (std::string)optarg;
				t.tweet(str); return; break;
			case 'u':
				t.usage(); return; break;
			case 'l':
				str = (std::string)optarg;
				bot.loop(str); return; break;
			case 'r': 
				bot.record(); return; break;
			case 'c':
				bot.clean(); return; break;
		}
	} 
}


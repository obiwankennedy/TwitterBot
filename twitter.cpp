
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

-----------------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------------
//									INCLUDES
//-----------------------------------------------------------------------------------

#include <iostream>
#include <getopt.h>

#include "TwitterFollowBot.h"
#include "../twitter_credentials.h"

//-----------------------------------------------------------------------------------
//									DECLARATIONS
//-----------------------------------------------------------------------------------

static struct option longopts[] = {
	{"tweet", required_argument, nullptr, 't'},
	{"rate", no_argument, nullptr, 'r'},
	{"loop", required_argument, nullptr, 'l'}, //make loop take an argument
	{nullptr, 0, nullptr, 0}
};

void setup(int argc, char *argv[], TwitterFollowBot & t);

//-----------------------------------------------------------------------------------
//										MAIN
//-----------------------------------------------------------------------------------

int main(int argc, char *argv[]){

	std::ios_base::sync_with_stdio(false);

	TwitterFollowBot t = TwitterFollowBot(CONSUMER_KEY, 
										  CONSUMER_SECRET, 
										  ACCESS_TOKEN, 
										  ACCESS_TOKEN_SECRET);

	setup(argc,argv, t);

	return 0;
}

//-----------------------------------------------------------------------------------
//									DEFENITIONS
//-----------------------------------------------------------------------------------

void setup(int argc, char *argv[], TwitterFollowBot & t){
	std::string str;
	int idx = 0;
	char c;
	while ((c = getopt_long(argc, argv, "t:rl:", longopts, &idx)) != -1){
		switch(c) {
			case 't': 
				str = (std::string)optarg;
				t.tweet(str);
				return;
				break;
			case 'r':
				t.rate();
				return;
				break;
			case 'l':
				str = (std::string)optarg;
				t.loop(str); 
				return;
				break;
		}
	} 
}


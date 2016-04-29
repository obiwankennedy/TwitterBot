
#include <iostream>
#include "TwitterFollowBot.h"

#include "../twitter_credentials.h"

int main(){

	TwitterFollowBot t = TwitterFollowBot(CONSUMER_KEY, 
										  CONSUMER_SECRET, 
										  ACCESS_TOKEN, 
										  ACCESS_TOKEN_SECRET);
	
	t.loop();
	


	return 0;
}

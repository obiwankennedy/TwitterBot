
main : twitter.cpp Twitter.h TwitterFollowBot.h
	g++ -std=c++11 -Wall -Werror -O2 twitter.cpp Twitter.h TwitterFollowBot.h json.h -ltwitcurl -o t

clean :
	rm -vf t *~

git :
	git add *
	git commit -m "$(note)"
	git push


main : twitter.cpp TwitterFollowBot.h
	g++ -std=c++11 -Wall -Werror -O2 twitter.cpp TwitterFollowBot.h json.h -ltwitcurl -o t

clean :
	rm -vf t *~

git :
	rm *~
	git add *
	git commit -m "$(note)"
	git push

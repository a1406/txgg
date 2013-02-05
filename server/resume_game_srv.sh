#/bin/sh
killall -9 game_srv
cd /home/jacktang/svnroot/txgg/server/game_srv/
./game_srv -d 111 >/dev/null 2>/dev/null

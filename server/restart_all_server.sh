#/bin/sh
killall -9 game_srv
killall -9 conn_srv
killall -9 db_srv
ipcrm -M 0x10001020
cd /home/jacktang/svnroot/txgg/server/conn_srv/
./conn_srv -d >/dev/null 2>/dev/null
cd /home/jacktang/svnroot/txgg/server/db_srv/
./db_srv -d >/dev/null 2>/dev/null
sleep 3
cd /home/jacktang/svnroot/txgg/server/game_srv/
./game_srv -d >/dev/null 2>/dev/null

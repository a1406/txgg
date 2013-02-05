#!/bin/sh
conn_srv/conn_srv >/dev/null 2>/dev/null &
db_srv/db_srv >/dev/null 2>/dev/null &
game_srv/game_srv >/dev/null 2>/dev/null &
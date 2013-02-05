all: db_bag.proto
	../bin/protoc-c  --c_out=. db_role_info.proto
	gcc -c -O0 -Wall -Werror -DDEBUG db_role_info.pb-c.c -g -O0 -Wall -Werror -DDEBUG -I../other_include/

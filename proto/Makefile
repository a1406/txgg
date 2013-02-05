all: cs.proto
	../server/bin/protoc-c  --c_out=. cs.proto
	../server/bin/protoc-c  --c_out=. err.proto
	gcc -c -O0 -Wall -Werror -DDEBUG cs.pb-c.c -g -O0 -Wall -Werror -DDEBUG -I../../protobuf-c/src/ -I../server/other_include/
#	gcc -c -O0 -Wall -Werror -DDEBUG test.c -g -O0 -Wall -Werror -DDEBUG -I../../protobuf-c/src/
#	gcc -o test test.o cs.pb-c.o -lprotobuf-c -L../../protobuf-c/src/.libs



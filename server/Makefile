all:
	make -C ../proto
	make -C common_include
	make -C tools/xls proto
	make -C tools/xls
	make -C tools/config proto
	make -C tools/config
	make -C conn_srv
	make -C game_srv
	make -C db_srv

clean:
	make -C tools/config clean
	make -C tools/xls clean
	make -C conn_srv clean
	make -C game_srv clean
	make -C db_srv clean


#/bin/sh
find ./ -type f -name "*.[hc]" > cscope.files
find ./ -type f -name "*.proto" >> cscope.files
find ../proto -type f -name "*.[hc]" >> cscope.files
find ../proto -type f -name "*.proto" >> cscope.files
cscope -bq
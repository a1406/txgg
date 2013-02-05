#/bin/sh
cd ../../../excel_doc/
svn up
cd ../server/tools/config/
rm *.output
./converter ../../../excel_doc/data.xls
../xls/SCENE ../../../excel_doc/scene.xls
cd ../../../config/
svn up
cp ../server/tools/config/*.output .
cp ../server/tools/xls/scene.output .
svn commit -m "update config"

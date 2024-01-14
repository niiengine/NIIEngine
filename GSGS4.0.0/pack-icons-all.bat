@ECHO OFF

cd gsgs
echo "Packing gsgs icons..."
call pack-icons.bat

cd ..\gsgs-fresh-farm
echo "Packing gsgs-fresh-farm..."
call pack-icons.bat

cd ..\gsgs-dark
echo "Packing gsgs-dark..."
call pack-icons.bat

cd ..
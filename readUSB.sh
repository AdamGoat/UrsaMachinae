cd `findmnt -o TARGET -p -f -n |head -1`
cp ./m* ~pi/Documents/UrsaMachinae/cords.json
echo "File copied"

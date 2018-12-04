#!/bin/bash

raspistill -n -o temp.jpg -t 10
raspistill -n -o temp2.jpg -t 10
echo "Clear!"
convert temp.jpg temp.tif
convert temp2.jpg temp2.tif
tesseract temp.tif char --psm 10
tesseract temp2.tif char2 --psm 10
#rm temp.jpg
#rm temp.tif
cat char.txt | head -1
cat char2.txt | head -1

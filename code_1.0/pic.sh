#!/bin/bash

raspistill -roi .25,.2,.5,.5 -n -o temp1.tif -t 10 -rot 0   -ifx denoise
#raspistill -roi .375,.375,.25,.25 -n -o temp2.tif -t 10 -rot 90  -ifx denoise
#raspistill -roi .375,.375,.25,.25 -n -o temp3.tif -t 10 -rot 180 -ifx denoise
#raspistill -roi .375,.375,.25,.25 -n -o temp4.tif -t 10 -rot 270 -ifx denoise
echo "Clear!"
mpack -s "Pi Image" ./temp1.tif goad.adam@gmail.com
#convert temp1.tif -rotate 90  temp2.tif 
#convert temp1.tif -rotate 180 temp3.tif
#convert temp1.tif -rotate 170 temp4.tif
tesseract temp1.tif char1 --psm 10
#tesseract temp2.tif char2 --psm 10
#tesseract temp3.tif char3 --psm 10
#tesseract temp4.tif char4 --psm 10
#rm temp.jpg
#rm temp.tif
cat char1.txt | head -1
#cat char2.txt | head -1
#cat char3.txt | head -1
#cat char4.txt | head -1

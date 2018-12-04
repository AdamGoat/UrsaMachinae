#!/usr/bin/python
# -*- coding: utf-8 -*-
import pytesseract
import requests
from PIL import Image
from PIL import ImageFilter
from StringIO import StringIO

def process_image(url):
    image = _get_image(url)
    image = image.filter(ImageFilter.SHARPEN)
#    image = image.convert('1')
    print image_to_string(image)

def _get_image(url):
    return Image.open(StringIO(requests.get(url).content))

process_image('https://upload.wikimedia.org/wikipedia/commons/3/3f/Polish_passport_biodata_page.png')

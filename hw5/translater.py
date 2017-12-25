#!/usr/bin/env python
import sys

from PIL import Image


if len(sys.argv) != 2:
    print("Usage: python translater.py <filename.jpg>")
    sys.exit()

input_file = sys.argv[1]
im = Image.open(input_file)

output_file = 'input'
f_out = open(output_file, 'w')

f_out.write('{}\n'.format(im.height * im.width*3))

for x in range(im.width):
    for y in range(im.height):
        f_out.write('{} {} {}\n'.format(*im.getpixel((x, y))))

im.close()
f_out.close()

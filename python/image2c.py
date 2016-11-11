import os
import gzip
import codecs

file = codecs.open("crossword.png", "rb")

old_file_position = file.tell()
file.seek(0, os.SEEK_END)
chartjs_length = file.tell()
file.seek(old_file_position, os.SEEK_SET)

file_content = file.read()
file.close()

file = codecs.open("BackgroundImage.cpp", "w", "utf-8")

file.write("#include \"Resources.h\"\r\n")
file.write("\r\n")
file.write("uint32_t Resources::background_image_length=")
file.write(str(chartjs_length)+";\r\n")
file.write("\r\n")
file.write("uint8_t Resources::background_image[]=\r\n")
file.write("{")
file.write("\r\n")

k=1
l=1
for x in file_content:
    char = "0x" + format(ord(x), 'x')
    file.write(char)
    if l<len(file_content):
        file.write(",")
    if k==25:
        file.write("\r\n")
        k=0
    k+=1
    l+=1

file.write("\r\n")
file.write("};")

file.close()
 

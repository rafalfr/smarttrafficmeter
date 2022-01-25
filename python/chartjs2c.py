import os
import gzip
import codecs

#python 2

# https://cdnjs.com/libraries/Chart.js

# open the original file
file = codecs.open("chart.min.js", "rb", "utf-8")
file_content = file.read()
file.close()

# save the gzip compressed file 
with gzip.open('chart.js.gz', 'wb', 9) as f:
    f.write(file_content)

# open the gzip compressed file
file = codecs.open("chart.js.gz", "rb")

old_file_position = file.tell()
file.seek(0, os.SEEK_END)
chartjs_length = file.tell()
file.seek(old_file_position, os.SEEK_SET)

file_content = file.read()
file.close()

file = codecs.open("chartjs.cpp", "w", "utf-8")

file.write("#include \"Resources.h\"\r\n")
file.write("\r\n")
file.write("uint32_t Resources::chart_js_length=")
file.write(str(chartjs_length)+";\r\n")
file.write("\r\n")
file.write("uint8_t Resources::chart_js[]=\r\n")
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

#file.write("0x00") #for gzip compressed file we skip this byte
file.write("\r\n")
file.write("};")

file.close()

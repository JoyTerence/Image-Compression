import numpy
import math
import Image
import cv2
from matplotlib import pyplot as plt

img = cv2.imread('C:\Users\joyte\Desktop\DrowRanger.jpg')

blue,g,r = cv2.split(img)

#a = numpy.matrix('52 55 61  66 70 61 64 73;63 59 55 90 109 85 69 72;62 59 68 113 144 104 66 73;63 58 71 122 154 106 70 69;67 61 68 104 126 88 68 70;79 65 60 70 77 68 58 75;85 71 64 59 55 61 65 83;87 79 69 68 65 76 78 94')

a= numpy.empty([8,8],float,0)
for i in range(0,8):
    for j in range(0,8):
        a[i,j] = blue[i,j]
        
print a
b = a.copy()

print "B  : " , type(b)
b = b - 128
print b

k = 1/4.0

G = numpy.empty([8,8],float,0)
Q = numpy.matrix('16 11 10 16 24 40 51 61;12 12 14 19 26 58 60 55;14 13 16 24 40 57 69 56;14 17 22 29 51 87 80 62;18 22 37 56 68 109 103 77;24 35 55 64  81 104 113 92;49 64 78 87 103 121 120 101;72 92 95 98 112 100 103 99')


sum_ =0
mul_fact = math.pi/16

for i in range(0,8):
    for j in range(0,8):
        alpha_x = 1
        alpha_y = 1
        if i==0:
            alpha_x = 1/math.sqrt(2)
        if j==0:
            alpha_y = 1/math.sqrt(2)
        for x in range(0,8):
            for y in range(0,8):
                cosx = math.cos( (2*x+1)*i*mul_fact)
                cosy = math.cos( (2*y+1)*j*mul_fact)
                sum_ = sum_ + (b[x,y]*cosx*cosy)
        G[i,j] = k *alpha_x*alpha_y*sum_
        #print "::",k," ",sum_," ",alpha_x," ",alpha_y
        sum_ = 0

print G
print
print Q
print


RESULT = numpy.around(G/Q,0)

line = list(str(RESULT))
print line
file = open('F:\Python\Compress.txt','w')
line[:] = (value for value in line if value != '[')
line[:] = (value for value in line if value != ']')
line[:] = (value for value in line if value != '.')
line[:] = (value for value in line if value != '\n')
line[:] = (value for value in line if value != "'")
string = ' '.join(line)
final_string = string.replace('       ',' ')
print final_string
file.write(final_string)
file.close()


import numpy
import math
import Image
import cv2
from matplotlib import pyplot as plt

img = cv2.imread('C:\Users\joyte\Desktop\DrowRanger.jpg')
file = open('Compress.txt','a')

blue,geen,red = cv2.split(img)

a= numpy.empty([8,8],float,0)

pixel_number = blue.size
row,column,channels = img.shape

row_iter = 0
col_iter = 0

print "ROW : " ,row," COL : ",column

while row_iter < row-row%8:
    col_iter = 0
    while col_iter < column-column % 8:
        print "RowIter : ",row_iter," ColIter : ",col_iter
        for i in range(row_iter,row_iter+8):
            for j in range(col_iter,col_iter+8):
                #Copy 8*8 matrix into matrix a
                a[i-row_iter,j-col_iter] = blue[i,j]
                
        b = a.copy()
        #Centering Values around Zero
        b = b - 128

        k = 1/4.0

        G = numpy.empty([8,8],float,0)
        #Quantization Matrix
        Q = numpy.matrix('16 11 10 16 24 40 51 61;12 12 14 19 26 58 60 55;14 13 16 24 40 57 69 56;14 17 22 29 51 87 80 62;18 22 37 56 68 109 103 77;24 35 55 64  81 104 113 92;49 64 78 87 103 121 120 101;72 92 95 98 112 100 103 99')


        sum_ =0
        mul_fact = math.pi/16
        
        #DCT
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
                sum_ = 0
        
        #Divide by Quantization Matrix
        FLOAT_RESULT = numpy.around(G/Q,0)
        RESULT = FLOAT_RESULT.astype(int)
        numpy.savetxt(file,RESULT,fmt='%i')
        col_iter += 8
    row_iter += 8
file.close()

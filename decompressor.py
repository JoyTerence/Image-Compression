import numpy
import math


with open('F:\Python\Compress.txt') as f:
    for line in f:
        int_list = [float(i) for i in line.split()]
        print int_list.stri

'''
#a = numpy.matrix('-26.  -3.  -6.   2.   2.  -1.   0.   0.;0. -2.  -4.   1.   1.   0.   0.   0.; -3.   1.   5.  -1.  -1.   0.   0.   0.;-3.   1.   2.  -1.   0.   0.   0.   0.; 1.   0.   0.   0.   0.   0.   0.   0.; 0.   0.   0.   0.   0.   0.   0.   0.; 0.   0.   0.   0.   0.   0.   0.   0.;0.   0.   0.   0.   0.   0.   0.   0.')
a = numpy.matrix(' -46.   7.   9.   5.   3.   1.   1.   0.;0.   0.   0.   0.   0.   0.   0.   0.; 0.   0.   0.   0.   0.   0.   0.   0.; 0.   0.   0.   0.   0.   0.   0.   0.;0.   0.   0.   0.   0.   0.   0.   0.;0.   0.   0.   0.   0.   0.   0.   0.; 0.   0.   0.   0.   0.   0.   0.   0.; 0.   0.   0.   0.   0.   0.   0.   0.')
f = numpy.empty([8,8],float,0)
Q = numpy.matrix('16 11 10 16 24 40 51 61;12 12 14 19 26 58 60 55;14 13 16 24 40 57 69 56;14 17 22 29 51 87 80 62;18 22 37 56 68 109 103 77;24 35 55 64  81 104 113 92;49 64 78 87 103 121 120 101;72 92 95 98 112 100 103 99')

INPUT =  numpy.multiply(a,Q)

mul_fact = math.pi/16
k=1/4.0
_sum_ = 0

for x in range(0,8):
    for y in range(0,8):
        for u in range(0,8):
            for v in range(0,8):
                alpha_u = 1
                alpha_v = 1
                if u == 0:
                    alpha_u = 1/math.sqrt(2)
                if v == 0:
                    alpha_v = 1/math.sqrt(2)
                cos_u = math.cos( (2*x+1)*u*mul_fact)
                cos_v = math.cos( (2*y+1)*v*mul_fact)
                _sum_  = _sum_ + alpha_u * alpha_v * INPUT[u,v] * cos_u * cos_v
        f[x,y] = k * _sum_
        _sum_ = 0

print numpy.around(f) + 128
'''

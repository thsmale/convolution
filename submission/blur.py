import numpy as np
import cv2 as cv
from matplotlib import pyplot as plt

fig, axs = plt.subplots(2, 3)

img = cv.imread('images/noisy.jpeg')

weight = 5.5
blur = cv.GaussianBlur(img, (5,5), sigmaX=weight, sigmaY=weight)

myblur = cv.imread('output/noisy.jpeg')

axs[0, 0].imshow(img)
axs[0, 0].set_title("Original")
axs[0, 0].set_xticks([])
axs[0, 0].set_yticks([])

axs[0, 1].imshow(blur)
axs[0, 1].set_title("Python Gaussian")
axs[0, 1].set_xticks([])
axs[0, 1].set_yticks([])

axs[0, 2].imshow(myblur)
axs[0, 2].set_title("My Gauss")
axs[0, 2].set_xticks([])
axs[0, 2].set_yticks([])

img = cv.imread('images/noisy.jpeg')

weight = 5.5
blur = cv.GaussianBlur(img, (5,5), sigmaX=weight, sigmaY=weight)

myblur = cv.imread('my_noisy.jpeg')

axs[1, 0].imshow(img)
axs[1, 0].set_xticks([])
axs[1, 0].set_yticks([])

axs[1, 1].imshow(blur)
axs[1, 1].set_xticks([])
axs[1, 1].set_yticks([])

axs[1, 2].imshow(myblur)
axs[1, 2].set_xticks([])
axs[1, 2].set_yticks([])

plt.show()
#plt.subplot(131), plt.imshow(img), plt.title('Original')
#plt.xticks([]), plt.yticks([])
#plt.subplot(132), plt.imshow(blur), plt.title('Python Gaussian')
#plt.xticks([]), plt.yticks([])
#plt.subplot(133), plt.imshow(myblur), plt.title('My Gauss')
#plt.xticks([]), plt.yticks([])

#img = cv.imread('images/noisy.jpeg')
#
#weight = 5.5
#blur = cv.GaussianBlur(img, (5,5), sigmaX=weight, sigmaY=weight)
#
#myblur = cv.imread('my_noisy.jpeg')
#
#plt.subplot(234), plt.imshow(img), plt.title('Original')
#plt.xticks([]), plt.yticks([])
#plt.subplot(235), plt.imshow(blur), plt.title('Python Gaussian')
#plt.xticks([]), plt.yticks([])
#plt.subplot(236), plt.imshow(myblur), plt.title('My Gauss')
#plt.xticks([]), plt.yticks([])


plt.show()

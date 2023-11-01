import numpy as np
import matplotlib.pyplot as plt
import cv2
from scipy.signal import convolve2d as conv2
from skimage import restoration, metrics

class Image:
    def __init__(self, file):
        self.file = file

    def read(self):  # read
        file = cv2.imread(self.file)
        ycrcb = cv2.cvtColor(file, cv2.COLOR_BGR2YCR_CB)  # change to YCRCB
        Y, CR, CB = cv2.split(ycrcb) 
        return Y, CR, CB, file

    def Gaussian(self, x, y):
        kernel_1d = cv2.getGaussianKernel(x, y)
        kernel_2d = kernel_1d * kernel_1d.T
        return kernel_2d

    def mean_filter(self, Y):
        m, n = np.shape(Y)
        new = np.zeros([m, n])
        for i in range(1, m-1):
            for j in range(1, n-1):
                temp = [Y[i-1, j-1],
                        Y[i-1, j],
                        Y[i-1, j + 1],
                        Y[i, j-1],
                        Y[i, j],
                        Y[i, j + 1],
                        Y[i + 1, j-1],
                        Y[i + 1, j],
                        Y[i + 1, j + 1]]
                temp = sorted(temp)
                new[i, j]= temp[4]  # find the median
 
        new = new.astype(np.uint8)
        return new

    def motion_blur(self, size):
        kernel_motion_blur = np.zeros((size, size))
        kernel_motion_blur[int((size-1)/2), :] = np.ones(size)
        kernel_motion_blur = kernel_motion_blur / size
        return kernel_motion_blur



# input1
input1 = Image('C:\Python\HW3\input1.bmp')
Y, CR, CB, file = input1.read()
input1_ori = Image('C:\Python\HW3\input1_ori.bmp')
Y_ori, CR_ori, CB_ori, file_ori = input1_ori.read()

Y = Y/255
kernel = input1.Gaussian(30, 10)
deconvolved_RL = restoration.richardson_lucy(Y, kernel, 7)
deconvolved_RL = cv2.normalize(deconvolved_RL,None,0,255,cv2.NORM_MINMAX, cv2.CV_8U)
image = cv2.cvtColor(cv2.merge([deconvolved_RL, CR, CB]), cv2.COLOR_YCrCb2BGR)
cv2.imwrite('C:\Python\HW3\output1_rl.bmp', image)

deconvolved_wnr = restoration.wiener(Y, kernel, 50)
deconvolved_wnr = cv2.normalize(deconvolved_wnr,None,0,255,cv2.NORM_MINMAX, cv2.CV_8U)
image_wnr = cv2.cvtColor(cv2.merge([deconvolved_wnr, CR, CB]), cv2.COLOR_YCrCb2BGR)
cv2.imwrite('C:\Python\HW3\output1_wnr.bmp', image_wnr)

psnr_1 = metrics.peak_signal_noise_ratio(file_ori, image)
psnr_2 = metrics.peak_signal_noise_ratio(file_ori, file)
print(psnr_1, psnr_2)
psnr_1 = metrics.peak_signal_noise_ratio(file_ori, image_wnr)
psnr_2 = metrics.peak_signal_noise_ratio(file_ori, file)
print(psnr_1, psnr_2)


# input2
input2 = Image('C:\Python\HW3\input2.bmp')
Y, CR, CB, file = input2.read()
input2_ori = Image('C:\Python\HW3\input2_ori.bmp')
Y_ori, CR_ori, CB_ori, file_ori = input2_ori.read()

Y = Y / 255
kernel_motion_blur = input2.motion_blur(5)
deconvolved_RL = restoration.richardson_lucy(Y, kernel_motion_blur, 7)
deconvolved_RL = cv2.normalize(deconvolved_RL,None,0,255,cv2.NORM_MINMAX, cv2.CV_8U)
image = cv2.cvtColor(cv2.merge([deconvolved_RL, CR, CB]), cv2.COLOR_YCrCb2BGR)
cv2.imwrite('C:\Python\HW3\output2_rl.bmp', image)

deconvolved_wnr = restoration.wiener(Y, kernel_motion_blur, 5)
deconvolved_wnr = cv2.normalize(deconvolved_wnr,None,0,255,cv2.NORM_MINMAX, cv2.CV_8U)
image_wnr = cv2.cvtColor(cv2.merge([deconvolved_wnr, CR, CB]), cv2.COLOR_YCrCb2BGR)
cv2.imwrite('C:\Python\HW3\output2_wnr.bmp', image_wnr)

psnr_1 = metrics.peak_signal_noise_ratio(file_ori, image)
psnr_2 = metrics.peak_signal_noise_ratio(file_ori, file)
print(psnr_1, psnr_2)
psnr_1 = metrics.peak_signal_noise_ratio(file_ori, image_wnr)
psnr_2 = metrics.peak_signal_noise_ratio(file_ori, file)
print(psnr_1, psnr_2)


# input3
input3 = Image('C:\Python\HW3\input3.bmp')
Y, CR, CB, file = input3.read()
input3_ori = Image('C:\Python\HW3\input3_ori.bmp')
Y_ori, CR_ori, CB_ori, file_ori = input3_ori.read()

mean = input3.mean_filter(Y)
mean = mean/255
kernel = input3.Gaussian(5, 5)
deconvolved_RL = restoration.richardson_lucy(mean, kernel, 7)
deconvolved_RL = cv2.normalize(deconvolved_RL,None,0,255,cv2.NORM_MINMAX, cv2.CV_8U)
image = cv2.cvtColor(cv2.merge([deconvolved_RL, CR, CB]), cv2.COLOR_YCrCb2BGR)
cv2.imwrite('C:\Python\HW3\output3_rl.bmp', image)

deconvolved_wnr = restoration.wiener(mean, kernel, 50)
deconvolved_wnr = cv2.normalize(deconvolved_wnr,None,0,255,cv2.NORM_MINMAX, cv2.CV_8U)
image_wnr = cv2.cvtColor(cv2.merge([deconvolved_wnr, CR, CB]), cv2.COLOR_YCrCb2BGR)
cv2.imwrite('C:\Python\HW3\output3_wnr.bmp', image_wnr)

psnr_1 = metrics.peak_signal_noise_ratio(file_ori, image)
psnr_2 = metrics.peak_signal_noise_ratio(file_ori, file)
print(psnr_1, psnr_2)
psnr_1 = metrics.peak_signal_noise_ratio(file_ori, image_wnr)
psnr_2 = metrics.peak_signal_noise_ratio(file_ori, file)
print(psnr_1, psnr_2)
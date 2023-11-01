import numpy as np
import matplotlib.pyplot as plt
import cv2
from math import exp, sqrt

class Image:
    def __init__(self, file):
        self.file = file

    def read(self):  # read
        file = cv2.imread(self.file)
        ycrcb = cv2.cvtColor(file, cv2.COLOR_BGR2YCR_CB)  # change to YCRCB
        Y, CR, CB = cv2.split(ycrcb) 
        return Y, CR, CB, file

    def write(self, Y, CR, CB, name): # write
        image = cv2.cvtColor(cv2.merge([Y, CR, CB]), cv2.COLOR_YCrCb2BGR) # change to BGR
        cv2.imwrite(name, image)

    def power_law(self, Y, gamma): # power law transformation
        gamma_corrected = np.array(255*(Y / 255) ** gamma, dtype = 'uint8')
        return gamma_corrected
       
    def histogram(self, Y, mask):  # histogram equalization
        flat = np.ravel(Y)
        if mask == "none":
            hist = np.histogram(Y, 256, [0,255])  # do histogram
        else:
            hist = np.histogram(mask, 256, [0,255])  # do histogram with mask
        cumsum = np.cumsum(hist[0])
        normalize = ((cumsum - cumsum.min()) * 255 / (cumsum.max() - cumsum.min())).astype('uint8')
        image = normalize[flat]
        image = np.reshape(image, np.shape(Y))
        return image

    def mask(self, file, Y):  # create a mask
        mask = np.zeros(file.shape[:2], np.uint8)
        mask[800:2000, 1200:1800] = 255   # set location and color
        masked_img = cv2.bitwise_and(Y, Y, mask=mask)  # put the mask amd figure together
        return masked_img

    def contraharmonic_mean(self, Y, size, Q):  # contraharmonic mean filter
        num = np.power(Y, Q + 1)
        denom = np.power(Y, Q)
        kernel = np.full(size, 1.0)
        result = cv2.filter2D(num, -1, kernel) / cv2.filter2D(denom, -1, kernel)  # convolution
        return result

    def FFT(self, Y):  # fourier transform
        rows,cols = np.shape(Y)  # (x,y)
        new_rows = cv2.getOptimalDFTSize(rows)  # change in 2^n
        new_cols = cv2.getOptimalDFTSize(cols)
        new_img = np.zeros((new_rows, new_cols))
        new_img[:rows, :cols] = Y
        dft = cv2.dft(np.float32(new_img), flags=cv2.DFT_COMPLEX_OUTPUT)  # do fft
        dft_shift = np.fft.fftshift(dft)  # shift the fft
        magnitude_spectrum = 20 * np.log(cv2.magnitude(dft_shift[:,:,0], dft_shift[:,:,1]))   # real part  
        cv2.imwrite('./HW2/output_fft.bmp', magnitude_spectrum)  # create the fft figure
        return magnitude_spectrum, dft, dft_shift

    def inverse_FFT(self, FFT):  # inverse fourier transform
        f_ishift = np.fft.ifftshift(FFT)  # shift back
        img_back = cv2.idft(f_ishift)  # inverse fft
        img_back = cv2.magnitude(img_back[:,:,0], img_back[:,:,1])
        img_back = cv2.normalize(img_back,None,0,255,cv2.NORM_MINMAX, cv2.CV_8U)  # normalize 0-255
        return img_back

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

    def homomorphic_filtering(self, Y, yh, yl, c, d0):  # yh, yl, c, d0 = 2, 0.5, 2, 100
        du = np.zeros(np.shape(Y), dtype = np.float32)
        for u in range(np.shape(Y)[0]):
            for v in range(np.shape(Y)[1]):
                du[u,v] = sqrt((u - np.shape(Y)[0]/2.0)*(u - np.shape(Y)[0]/2.0) + (v - np.shape(Y)[1]/2.0)*(v - np.shape(Y)[1]/2.0))
        
        du2 = cv2.multiply(du,du) / (d0*d0)
        re = np.exp(- c * du2)
        H = (yh - yl) * (1 - re) + yl  # function of H(u, v)

        I_log = np.log1p(np.array(Y, dtype="float")) 
        I_fft = np.fft.fft2(I_log)

        filtered = I_fft * H

        inverse = np.fft.ifft2(filtered)
        I = np.exp(np.real(inverse)) - 1
        I = cv2.normalize(I,None,0,255,cv2.NORM_MINMAX, cv2.CV_8U)
        return I

    
    def bandreject_filter(self, Y, d0=160, w=20, n=2):
        H = np.ones(np.shape(Y))

        for u in range(0, np.shape(Y)[0]):
            for v in range(0, np.shape(Y)[1]):
                D_uv = np.sqrt((u - (np.shape(Y)[0]/2))**2 + (v - (np.shape(Y)[1]/2))**2)
                if D_uv == d0: 
                    H[u, v] = 0
                else:
                    H[u, v] = 1/(1 + ((D_uv*w)/(D_uv**2 - d0**2))**(2*n))
        return H



# input1.bmp
input1 = Image('./HW2/input1.bmp')
Y, CR, CB, file = input1.read()
power_law = input1.power_law(Y, 0.5)
mean_filter = input1.mean_filter(power_law)
input1.write(mean_filter, CR, CB, './HW2/output1.bmp')

# input2.bmp
input2 = Image('./HW2/input2.bmp')
Y, CR, CB, file = input2.read()
mask = input2.mask(file, Y)
#cv2.imwrite('./HW2/masked.bmp', mask)
hist = input2.histogram(Y, mask)
input2.write(hist, CR, CB, './HW2/output2.bmp')

# input3.bmp
input3 = Image('./HW2/input3.bmp')
Y, CR, CB, file = input3.read()
hist = input3.histogram(Y, "none")
mean_filter = input3.mean_filter(hist)
input3.write(mean_filter, CR, CB, './HW2/output3.bmp')

# input4.bmp
input4 = Image('./HW2/input4.bmp')
Y, CR, CB, file = input4.read()
homomorphic_filtering = input4.homomorphic_filtering(Y, 2, 0.5, 2, 100)
input4.write(homomorphic_filtering, CR, CB, './HW2/output4.bmp')



#====================================================================================================


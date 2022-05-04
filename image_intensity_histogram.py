''' 
tested and developed on Google Colab for ease of use.
display a plot showing the intensity levels in a given image.
'''

import cv2
import numpy as np
from matplotlib import pyplot as plt
from google.colab.patches import cv2_imshow # see: https://github.com/jupyter/notebook/issues/3935
  
# read in frame as grayscale
frame = cv2.imread('./frame_out0.pgm', cv2.IMREAD_GRAYSCALE)
cv2_imshow(frame)

dst = cv2.calcHist(frame, [0], None, [256], [0,256])

plt.figure(figsize=(10,6), dpi=80)
plt.hist(frame.ravel(),255,[240,255])
plt.axvspan(254.5, 255.5, color='red', alpha=0.5)
plt.title('Histogram for gray scale frame')
plt.xlabel('Pixel intensity')
plt.ylabel('Pixels count')
plt.ylim(top=300)
plt.show()

# show count of each unique value near full saturation
intensityFreq = {}

intensityList = np.unique(frame.ravel(), return_counts=True)[0]
countList = np.unique(frame.ravel(), return_counts=True)[1]

# append in a dict for easier manipulation
for i in range(len(intensityList)):
    intensityFreq[intensityList[i]] = countList[i]

# sort by pixels count in descending order
intensityFreq = {k: v for k, v in sorted(intensityFreq.items(), key=lambda x: x[1], reverse=True)}

for k,v in intensityFreq.items():
    print(f'Intensity level [{k}] appeared in [{v}] pixels.')
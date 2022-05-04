# track_laser_pointer
A program to detect the foreground target (laser spot) using histogram analysis and frame differencing, and lock on target by overlaying a crosshair marker on the center of the detected spot.

# build and run
1. To perform the histogram analysis and set an appropriate thresholds, run `image_intensity_histogram.py` using Google colab for conveience (or use python 3 and required python modules locally). Change line #12 to specify the path of the of the frame to be inspected.
2. For object detection:
    1. Set threshold per your findings in the preivous step. See line #23 in `lock_on_target.cpp`.
    2. use the attached makefile to build the source code as follows:
    ```
    make
    ```
    3. run the executable and provide the input video as a command line argument. For a video file name `in_video.mpeg` that has been sliced into individual frames (see [rgb2gray_slicer](https://github.com/Feras-dev/rgb2gray_slicer)) in pgm format and saved under the folder named `PATH_TO_FRAMES_FOLDER`, use:
    ```
    ./lock_on_target ./PATH_TO_FRAMES_FOLDER
    ```
    4. Consider usinf ffmpeg to stitch the output frames back into a single video. E.g., `ffmpeg -start_number 0 -i frame_out%d.pgm -vcodec mpeg4 out_video.mp4`.

# Example: Histogram analysis 
The analysis shown below exposed the difference between full-saturation level and the near-full-saturation levels. The difference is large enough to allow us to solve the problem without the need for edge enhancement. This is based on this example only, and edge enhancement methods may be required for other images (e.g., RGB scale, with cluttered background.. etc).

The first frame was analyzed per-pixel for each pixel’s intensity level to determine an appropriate threshold. A combination of OpenCV, Matplotlib, NumPy in Python 3.7 (using Google Colab) were used to perform the preliminary analysis.
The first frame in gray scale (see original file at: q5/frames_out_chan_0/frame_out0.pgm):

![histogram_analysis_frame_used](https://user-images.githubusercontent.com/72912013/166822975-f3e51915-9976-4330-8fb7-9014e384da40.png)

Since OpenCV in Python stores each image object (equivalent to Mat object in C++) as a NumPy array, using NumPy and Matplotlib, the full array of pixels was easily plotted as a histogram.

![histogram_full_spectrum](https://user-images.githubusercontent.com/72912013/166823293-a999446f-f1e0-4d0d-a857-6cd2d8f318d0.png)

However, note that due to the count of pixels with intensity values of ~0, we are not able to see any other bins easily. One way to get around this is by looking at the upper half of the saturation level (128-255, as opposed to the full range of 0-255).

![histogram_upper_half_intesity](https://user-images.githubusercontent.com/72912013/166823351-4bfa43ad-19be-4ae9-9a2f-5076096ec5c8.png)

Alternatively, we could limit the top of our y-axis to a reasonable value (~300). Note that this will introduce some cropping in the bar’s height in the lower saturation levels.

![histogram_lower_frequency](https://user-images.githubusercontent.com/72912013/166823393-b29edcf1-f13c-4169-9207-d933631f0f2e.png)

To further isolate the area of interest to us for the purpose of solving this question, we can limit both the x (240-255) and y (0-300) axis.

![histogram_lower_frequency_upper_half_intensity](https://user-images.githubusercontent.com/72912013/166823421-13adc3e0-0696-42d4-b9db-b018586ee0f3.png)

Now, let’s inspect the exact count of the most frequent saturation levels (top 15 shown below).

![histogram_stats](https://user-images.githubusercontent.com/72912013/166823446-6f395e9d-99bc-4a9f-a340-d60f31411008.png)

From the analysis shown above, setting our thresholding function as follows seems reasonable:

![histogram_analysis_threshold_function](https://user-images.githubusercontent.com/72912013/166823479-995c855c-fa27-4d1f-8969-001f7b731e13.png)

And for our histogram, this would look like the shaded red region as shown below:

![histogram_region_of_interest](https://user-images.githubusercontent.com/72912013/166823706-36bcb08b-bd1a-4fd6-83f6-dbeb74961166.png)


# Example: Object tracking
## Input video
![in_video](https://user-images.githubusercontent.com/72912013/166824029-c55682cf-c3b3-4867-b45d-2a31b5ad81e8.gif)

## Output video
![out_video](https://user-images.githubusercontent.com/72912013/166824084-aa939ffd-05d8-4425-add1-b6d18dfb9163.gif)

# Future work
1. Match frame rate 100% in real-time.
2. Enable frames stitching from the object detection program or via a bash script.
3. Incorporate all programs to be ran consecutively using a BASH shell script. Wait for user input between steps. Share threshold value with program using a pipe. 

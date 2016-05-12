# marr-hildreth-edge-detection

In this project Marr-Hildreth edge detection is implemented to output a binary image of edge points.  

Combines Laplacian with 2D Gaussian which is Laplacian of a Gaussian (LoG).  

Algorithm:  

- Filter image with N x N Gaussian filter.
- Compute Laplacian using 3 x 3 mask or kernel.
- Find zero crossing of image from above step.

This filter text file will have the following format:  

    <rows> <columns>  
    <H/ L>  
    <mask values>  

The &lt;H/L&gt; part will either be H = high-pass filter or L = low-pass filter.  

A (3 by 3) Gaussian filter would look like this:

    3 3  
    L  
    1  2  1  
    2  4  2  
    1  2  1

A (3 by 3) Laplacian filter would look like this:

    3 3  
    H  
    1  1  1  
    1 -8  1  
    1  1  1

Images to undergo edge detection are in inputDir, two filters laplacian and gaussian are in filterDir, after computation images are saved to outputDir.  

The program takes three command line arguments  

    marrHildreth ./inputDir ./filterDir ./outputDir

Each image is blurred using the 3 x 3 Gaussian filter 9 times. This is approximately equivalent to filtering with a 9 x 9 filter. In other words, each image is blurred, and then the output of the previous blurring is used as the input to the next blurring. Then Laplacian filter is applied to the final output image of Gaussian blurring. Zero crossing points are computed for output image of Laplacian filter to get the final edge map. Finally, edge map is save to output directory.

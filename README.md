# HPC-LowPassFilter
---
## How to Run
#### This project depends on the MPI API, so make sure to update in the project's properties page:

##### Properties Configuration
1. (All configurations) path\to\your\mpi\folder\include (in the C/C++ tab > General > Additional Include Directories)
2. (All configurations) path\to\your\mpi\folder\lib\[x86/x64] (in the Linker tab > General > Additional Library Directories)
3. (All configurations) type "msmpi.lib" (in the Linker tab > Input > Additional Dependencies)

##### Specifying Input & Output Directories
1. Set the output folder at line 78. The default is "../Data/Output/"
2. Set the input image path at line 250. "../Data/Test Cases/"

##### Executing The Parallel/Sequential Code
1. Uncomment line 259 for a sequential filter, or 262 for a parallel one
2. Copy the text under the line you just uncommented for step #4
3. Open cmd where the HPC_ProjectTemplate.exe resides
4. Paste the line from step #2

---

## Results
<img src="https://github.com/Amr-Wael-Dev/HPC-LowPassFilter/blob/main/Resources/test1.png" alt="Image 1 Before Applying The Filter" title="Image 1 Before Applying The Filter" width="500" height="500">&nbsp;<img src="https://github.com/Amr-Wael-Dev/HPC-LowPassFilter/blob/main/Resources/res1.png" alt="Image 1 After Applying The Filter" title="Image 1 After Applying The Filter" width="500" height="500">

<img src="https://github.com/Amr-Wael-Dev/HPC-LowPassFilter/blob/main/Resources/test2.jpg" alt="Image 2 Before Applying The Filter" title="Image 2 Before Applying The Filter" width="500" height="500">&nbsp;<img src="https://github.com/Amr-Wael-Dev/HPC-LowPassFilter/blob/main/Resources/res2.png" alt="Image 2 After Applying The Filter" title="Image 2 After Applying The Filter" width="500" height="500">

<img src="https://github.com/Amr-Wael-Dev/HPC-LowPassFilter/blob/main/Resources/test3.jpeg" alt="Image 3 Before Applying The Filter" title="Image 3 Before Applying The Filter" width="500" height="500">&nbsp;<img src="https://github.com/Amr-Wael-Dev/HPC-LowPassFilter/blob/main/Resources/res3.png" alt="Image 3 After Applying The Filter" title="Image 3 After Applying The Filter" width="500" height="500">

---

# Further Reading
## Parallel Low Pass Filter (Blurring)
It is used to make images appear smoother. Low pass filtering smooths out noise. It allows low frequency components of the image to pass through and blocks high frequencies. Low pass image filters work by convolution which is a process of making each pixel of an image by a fixed size kernel.

<img src="https://github.com/Amr-Wael-Dev/HPC-LowPassFilter/blob/main/Resources/1.jpg" alt="Aligning The Kernel Onto The Image" title="Aligning The Kernel Onto The Image" width="400" height="200">

Imagine that the given
image is I, the kernel is K. If we applied the filter on the red region at I, the result will be
computed by aligning the kernel onto the image part then doing basic multiplication
between the aligned elements as: (1 * 1 + 0 * 0 + 0 * 1) + (0 * 1 + 1 * 1 + 0 * 0) + (1 * 1 + 1 * 0 + 1 * 1) = 4. Every kernel based filter has its kernel, the low pass filter has this as a kernel:

<img src="https://github.com/Amr-Wael-Dev/HPC-LowPassFilter/blob/main/Resources/2.jpg" alt="Aligning The Kernel Onto The Image" title="Aligning The Kernel Onto The Image">

For the border elements of the input image (e.g. pixel (0,0) ), there are two ways to handle this either by zero padding or border replication.

---

## For more information about how blurs & filters work, I suggest you check out [this video](https://www.youtube.com/watch?v=C_zFhWdM4ic)

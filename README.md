## Parallel Grayscale Conversion using OpenCL and OpenCV

Welcome to the Parallel Grayscale Conversion project! This program efficiently converts RGB images to grayscale using parallel processing with OpenCL, leveraging the power of GPUs for fast image processing. The project uses OpenCV for image handling and OpenCL for parallel computation.

## Features

    High Performance: Utilizes GPU acceleration with OpenCL for fast grayscale conversion.
    Flexible Input: Add more images to the input folder to process additional files beyond the provided samples.
    Easy Integration: Simple directory-based input and output handling for seamless integration into your workflow.

## Prerequisites

    OpenCV: Ensure OpenCV is installed on your system. You can install it using the following command:
    sudo apt-get install libopencv-dev

    OpenCL: Ensure OpenCL is installed and your GPU supports it. You may need to install the appropriate drivers for your GPU.


## Installation

    Clone the repository:
    git clone https://github.com/yourusername/parallel-grayscale-conversion.git
    cd parallel-grayscale-conversion

    Build the project:
    g++ -o grayscale_converter grayscale_converter.cpp -lOpenCL pkg-config --cflags --libs opencv4


## Usage

    Prepare the Input Folder:
        Place your RGB images in the input_folder. The current folder contains sample images to demonstrate functionality. You can add or remove images as needed.

    Run the Program:
    ./grayscale_converter

    Check the Output Folder:
        The converted grayscale images will be saved in the output_folder.


## Directory Structure

.
├── grayscale_converter.cpp
├── input_folder
│ ├── sample1.jpg
│ ├── sample2.jpg
│ └── ... (add more images here)
├── output_folder
│ └── (grayscale images will be saved here)
└── README.md


## How it Works

    Initialization:
        The program initializes OpenCL and loads the kernel for grayscale conversion.
        It sets up the input and output folders for processing images.

    Processing:
        For each image in the input folder:
            The image is loaded using OpenCV.
            The image is converted to RGBA format.
            The image data is transferred to the GPU.
            The grayscale conversion kernel is executed on the GPU.
            The resulting grayscale image is read back from the GPU.
            The grayscale image is saved to the output folder.

    Parallel Processing:
        The OpenCL kernel processes each pixel in parallel, leveraging the GPU's computational power for efficient conversion.


## Adding More Images

    You can add more images to the input_folder to process them. The program dynamically handles all images present in the folder, converting each one to grayscale and saving the result in the output_folder.

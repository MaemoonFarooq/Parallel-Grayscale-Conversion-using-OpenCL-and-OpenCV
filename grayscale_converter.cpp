#include <opencv2/opencv.hpp>
#include <CL/cl.hpp>
#include <iostream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

// stucture for the pixel storage
struct uchar4 {
    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned char w;
};

//grayscale conversion using OpenCL kernel
const char* kernelSource = R"(
    __kernel void rgb_to_gray(__global const uchar4* inputImage,
                              __global unsigned char* outputImage,
                              const int width,
                              const int height) {
        int x = get_global_id(0);
        int y = get_global_id(1);

        if (x < width && y < height) {
            int index = y * width + x;
            uchar4 pixel = inputImage[index];

            // Grayscale conversion formula: Y = 0.299*R + 0.587*G + 0.114*B
            unsigned char gray = (unsigned char)(0.299f * pixel.x + 0.587f * pixel.y + 0.114f * pixel.z);

            outputImage[index] = gray;
        }
    }
)";

int main() {
    //Input folder path
    string inputFolder = "input_folder";
    
    //Output folder path
    string outputFolder = "output_folder";

    //Initializing OpenCL
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        std::cerr << "No OpenCL platforms found." << std::endl;
        return -1;
    }

    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
    cl::Context context(CL_DEVICE_TYPE_GPU, properties);

    std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    cl::CommandQueue commandQueue(context, devices[0]);

    //Loading kernel
    cl::Program::Sources source(1, std::make_pair(kernelSource, strlen(kernelSource)));

    //Creating program
    cl::Program program(context, source);

    //Building program for devices
    program.build(devices);

    //Iterating through the input folder
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        
        //Image loading done by using OpenCV
        cv::Mat colorImage = cv::imread(entry.path().string(), cv::IMREAD_COLOR);

        //Check if the image is empty
        if (colorImage.empty()) {
            std::cerr << "Failed to load image: " << entry.path().string() << std::endl;
            //else continue
            continue;
        }

        //Converting the image to RGBA format
        cv::Mat rgbaImage;
        cv::cvtColor(colorImage, rgbaImage, cv::COLOR_BGR2BGRA);

        //Image dimensions width
        int width = rgbaImage.cols;

        //Image dimensions height
        int height = rgbaImage.rows;

        //Create OpenCL buffers for input images
        cl::Buffer inputBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uchar4) * width * height, rgbaImage.data);
        
        //Create OpenCL buffers for output images
        cl::Buffer outputBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned char) * width * height);

        //Creating kernel
        cl::Kernel kernel(program, "rgb_to_gray");

        //Setting kernel arguments
        kernel.setArg(0, inputBuffer);
        kernel.setArg(1, outputBuffer);
        kernel.setArg(2, width);
        kernel.setArg(3, height);

        //Define global and local work sizes
        cl::NDRange globalWorkSize(width, height);
        cl::NDRange localWorkSize(16, 16); // Adjust according to your device

        //Enqueue the kernel
        commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, globalWorkSize, localWorkSize);

        //Reading the output buffer
        unsigned char* outputData = new unsigned char[width * height];
        commandQueue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(unsigned char) * width * height, outputData);

        //Saving the grayscale image to the output folder
        cv::Mat grayImage(height, width, CV_8UC1, outputData);
        cv::imwrite(outputFolder + "/" + entry.path().filename().string(), grayImage);

        //Freeing up the resorces
        delete[] outputData;
    }

    return 0;
}

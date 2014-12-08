You will find
- A program to display and manipulate images. This has been
given as Microsoft Visual C++ .dsp and .dsw project files along with
the accompanying code in the .cpp and .h files
- Example images, which are in the native RGB formats. They are
as follows with dimensions and brief description
image1.rgb -> 640x480 shows a weather map
image2.rgb -> 640x480 shows an expanding spiral
image3.rgb -> 640x480 shows an image of text
image4.rgb -> 352x288 shows a construction site
image5.rgb -> 480x360 shows a city by night
image6.rgb -> 320x240 shows a few buildings with clouds
image7.rgb -> 512x512 shows an example of a gray (not color) image

Each image is different in the colors, frequency content etc. and should
serve as a good example for playing with subsampling, color space transformations,
quantization, compression etc.

The project includes the following important files.
1. Image.h	- header file for MyImage class
2. Image.cpp	- defines operations on an image such as read, write, modify
3. Main.cpp	- entry point of the application, takes care of the GUI and the 
		  image display operations

Some indicators have been placed in the code to guide you to develop your code. But you
you are free to modify the program in any way to get the desirable output.

- Unzip the folder in your desired location
- Launch Visual Studio and load the .dsw or .dsp project files
- If you use the .net compiler, you can still load the project file, just 
  follow the default prompts and it will create a solution .sln file for you
- Compile the program to produce a an executable Image.exe
- To run the program you need to provide the program with command line arguments, they can 
  be passed in Visual C++ using Project > Settings or just launch a .exe file from the command prompt
- Here is the usage (for the given example images)

Image.exe image1.rgb 640 480
Image.exe image2.rgb 640 480
Image.exe image3.rgb 640 480
Image.exe image4.rgb 352 288
Image.exe image5.rgb 480 360
Image.exe image6.rgb 320 240
Image.exe image7.rgb 512 512
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/videodev2.h>

using namespace cv;
using namespace std;

int edgeThresh = 1;
Mat image, gray, edge, cedge;

int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbp = 0;

int displayDeviceInit()
{
	fbfd = open("/dev/fb0", O_RDWR);
	if(!fbfd)
	{
		printf("Error: can't open framebuffer device");
		return 1;
	}

	if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
		printf("Error reading fixed information");
		return 1;
	}

	if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error reading variable information");
		return 1;
	}	

	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

	if((int)fbp == -1)
	{
		printf("Error failed to mapped framebuffer device to memory \n");
		return 1;
	}
	printf("Mapped framebuffer device to memory successfully\n");
}

static void hdmi_show(Mat& outputimage)
{
	int x = 100, y = 100;

	for (x = 100; x < 100 + outputimage.cols; x++)
		for (y = 100; y < 100 + outputimage.rows; y++)
		{
			long int location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;
			unsigned r, g, b;
			r = outputimage.at<Vec3b>(y-100,x-100)[2];
			g = outputimage.at<Vec3b>(y-100,x-100)[1];
			b = outputimage.at<Vec3b>(y-100,x-100)[0];
			unsigned *ptr = (unsigned *)(fbp + location);			
			*ptr = r << 16 | g << 8 | b;			
		}
}

// define a trackbar callback
static void onTrackbar(int, void*)
{
    std::vector<cv::KeyPoint> keypoints;
    cv::FAST( gray, keypoints, 20, true);
    image.copyTo(edge);

    for (int i=0;i<keypoints.size();i++)
    {
      rectangle(edge, cvPoint(keypoints[i].pt.x-1,keypoints[i].pt.y-1),
		cvPoint(keypoints[i].pt.x+1,keypoints[i].pt.y+1), cvScalar(255,0,0),CV_FILLED);
    }

    image.copyTo(cedge, edge);
    imwrite("edge.bmp",edge);
}


const char* keys =
{
    "{1| |lena.jpg|input image name}"
};

int main( int argc, const char** argv )
{
    CommandLineParser parser(argc, argv, keys);
    string filename = parser.get<string>("1");
    image = imread(filename, 1);
    if(image.empty())
    {
        printf("Cannot read image file: %s\n", filename.c_str());
        return -1;
    }
    cedge.create(image.size(), image.type());
    cvtColor(image, gray, COLOR_BGR2GRAY);
    onTrackbar(0, 0);
   
	displayDeviceInit();
    while(1)
    {
		hdmi_show(edge);
    }  
    
    return 0;
}

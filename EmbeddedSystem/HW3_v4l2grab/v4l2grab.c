#include "v4l2grab.h"

#define  TRUE	1
#define  FALSE	0

#define FILE_VIDEO 	"/dev/video0"
#define BMP      	"./image_bmp.bmp"
#define YUV			"./image_yuv.yuv"

#define  IMAGEWIDTH    640
#define  IMAGEHEIGHT   480

static  int     fd;
static  struct  v4l2_capability     cap;
        struct  v4l2_fmtdesc        fmtdesc;
        struct  v4l2_format         fmt,fmtack;
        struct  v4l2_streamparm     setfps;
        struct  v4l2_requestbuffers req;
        struct  v4l2_buffer         buf;
        enum    v4l2_buf_type       type;

unsigned char frame_buffer[IMAGEWIDTH*IMAGEHEIGHT*3];
BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;

struct buffer
{
	void * start;
	unsigned int length;
} *buffers;

int init_v4l2(void)
{
	// 1. Open device////////////////////////////////////
	if ((fd = open(FILE_VIDEO, O_RDWR)) == -1)
	{
		printf("Error opening V4L interface\n");
		return  FALSE;
	}
    /////////////////////////////////////////////////////

    // 2. Device setup///////////////////////////////////
	// Query device capabilities
	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
	{
		printf("Error opening device %s: unable to query device.\n",FILE_VIDEO);
		return  FALSE;
	}
	else
	{
     	printf("driver:\t\t%s\n"    , cap.driver);
     	printf("card:\t\t%s\n"      , cap.card);
     	printf("bus_info:\t%s\n"    , cap.bus_info);
     	printf("version:\t%d\n"     , cap.version);
     	printf("capabilities:\t%x\n", cap.capabilities);

     	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
			printf("Device %s: supports capture.\n",FILE_VIDEO);

		if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
			printf("Device %s: supports streaming.\n",FILE_VIDEO);
	}

	// Print support format
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("Support format:\n");
	while(ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
	{
		printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
		fmtdesc.index++;
	}

    // Set format
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.height = IMAGEHEIGHT;
	fmt.fmt.pix.width = IMAGEWIDTH;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    // Set the data format
	if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
	{
		printf("Unable to set format\n");
		return FALSE;
	}
    // Get the data format
	if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
	{
		printf("Unable to get format\n");
		return FALSE;
	}
	{
     	printf("fmt.type:\t\t%d\n", fmt.type);
     	printf("pix.pixelformat:\t%c%c%c%c\n", fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF, (fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
     	printf("pix.height:\t\t%d\n", fmt.fmt.pix.height);
     	printf("pix.width:\t\t%d\n", fmt.fmt.pix.width);
     	printf("pix.field:\t\t%d\n", fmt.fmt.pix.field);
	}
	// Set fps
	setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	setfps.parm.capture.timeperframe.numerator = 10;
	setfps.parm.capture.timeperframe.denominator = 10;
    /////////////////////////////////////////////////////

	printf("init %s \t[OK]\n", FILE_VIDEO);

	return TRUE;
}

int v4l2_grab(void)
{
	unsigned int n_buffers;

    // 3. Memory map/////////////////////////////////////
	// Request for 4 buffers
	req.count  = 4;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
    // Initiate memory mapping or user pointer I/O
	if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
		perror("Request for buffers error:");

	// Memory allocate
	buffers = malloc(req.count * sizeof(*buffers));
	if (!buffers) {
		perror("Memory allocate error:");
		return FALSE;
	}

	for (n_buffers = 0; n_buffers < req.count; n_buffers++)
	{
		buf.type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory    = V4L2_MEMORY_MMAP;
		buf.index     = n_buffers;
		// Query buffers
		if (ioctl (fd, VIDIOC_QUERYBUF, &buf) == -1) {
			perror("query buffer error\n");
			return FALSE;
		}

		buffers[n_buffers].length = buf.length;
		// Memory map
		buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ |PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
		if (buffers[n_buffers].start == MAP_FAILED) {
			printf("buffer map error\n");
			return FALSE;
		}
	}

	// Queue
	for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
		buf.index = n_buffers;
        // Enqueue an empty (capturing) or filled (output) buffer in the driver's incoming queue
		if (ioctl(fd, VIDIOC_QBUF, &buf))
            perror("Queue buffer error:");
	}
    /////////////////////////////////////////////////////

    // 4. Start capturing////////////////////////////////
    // Start streaming I/O
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &type)) {
        perror("Start streaming error:");
        return FALSE;
    }
    /////////////////////////////////////////////////////

    // 5. Read buffer////////////////////////////////////
    // Dequeue
	if (ioctl(fd, VIDIOC_DQBUF, &buf)) {
        perror("Dequeue buffer error:");
        return FALSE;
    }
    /////////////////////////////////////////////////////
	if (ioctl(fd, VIDIOC_QBUF, &buf))
            perror("Queue buffer error:");
    printf("grab yuyv OK\n");

	return TRUE;
}

void yuyv_2_rgb888(void)
{
    // 6. Image processing///////////////////////////////
	int           i, j;
    unsigned char y1, y2, u, v;
    int r1, g1, b1, r2, g2, b2;
    char *pointer;

	pointer = buffers[0].start;

    for(i = 0; i < IMAGEHEIGHT; i++)
    {
    	for(j = 0;j < (IMAGEWIDTH/2); j++)
    	{
    		y1 = *( pointer + (i*(IMAGEWIDTH/2)+j)*4);
    		u  = *( pointer + (i*(IMAGEWIDTH/2)+j)*4 + 1);
    		y2 = *( pointer + (i*(IMAGEWIDTH/2)+j)*4 + 2);
    		v  = *( pointer + (i*(IMAGEWIDTH/2)+j)*4 + 3);

    		r1 = y1 + 1.042*(v-128);
    		g1 = y1 - 0.34414*(u-128) - 0.71414*(v-128);
    		b1 = y1 + 1.772*(u-128);

    		r2 = y2 + 1.042*(v-128);
    		g2 = y2 - 0.34414*(u-128) - 0.71414*(v-128);
    		b2 = y2 + 1.772*(u-128);

            r1 = (r1>255)? 255: ((r1<0)? 0: r1);
            b1 = (b1>255)? 255: ((b1<0)? 0: b1);
            g1 = (g1>255)? 255: ((g1<0)? 0: g1);
            r2 = (r2>255)? 255: ((r2<0)? 0: r2);
            b2 = (b2>255)? 255: ((b2<0)? 0: b2);
            g2 = (g2>255)? 255: ((g2<0)? 0: g2);

    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6    ) = (unsigned char)b1;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 1) = (unsigned char)g1;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 2) = (unsigned char)r1;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 3) = (unsigned char)b2;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 4) = (unsigned char)g2;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 5) = (unsigned char)r2;

		//if(i==IMAGEHEIGHT-1&&j==0)
		//printf("rgb: %x %x %x %x %x %x\n",b1,g1,r1,b2,g2,r2);
    	}
    }
    /////////////////////////////////////////////////////
    printf("change to RGB OK \n");
}

int close_v4l2(void)
{
    // 9. Close device///////////////////////////////////
    if(fd != -1) {
        close(fd);
        return TRUE;
    }
    return FALSE;
    /////////////////////////////////////////////////////
}

void stop_streaming(void)
{
    unsigned int n_buffers;

    // 7. Stop streaming/////////////////////////////////
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type))
        perror("Stop streaming error:");
    /////////////////////////////////////////////////////

    // 8. Memory unmap///////////////////////////////////
    for (n_buffers = 0; n_buffers < req.count; n_buffers++)
        munmap(buffers[n_buffers].start, buffers[n_buffers].length);
    /////////////////////////////////////////////////////

    // 9. Close device///////////////////////////////////
    close_v4l2();
    /////////////////////////////////////////////////////
}

void output_bmp_header(FILE *fp1, FILE *fp2)
{
    // Set BITMAPFILEHEADER
    bf.bfType = 0x4d42;
    bf.bfSize = 54 + bi.biSizeImage;
    bf.bfReserved = 0;
    bf.bfOffBits = 54;
    // Output BITMAPFILEHEADER
    fwrite(&bf.bfType, 2, 1, fp1);
    fwrite(&bf.bfSize, 4, 1, fp1);
    fwrite(&bf.bfReserved, 4, 1, fp1);
    fwrite(&bf.bfOffBits, 4, 1, fp1);

    // Set BITMAPINFOHEADER
    bi.biSize = 40;
    bi.biWidth = IMAGEWIDTH;
    bi.biHeight = IMAGEHEIGHT;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = 0;
    bi.biSizeImage = IMAGEWIDTH*IMAGEHEIGHT*3;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    // Output BITMAPINFOHEADER
    fwrite(&bi.biSize, 4, 1, fp1);
    fwrite(&bi.biWidth, 4, 1, fp1);
    fwrite(&bi.biHeight, 4, 1, fp1);
    fwrite(&bi.biPlanes, 2, 1, fp1);
    fwrite(&bi.biBitCount, 2, 1, fp1);
    fwrite(&bi.biCompression, 4, 1, fp1);
    fwrite(&bi.biSizeImage, 4, 1, fp1);
    fwrite(&bi.biXPelsPerMeter, 4, 1, fp1);
    fwrite(&bi.biYPelsPerMeter, 4, 1, fp1);
    fwrite(&bi.biClrUsed, 4, 1, fp1);
    fwrite(&bi.biClrImportant, 4, 1, fp1);
}

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

void displayFrame()
{
	int x = 100, y = 100;

	for (y = 100; y < 100 + IMAGEHEIGHT; y++)
	{
	    for (x = 100; x < 100 + IMAGEWIDTH; x++)
		{
			long int location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;

			unsigned r, g, b;
			r = frame_buffer[IMAGEHEIGHT*IMAGEWIDTH*3 -(((y - 100) * IMAGEWIDTH + (x - 100))*3 + 0) - 1];
			g = frame_buffer[IMAGEHEIGHT*IMAGEWIDTH*3 -(((y - 100) * IMAGEWIDTH + (x - 100))*3 + 1) - 1];
			b = frame_buffer[IMAGEHEIGHT*IMAGEWIDTH*3 -(((y - 100) * IMAGEWIDTH + (x - 100))*3 + 2) - 1];

			unsigned *ptr = (unsigned *)(fbp + location);
			*ptr = r << 16 | g << 8 | b;
		}
	}
}

int main(void)
{
	if (init_v4l2() == FALSE)
     	return FALSE;

    displayDeviceInit();

    v4l2_grab();

    while(1)
    {

        if (ioctl(fd, VIDIOC_DQBUF, &buf)) {
            perror("Dequeue buffer error:");
            return FALSE;
        }

        if (ioctl(fd, VIDIOC_QBUF, &buf))
                perror("Queue buffer error:");

        yuyv_2_rgb888();
        displayFrame();
    }

    stop_streaming();

    return 0;
}

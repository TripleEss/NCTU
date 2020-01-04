#include <iostream>
#include <string>
#include <algorithm>


#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2\imgproc\types_c.h>

using namespace std;
using namespace cv;

class imageCompletor
{
	int halfPatchSize;

	Mat img;
	Mat mask;
	Mat output;

	Mat _img, _mask; // current

	Mat gradientX, gradientY;

	Mat confidence;
	Mat data;

	struct boundaryPixel
	{
		Point2i pt;
		Point2f normal;
	};
	vector<boundaryPixel> boundaryPixels;

	void get_patch(Point2i center, Point2i& uplf, Point2i& btrt)
	{
		uplf.x = max(center.x - halfPatchSize, 0);
		uplf.y = max(center.y - halfPatchSize, 0);
		btrt.x = min(center.x + halfPatchSize, img.cols - 1);
		btrt.y = min(center.y + halfPatchSize, img.rows - 1);
	}

	void compute_gradient()
	{
		Mat grayImg;
		cvtColor(img, grayImg, CV_BGR2GRAY);

		Scharr(grayImg, gradientX, CV_16S, 1, 0);
		convertScaleAbs(gradientX, gradientX);
		gradientX.convertTo(gradientX, CV_32F);

		Scharr(grayImg, gradientY, CV_16S, 0, 1);
		convertScaleAbs(gradientY, gradientY);
		gradientY.convertTo(gradientY, CV_32F);

		for (int i = 0; i < _mask.cols; i++) {
			for (int j = 0; j < _mask.rows; j++) {
				if (_mask.at<uchar>(j, i) == 255) {
					gradientX.at<float>(j, i) = 0;
					gradientY.at<float>(j, i) = 0;
				}
			}
		}

		gradientX /= 255;
		gradientY /= 255;

		//imshow("gradientX", gradientX);
		//imshow("gradientY", gradientY);
	}

	void init()
	{
		_img = img.clone();
		_mask = mask.clone();

		threshold(_mask, _mask, 10, 255, CV_THRESH_BINARY); // (mask > 10) ? 255 : 0
		threshold(_mask, confidence, 2, 1, CV_THRESH_BINARY_INV);
		confidence.convertTo(confidence, CV_32F);

		data.create(img.size(), CV_32F);

		compute_gradient();
	}

	void compute_boundary()
	{
		Mat LAPLACIAN_KERNEL = Mat::ones(3, 3, CV_32F);
		LAPLACIAN_KERNEL.at<float>(1, 1) = -8;

		Mat NORMAL_KERNELX = Mat::zeros(3, 3, CV_32F);
		NORMAL_KERNELX.at<float>(1, 0) = -1;
		NORMAL_KERNELX.at<float>(1, 2) = 1;
		Mat NORMAL_KERNELY;
		transpose(NORMAL_KERNELX, NORMAL_KERNELY);

		Mat srcRegion, tarRegion;

		Mat boundaryMask;
		filter2D(_mask, boundaryMask, CV_32F, LAPLACIAN_KERNEL);
		Mat srcGradientX, srcGradientY;
		filter2D(~_mask, srcGradientX, CV_32F, NORMAL_KERNELX);
		filter2D(~_mask, srcGradientY, CV_32F, NORMAL_KERNELY);

		boundaryPixels.clear();
		for (int i = 0; i < boundaryMask.cols; i++) {
			for (int j = 0; j < boundaryMask.rows; j++) {

				if (boundaryMask.at<float>(j, i) > 0) {
					boundaryPixel newpx;
					newpx.pt = Point2i(i, j);

					newpx.normal.x = srcGradientY.at<float>(j, i);
					newpx.normal.y = -srcGradientX.at<float>(j, i);
					float diffl = sqrt((newpx.normal.x * newpx.normal.x) + (newpx.normal.y * newpx.normal.y));
					if (diffl) {
						newpx.normal.x /= diffl;
						newpx.normal.y /= diffl;
					}
					boundaryPixels.push_back(newpx);
				}
			}
		}

		//cout << boundaryPixels.size() << endl;
		//imshow("boudaryMask", boundaryMask);
		//imshow("srcGradientX", srcGradientX);
		//imshow("srcGradientY", srcGradientY);
	}

	// confidence(p) = sum(confidence(q))/|patch(p)|
	// q are pixels in both patch(p) and source region 
	void compute_confidence()
	{
		for (int n = 0; n < boundaryPixels.size(); n++) {
			Point2i uplf, btrt;
			get_patch(boundaryPixels[n].pt, uplf, btrt);

			float tmpconf = 0;
			for (int i = uplf.x; i <= btrt.x; i++) {
				for (int j = uplf.y; j <= btrt.y; j++) {
					if (_mask.at<uchar>(j, i) == 0) {
						tmpconf += confidence.at<float>(j, i);
					}
				}
			}
			int pxn = (btrt.x - uplf.x + 1) * (btrt.y - uplf.y + 1);
			confidence.at<float>(boundaryPixels[n].pt.y, boundaryPixels[n].pt.x) = tmpconf / pxn;
		}

		//imshow("confidence", confidence);
	}

	// data(p) = gradient(p)*normal(p)
	void compute_data()
	{
		for (int n = 0; n < boundaryPixels.size(); n++) {
			Point2i pt = boundaryPixels[n].pt;
			Point2f normal = boundaryPixels[n].normal;
			data.at<float>(pt.y, pt.x) = fabs(gradientX.at<float>(pt.y, pt.x) * normal.x + gradientY.at<float>(pt.y, pt.x) * normal.y) + .001;
		}
	}

	// priority(p) = confidence(p)*data(p)
	Point2i get_boundaryPixel()
	{
		int max = 0;
		float maxPriority = data.at<float>(boundaryPixels[0].pt.y, boundaryPixels[0].pt.x) * confidence.at<float>(boundaryPixels[0].pt.y, boundaryPixels[0].pt.x);
		for (int n = 1; n < boundaryPixels.size(); n++) {
			float p = data.at<float>(boundaryPixels[n].pt.y, boundaryPixels[n].pt.x) * confidence.at<float>(boundaryPixels[n].pt.y, boundaryPixels[n].pt.x);
			if (p > maxPriority) {
				maxPriority = p;
				max = n;
			}
		}
		return boundaryPixels[max].pt;
	}

	void match_boundaryPixel(Point2i px)
	{
		Point2i uplf, btrt;
		get_patch(px, uplf, btrt);

		int patchw = btrt.x - uplf.x + 1;
		int patchh = btrt.y - uplf.y + 1;

		Point2i bestMatchUpLf;
		float minErr = 999999999;
		for (int i = 0; i <= img.cols - patchw; i++) {
			for (int j = 0; j <= img.rows - patchh; j++) {

				float patchErr = 0;
				bool isValidPatch = true;
				for (int ii = 0; ii < patchw; ii++) {
					for (int jj = 0; jj < patchh; jj++) {
						// check if the matching patch is in the target region of original image
						if (mask.at<uchar>(j + jj, i + ii) == 255) {
							isValidPatch = false;
							break;
						}
						// check if the pixel in the target patch is in the target region of current image
						if (_mask.at<uchar>(uplf.y + jj, uplf.x + ii) == 255) {
							continue;
						}

						for (int c = 0; c < _img.channels(); c++) {
							float diff = _img.at<Vec3b>(j + jj, i + ii)[c] - _img.at<Vec3b>(uplf.y + jj, uplf.x + ii)[c];
							patchErr += diff * diff;
						}
					}
					if (!isValidPatch) {
						break;
					}
				}

				if (!isValidPatch) {
					continue;
				}

				if (patchErr < minErr) {
					minErr = patchErr;
					bestMatchUpLf = Point2i(i, j);
				}
			}
		}

		//cout << bestMatchUpLf << endl;
		for (int i = 0; i < patchw; i++) {
			for (int j = 0; j < patchh; j++) {
				if (_mask.at<uchar>(uplf.y + j, uplf.x + i) == 255) {
					_img.at<Vec3b>(uplf.y + j, uplf.x + i) = _img.at<Vec3b>(bestMatchUpLf.y + j, bestMatchUpLf.x + i);
					_mask.at<uchar>(uplf.y + j, uplf.x + i) = 0;

					gradientX.at<float>(uplf.y + j, uplf.x + i) = gradientX.at<float>(bestMatchUpLf.y + j, bestMatchUpLf.x + i);
					gradientY.at<float>(uplf.y + j, uplf.x + i) = gradientY.at<float>(bestMatchUpLf.y + j, bestMatchUpLf.x + i);

					confidence.at<float>(uplf.y + j, uplf.x + i) = confidence.at<float>(px.y, px.x);
				}
			}
		}
	}

	bool isCompleted()
	{
		for (int i = 0; i < _mask.cols; i++) {
			for (int j = 0; j < _mask.rows; j++) {
				if (_mask.at<uchar>(j, i) == 255) {
					return false;
				}
			}
		}
		return true;
	}

public:

	imageCompletor(Mat inputImage, Mat inputMask, int inputhalfPatchSize = 4)
	{
		img = inputImage.clone();
		mask = inputMask.clone();
		halfPatchSize = inputhalfPatchSize;
	}

	Mat complete()
	{
		init();

		while (!isCompleted()) {
			compute_boundary();
			compute_confidence();
			compute_data();

			Point2i nextBoundaryPixel = get_boundaryPixel();
			//cout << nextBoundaryPixel;
			match_boundaryPixel(nextBoundaryPixel);

			imshow("image", _img);
			imshow("mask", _mask);
			waitKey(1);
		}

		output = _img.clone();
		return output;
	}
};

Mat img, _img;
Mat mask;

Point prevPt(-1, -1);
int thickness = 1;
static void onMouse(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON))
		prevPt = Point2i(-1, -1);
	else if (event == EVENT_LBUTTONDOWN)
		prevPt = Point2i(x, y);
	else if (event == EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON))
	{
		Point2i pt = Point2i(x, y);
		if (prevPt.x < 0) {
			prevPt = pt;
		}
		line(mask, prevPt, pt, Vec3b(255, 255, 255), thickness, 8, 0);
		line(_img, prevPt, pt, Vec3b(255, 255, 255), thickness, 8, 0);
		prevPt = pt;
		imshow("image", _img);
	}
}

int main()
{
	int halfPatchSize = 4;

	string imgfname;
	string maskfname;

	cin >> imgfname >> maskfname;

	img = imread(imgfname, cv::IMREAD_COLOR);
	if (maskfname != "0") {
		mask = imread(maskfname, cv::ImreadModes::IMREAD_GRAYSCALE);

		imageCompletor imgCompletor(img, mask, halfPatchSize);
		Mat output = imgCompletor.complete();
		imwrite("../data/result.jpg", output);

		waitKey(0);
	}
	else {
		_img = img.clone();
		mask = Mat::zeros(img.size(), CV_8U);

		imshow("image", _img);
		setMouseCallback("image", onMouse, 0);
		while (true)
		{
			char instr = waitKey(0);

			if (instr == 'ESC')
				break;

			if (instr == 'r')
			{
				_img = img.clone();
				mask.setTo(0);
				imshow("image", _img);
			}
			if (instr == ' ')
			{
				imageCompletor imgCompletor(_img, mask, halfPatchSize);
				Mat output = imgCompletor.complete();
				imwrite("result.jpg", output);

				_img = output.clone();
				mask.setTo(0);
			}
			if (instr == 'x') {
				if (thickness < 10)
					thickness++;
				cout << endl << "thickness = " << thickness;
			}
			if (instr == 'z') {
				if (thickness > 1)
					thickness--;
				cout << endl << "thickness = " << thickness;
			}
		}
	}
}
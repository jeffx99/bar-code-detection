#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>

using namespace cv;

int main() {

	// Replace with necessary location
	Mat src = imread("C:\\Users\\Jeffery\\Documents\\Visual Studio 2015\\Projects\\bar-code-detection-master\\data\\samplecode25x.png");
	if (src.empty()) {
		std::cerr << "Cannot read image!" << std::endl;
		return -1;
	}
	// namedWindow("source", CV_WINDOW_NORMAL);
	// imshow("source", src);

	// Preprocessing
	Mat pre;
	cvtColor(src, pre, CV_BGR2GRAY);
	//GaussianBlur(pre, pre, Size{ 25, 25 }, 2, 2);

	Mat dst;
	resize(src, dst, Size{ 250,109 });

	// Resize image
	Mat gray;
	resize(pre, gray, Size{ 250,109 });

	// namedWindow("Reduced", CV_WINDOW_NORMAL);
	// imshow("Reduced", gray);

	// Construct array of possible locations for dots.
	Point possible_pts[168];
	// Construct array of alignment points
	Point alignment_pts[6]{
		Point(5+1, 30),
		Point(39+1,30),
		Point(66+1, 30),
		Point(160+1, 30),
		Point(187+1, 30),
		Point(221+1, 30)
	};

	// Iterate through each alignment point, and 
	for (int k = 0; k < 6; ++k) {
		Point marker = alignment_pts[k];
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 7; ++j) {
				possible_pts[j + i*7 + k*28] = marker;
				marker.y += 8;
			}
			marker.y = 30;
			marker.x += 7;
		}
	}

	// The code in the barcode
	char code[25];
	// Null-terminate
	code[24] = 0;
	// Check each possible point for some whiteness (BASIC VERSION)
	for (int i = 0; i < 24; ++i) {
		code[i] = 0;
		for (int j = 0; j < 7; ++j) {
			Point test = possible_pts[j + i * 7];
			int value = (int)gray.at<uchar>(test.y, test.x);
			if (value >= 75) // threshold for whiteness
				++code[i];
			if (j < 6) 
				code[i] <<= 1;
		}
	}

	// Say the message
	std::cout << code << std::endl;

	for (auto const &data : possible_pts) {
		circle(dst, data, 1, Scalar(0, 0, 255), -1, 8, 0);
	}

	namedWindow("final", CV_WINDOW_NORMAL);
	imshow("final", dst);

	waitKey(0);
	return 0;
}

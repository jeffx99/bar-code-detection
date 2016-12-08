#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>

using namespace cv;

int main() {

	Mat src = imread("C:\\Users\\mpatel\\Documents\\Visual Studio 2015\\Projects\\OpenCV\\data\\samplecode25x.png");
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
		Point(5, 30)/*,
		Point(39,30),
		Point(66, 30),
		Point(160, 30),
		Point(187, 30),
		Point(221, 30)*/
	};

	// Iterate through each alignment point, and 
	for (auto alignment_pt : alignment_pts) {
		int k = 0;
		Point marker(5, 30);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 7; ++j) {
				possible_pts[j + i*7 + k*24] = marker;
				marker.y += 7;
			}
			marker.y = 30;
			marker.x += 7;
		}
		++k;
	}

	for (auto const &data : possible_pts) {
		circle(dst, data, 0.5, Scalar(0, 255, 0), 3, 8, 0);
	}

	namedWindow("final", CV_WINDOW_NORMAL);
	imshow("final", dst);

	waitKey(0);
	return 0;
}
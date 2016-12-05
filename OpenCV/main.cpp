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
	namedWindow("source", CV_WINDOW_NORMAL);
	imshow("source", src);

	waitKey(0);
	return 0;
}
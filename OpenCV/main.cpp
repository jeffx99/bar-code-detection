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

	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(25, 25), 2, 2);

	std::vector<Vec3f> circles;
	HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 30, 200, 35, 0, 100);
	for (auto const &data : circles) {
		Point center(cvRound(data[0]), cvRound(data[1]));
		int radius = cvRound(data[2]);
		circle(src, center, radius, Scalar(0, 255, 0), 3, 8, 0);
	}

	for (auto const &circle : circles) {
		std::cout << "Center: " << Point{cvRound(circle[0]), cvRound(circle[1])} << "\tRadius: " << cvRound(circle[2]) << std::endl;
	}

	namedWindow("Hough Circles", CV_WINDOW_NORMAL);
	imshow("Hough Circles", src);

	waitKey(0);
	return 0;
}
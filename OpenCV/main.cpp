#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>

using namespace cv;

int roundToTens(int x) {
	if (x % 10)
		x += 10 - x % 10;
	return x;
}

int main() {

	Mat src = imread("C:\\Users\\mpatel\\Documents\\Visual Studio 2015\\Projects\\OpenCV\\data\\samplecode25x.png");
	if (src.empty()) {
		std::cerr << "Cannot read image!" << std::endl;
		return -1;
	}
	// namedWindow("source", CV_WINDOW_NORMAL);
	// imshow("source", src);

	// Preproccessing
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(25, 25), 2, 2);

	// Find the circles in the picture
	std::vector<Vec3f> circles;
	HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 30, 200, 35, 0, 100);
	for (auto const &data : circles) {
		Point center(cvRound(data[0]), cvRound(data[1]));
		int radius = cvRound(data[2]);
	}

	// Extract the center points
	std::vector<Point> centers;
	for (auto const &circle : circles) {
		centers.push_back(Point{ cvRound(circle[0]), cvRound(circle[1]) });
	}

	// Sort the center points based on their X-coordinate
	std::sort(centers.begin(), centers.end(),
		[](const Point &a, const Point &b) {
		return a.x < b.x;
	});
	
	// Round the center points
	for (auto &center : centers) {
		center.x = roundToTens(center.x);
		center.y = roundToTens(center.y);
	}

	for (size_t i = 1; i < centers.size(); i++) {
		std::cout << i << "\tCenter: " << centers[i] << "\tDifference X: " << centers[i].x - centers[i - 1].x << std::endl;

	}

	// Draw the circles on the rounded position
	for (auto const &data : centers) {
		circle(src, data, 10, Scalar(0, 0, 255), 3, 8, 0);
	}

	namedWindow("Hough Circles", CV_WINDOW_NORMAL);
	imshow("Hough Circles", src);

	waitKey(0);
	return 0;
}
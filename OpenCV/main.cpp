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

	Mat binary;
	cvtColor(src, binary, CV_BGR2GRAY);
	GaussianBlur(binary, binary, Size{ 3, 3 }, 2, 2);
	
	Mat dst, dst_norm, dst_norm_scaled;
	dst = Mat::zeros(src.size(), CV_32FC1);

	cornerHarris(binary, dst, 7, 5, 0.05, BORDER_DEFAULT);

	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);

	std::vector<Point> corners;

	for (int j = 0; j < dst_norm.rows; j++) {
		for (int i = 0; i < dst_norm.cols; i++) {
			if ((int)dst_norm.at<float>(j, i) > 75) {
				// circle(dst_norm_scaled, Point(i, j), 25, Scalar(0), 2, 8, 0);
				corners.push_back(Point(i, j));
				std::cout << "Point: ( " << std::to_string(j) << " , " << std::to_string(i) << " )" << std::endl;
			}
		}
	}

	std::cout << std::to_string(corners.size()) << std::endl;

	namedWindow("Harris Edge", CV_WINDOW_NORMAL);
	imshow("Harris Edge", dst_norm_scaled);

	Mat threshold_output;
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;

	threshold(dst_norm_scaled, threshold_output, 100, 255, THRESH_BINARY);
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	std::vector<std::vector<Point> > hull(contours.size());
	for (size_t i = 0; i < contours.size(); i++) {
		convexHull(Mat(contours[i]), hull[i], false);
	}

	RNG rng(12345);
	Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++) {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, (int)i, color, 1, 8, std::vector<Vec4i>(), 0, Point());
		drawContours(drawing, hull, (int)i, color, 1, 8, std::vector<Vec4i>(), 0, Point());
	}

	namedWindow("convex hull", CV_WINDOW_NORMAL);
	imshow("convex hull", drawing);


	waitKey(0);
	return 0;
}
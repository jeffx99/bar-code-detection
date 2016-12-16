#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>

using namespace cv;

int main() {

	// Read original photo -- source may vary
	Mat src = imread("C:\\Users\\Jeffery\\Documents\\Visual Studio 2015\\Projects\\bar-code-detection-master\\data\\data_10.png");
	if (src.empty()) {
		std::cerr << "Cannot read image!" << std::endl;
		return -1;
	}

	// Prepare and execute inverse threshold
	Mat thr;
	cvtColor(src, thr, CV_BGR2GRAY);
	threshold(thr, thr, 20, 255, CV_THRESH_BINARY_INV);

	namedWindow("threshold", CV_WINDOW_NORMAL);
	imshow("threshold", thr);

	// Find the contours
	std::vector<std::vector<Point>> contours; // Vector for storing contour
	std::vector<Vec4i> hierarchy;
	int largest_contour_index = 0;
	int largest_area = 0;

	Mat mid(src.rows, src.cols, CV_8UC1, Scalar::all(0)); //create image to have intermediate version
	findContours(thr.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
	for (int i = 0; i< contours.size(); i++) {
		double a = contourArea(contours[i], false);  //  Find the area of contour
		if (a>largest_area) {
			largest_area = a;
			largest_contour_index = i;                //Store the index of largest contour
		}
	}

	// draws largest contour found into intermediate
	drawContours(mid, contours, largest_contour_index, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	namedWindow("contour", CV_WINDOW_NORMAL);
	imshow("contour", mid);

	std::vector<std::vector<Point>> contours_poly(1);
	approxPolyDP(Mat(contours[largest_contour_index]), contours_poly[0], 5, true); // get polygon with ordered verticies
	Rect boundRect = boundingRect(contours[largest_contour_index]); // get bounding box with ordered verticies

	// Draw rectangle to map to a colored version of intermediate
	Mat c_mid;
	cvtColor(mid, c_mid, CV_GRAY2BGR);
	rectangle(c_mid, boundRect, Scalar(0, 255, 0), 2, 8, 0);

	// draws bounding rectangle into intermediate
	namedWindow("contour + bounding rect", CV_WINDOW_NORMAL);
	imshow("contour + bounding rect", c_mid);

	// cropped image
	Mat cropped;



	// AUTOMATICALLY align the verticies in order for perspective warp - current algo only works if not taken at very extreme angles
	// We allow contours with 5 vertices incase the extra vertex is creates a degenerate pentagon, i.e, a quadrilateral
	if (contours_poly[0].size() == 4 || contours_poly[0].size() == 5) {
		std::cout << "HANDLING CONTOUR WITH " << contours_poly[0].size() << " VERTICES" << std::endl;

		std::cout << "CONTOUR POLYGON VERTICES IN THE ORDER:" << std::endl;
		for (Point2f point : contours_poly[0])
			std::cout << point << std::endl;

		std::vector<Point2f> quad_pts;
		std::vector<Point2f> squre_pts;
		// Order the vertices

		// Find the two points with the smallest x coords
		Point2f sxcs[2] = { contours_poly[0][0] , contours_poly[0][1] };
		for (Point2f point : contours_poly[0]) {
			if (point.x < sxcs[0].x && point != sxcs[1]) {
				if (sxcs[0].x >= sxcs[1].x)
					sxcs[0] = point;
				else
					sxcs[1] = point;

			}
			else if (point.x < sxcs[1].x && point != sxcs[0]) {
				if (sxcs[0].x >= sxcs[1].x)
					sxcs[0] = point;
				else
					sxcs[1] = point;
			}
		}

		// Order sxcs by increasing y coord
		if (sxcs[0].y > sxcs[1].y) {
			Point2f temp = sxcs[0];
			sxcs[0] = sxcs[1];
			sxcs[1] = temp;
		}

		// Find the two points with the largest x coords
		Point2f lxcs[2] = { contours_poly[0][0] , contours_poly[0][1] };
		for (Point2f point : contours_poly[0]) {
			if (point.x > lxcs[0].x && point != lxcs[1]) {
				if (lxcs[0].x <= lxcs[1].x)
					lxcs[0] = point;
				else
					lxcs[1] = point;

			} else if (point.x > lxcs[1].x && point != lxcs[0]) {
				if (lxcs[0].x <= lxcs[1].x)
					lxcs[0] = point;
				else
					lxcs[1] = point;
			}
		}

		// Order lxcs by increasing y coord
		if (lxcs[0].y > lxcs[1].y) {
			Point2f temp = lxcs[0];
			lxcs[0] = lxcs[1];
			lxcs[1] = temp;
		}


		quad_pts.push_back(sxcs[0]);
		quad_pts.push_back(sxcs[1]);
		quad_pts.push_back(lxcs[0]);
		quad_pts.push_back(lxcs[1]);

		squre_pts.push_back(Point2f(boundRect.x, boundRect.y));
		squre_pts.push_back(Point2f(boundRect.x, boundRect.y + boundRect.height));
		squre_pts.push_back(Point2f(boundRect.x + boundRect.width, boundRect.y));
		squre_pts.push_back(Point2f(boundRect.x + boundRect.width, boundRect.y + boundRect.height));

		std::cout << "\nVERTICES ARRANGED IN THE ORDER: " << std::endl;
		for (Point2f point : quad_pts)
			std::cout << point << std::endl;


		Mat transmtx = getPerspectiveTransform(quad_pts, squre_pts); // Do the actual perspective warp
		if (transmtx.empty()) {
			std::cout << "TRANSFORMATION FAILED!" << std::endl;
		}
		Mat transformed = Mat::zeros(src.rows, src.cols, CV_8UC3);
		warpPerspective(src, transformed, transmtx, src.size());


		// Shows transformed source
		rectangle(src, boundRect, Scalar(0, 255, 0), 2, 8, 0);
		namedWindow("source", CV_WINDOW_NORMAL);
		imshow("source", src);

		rectangle(transformed, boundRect, Scalar(0, 255, 0), 2, 8, 0);
		namedWindow("perspective transformed", CV_WINDOW_NORMAL);
		imshow("perspective transformed", transformed);



		Mat croppedRef(transformed, boundRect);
		croppedRef.copyTo(cropped);
	} else {
		std::cout << "CONTOUR WITH INADMISSIBLE AMOUNT OF VERTICES" << std::endl;
		std::cout << contours_poly[0].size() << " VERTICES FOUND" << std::endl;
	}

	if (cropped.empty()) {
		std::cout << "FAILURE TO CROP PERSPECTIVE TRANSFORM" << std::endl;
		waitKey(0);
		return -1;
	}

	// Shows cropped source
	namedWindow("perspective transformed cropped", CV_WINDOW_NORMAL);
	imshow("perspective transformed cropped", cropped);

	// Preprocessing to grayscale
	Mat pre;
	cvtColor(cropped, pre, CV_BGR2GRAY);
	//GaussianBlur(pre, pre, Size{ 25, 25 }, 2, 2);

	// Resize image
	Mat reduced;
	resize(cropped, reduced, Size{ 250,109 });

	// Resize image -- grayscale
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
			if (value >= 75) { // threshold for whiteness
				++code[i];
				circle(reduced, test, 1, Scalar(0, 0, 255), -1, 8, 0);
			}
			if (j < 6) 
				code[i] <<= 1;
		}
	}

	std::cout << "\nBARCODE DECODED:" << std::endl;
	// Say the message
	std::cout << code << std::endl;


	namedWindow("final", CV_WINDOW_NORMAL);
	imshow("final", reduced);

	waitKey(0);
	return 0;
}

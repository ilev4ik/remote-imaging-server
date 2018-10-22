#include "imaging.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

namespace {

	void show_mat(const cv::Mat& m) {
		std::cout << m << std::endl << std::endl;
	}

}

void test()
{
	cv::Mat M(2, 2, CV_8UC(2), cv::Scalar(12, 4, 255));
	::show_mat(M);
}
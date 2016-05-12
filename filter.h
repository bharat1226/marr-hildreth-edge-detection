#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <boost/filesystem.hpp>
#include "boost/filesystem/fstream.hpp"
#include <string>
#include <vector>

using namespace boost::filesystem;
using namespace cv;
using namespace std;

float lowPassMaskSum, highPassMaskSum;
Mat realGaussianMask;
Mat realLaplacianMask;
bool passFilter;

bool createMask(const vector<vector<string>>& mask, Mat& realMask, float& sum, float& maskSum);
void readFilterText(vector<vector<string>>& mask, const string filterPath);
void inputFilterPath(path filterTextPath);
void zeroCrossingPoint(Mat& edgeMap, float var1, float var2, int i, int j, int k, int l);

class MarrHildrethEdgeDetection {

public:
	MarrHildrethEdgeDetection() {
	};

	Mat computeFilter(const Mat& image, Mat& filterImage) {

		int i = 0;
		bool pFilter = true;

		while (i < 9) {
			computeNextFilter(image, realGaussianMask, filterImage, pFilter);
			filterImage.copyTo(image);
			i += 1;
		}

		pFilter = false;
		computeNextFilter(image, realLaplacianMask, filterImage, pFilter);

		Mat edgeMap = Mat::zeros(Size(image.cols, image.rows), CV_8UC1);

		// ---------------------------------------edgeMap calculation ---------------------------------------------

		for (int i = 1; i < image.rows - 1; i++) {
			for (int j = 1; j < image.cols - 1; j++) {

				float var1 = filterImage.at<float>(i, j);
				float var21 = filterImage.at<float>(i, j + 1);
				float var22 = filterImage.at<float>(i + 1, j + 1);
				float var23 = filterImage.at<float>(i + 1, j);
				float var24 = filterImage.at<float>(i - 1, j + 1);
				// ----------------------------------------- var21 ------------------------------------------

				zeroCrossingPoint(edgeMap, var1, var21, i, j, i, j + 1);

				//------------------------------------------var22---------------------------------------------

				zeroCrossingPoint(edgeMap, var1, var22, i, j, (i + 1), (j + 1));

				// ----------------------------------------------var23----------------------------

				zeroCrossingPoint(edgeMap, var1, var23, i, j, (i + 1), j);

				// ----------------------------------------------var24------------------------------------

				zeroCrossingPoint(edgeMap, var1, var24, i, j, (i - 1), (j + 1));
			}
		}
		return edgeMap;
	};

	void computeNextFilter(const Mat& image, const Mat& realMask, Mat& filterImage, bool pFilter) {

		int a = (realMask.rows - 1) / 2;
		int b = (realMask.cols - 1) / 2;

		for (int i = 0; i < image.rows; i++) {
			for (int j = 0; j < image.cols; j++) {

				double eachPixelValue = 0.0;

				for (int s = -a; s <= a; s++) {
					for (int t = -b; t <= b; t++) {

						if (i - s >= 0 && j - t >= 0 && i - s < image.rows && j - t < image.cols) {
							eachPixelValue += realMask.at<float>(s + a, t + b) * image.at<float>(i - s, j - t);
						}
						else {
							eachPixelValue += realMask.at<float>(s + a, t + b) * 128.0;
						}
					}
				}

				float computedPixelValue = 0;
				if (pFilter == true) {
					computedPixelValue = ((float)eachPixelValue / lowPassMaskSum);
				}
				else {
					computedPixelValue = (float)eachPixelValue;
				}
				filterImage.at<float>(i, j) = computedPixelValue;
			}
		}
	};
};

void inputFilterPath(path filterTextPath) {
	string filterPath = filterTextPath.string();
	string filterPath2 = filterTextPath.string();
	string gaussianPath = filterPath.append("/gaussian3x3.txt");
	string laplacianPath = filterPath2.append("/laplacian.txt");
	vector<vector<string>> gaussianMask;
	vector<vector<string>> laplacianMask;

	readFilterText(gaussianMask, gaussianPath);
	readFilterText(laplacianMask, laplacianPath);

	realGaussianMask = Mat::zeros(Size(stoi(gaussianMask[0][1]), stoi(gaussianMask[0][0])), CV_32FC1);
	realLaplacianMask = Mat::zeros(Size(stoi(laplacianMask[0][1]), stoi(laplacianMask[0][0])), CV_32FC1);

	lowPassMaskSum = 0.0;
	highPassMaskSum = 0.0;

	bool gaussFilter = createMask(gaussianMask, realGaussianMask, lowPassMaskSum, highPassMaskSum);
	bool laplacianFilter = createMask(laplacianMask, realLaplacianMask, lowPassMaskSum, highPassMaskSum);

	passFilter = gaussFilter;
}

bool createMask(const vector<vector<string>>& mask, Mat& realMask, float& lowPassMaskSum, float& highPassMaskSum) {

	bool passFilter;

	if (mask[1][0] == "L") { passFilter = true; }
	else { passFilter = false; }

	for (int i = 0; i < stoi(mask[0][0]); i++) {
		for (int j = 0; j < stoi(mask[0][1]); j++) {
			realMask.at<float>(i, j) = (float)(stoi(mask[2 + i][j]));
			lowPassMaskSum += realMask.at<float>(i, j);
			if (realMask.at<float>(i, j) > 0) {
				highPassMaskSum += realMask.at<float>(i, j);
			}
		}
	}

	return passFilter;
}

void readFilterText(vector<vector<string>>& mask, const string filterPath) {
	boost::filesystem::fstream filter;
	filter.open(filterPath);
	string line;

	while (getline(filter, line)) {
		istringstream is(line);
		mask.push_back(
			vector<string>(istream_iterator<string>(is),
				istream_iterator<string>()));
	}
	filter.close();
}

void zeroCrossingPoint(Mat& edgeMap, float var1, float var2, int i, int j, int k, int l) {
	if (fabs(var1 - var2) >= 5.0) {
		if ((var1 > 0.0 && var2 < 0.0) || (var1 < 0.0 && var2 > 0.0)) {
			if (var1 < var2) {
				edgeMap.at<uchar>(i, j) = 255;
			}
			else {
				edgeMap.at<uchar>(k, l) = 255;
			}
		}
	}
}



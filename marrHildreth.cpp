#include "filter.h"

int main(int argc, char **argv) {

	path imagesFolderPath(argv[1]);
	path filterTextPath(argv[2]);
	path outputFolderPath(argv[3]);

	inputFilterPath(filterTextPath);

	directory_iterator end_itr;
	directory_iterator itr(imagesFolderPath);

	for (directory_iterator itr(imagesFolderPath); itr != end_itr; ++itr) {

		string bmpCheck = itr->path().extension().string();

		if (bmpCheck == ".bmp" || bmpCheck == ".BMP") {

			Mat image = imread(itr->path().string(), IMREAD_GRAYSCALE);
			Mat image1 = Mat::zeros(Size(image.cols, image.rows), CV_32FC1);
			Mat edgeMap;

			image.convertTo(image1, CV_32FC1);
			Mat filterImage = Mat::zeros(Size(image.cols, image.rows), CV_32FC1);

			MarrHildrethEdgeDetection filter;
			filter.computeFilter(image1, filterImage).copyTo(edgeMap);

			string outputFile = outputFolderPath.string();
			outputFile.append("/");
			outputFile.append(itr->path().filename().string());

			imwrite(outputFile, edgeMap);
		}
	}
	return 0;
}



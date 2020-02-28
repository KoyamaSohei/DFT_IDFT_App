#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

void help(char* progName) {
	cout << "Usage:" << endl
		 << progName << " [image_path]" << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		help(argv[0]);
		return 1;
	}
	const string filename = argv[1];
	Mat src = imread(filename, IMREAD_GRAYSCALE);
	if (src.empty()) {
		cout << "Error when opening image: " << filename << endl;
		return 1;
	}
	imshow("original image", src);
	waitKey(0);
	return 0;
}

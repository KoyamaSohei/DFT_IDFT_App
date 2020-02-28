#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

const int minW = 200;
const int minH = 20;

Mat src;
Mat cropped;
Rect roi;
int px, py, w, h;

void help(char* progName) {
	cout << "Usage:" << endl
		 << progName << " [image_path]" << endl;
}

void mouseHandler(int event, int x, int y, int flags, void* param) {
	switch (event) {
	case CV_EVENT_MOUSEMOVE:
		if (flags & CV_EVENT_FLAG_LBUTTON) {
			w = min(max(x - px, minW),max(src.cols-px, minW));
			h = min(max(y - py, minH),max(src.rows-py, minH));
		}
		break;
	case CV_EVENT_LBUTTONDOWN:
		px = x;
		py = y;
		w = minW;
		h = minH;
		break;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		help(argv[0]);
		return 1;
	}
	const string filename = argv[1];
	src = imread(filename, IMREAD_GRAYSCALE);
	if (src.empty()) {
		cout << "Error when opening image: " << filename << endl;
		return 1;
	}
	string origin_win = "original image";
	imshow(origin_win, src);
	px = 0;
	py = 0;
	w = src.cols;
	h = src.rows;
	cvSetMouseCallback(origin_win.c_str(), mouseHandler);
	roi = Rect(px, py, w, h);
	cropped = Mat(src, roi);
	while (1) {
		waitKey(10);
		roi.x = px;
		roi.y = py;
		roi.width = w;
		roi.height = h;
		cout << roi << endl;
		cropped = Mat(src, roi);
		imshow("cropped", cropped);
	}
	return 0;
}

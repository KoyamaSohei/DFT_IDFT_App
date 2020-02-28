#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

const int minW = 200;
const int minH = 20;

Mat src;
Mat cropped;
Mat padded;
Mat mag;
Rect roi;
int px, py, w, h;

void help(char* progName) {
	cout << "Usage:" << endl
		 << progName << " [image_path]" << endl;
}

void shiftDFT(Mat& src, Mat& dst) {
	Mat tmp;
	int cx = src.cols / 2;
	int cy = src.rows / 2;

	for (int i = 0; i <= cx; i += cx) {
		Mat qs(src, Rect(i ^ cx, 0, cx, cy));
		Mat qd(dst, Rect(i, cy, cx, cy));
		qs.copyTo(tmp);
		qd.copyTo(qs);
		tmp.copyTo(qd);
	}
}

void genMag() {
	int m = getOptimalDFTSize(src.rows);
	int n = getOptimalDFTSize(src.cols);
	copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));
	Mat Re = Mat(src.size(), CV_64F);
	src.convertTo(Re, CV_64F);
	Mat Im = Mat::zeros(src.size(), CV_64F);
	vector<Mat> ms;
	ms.push_back(Re);
	ms.push_back(Im);
	Mat complex = Mat(src.size(), CV_64FC2);
	merge(ms, complex);
	Mat dst;
	dft(complex, dst);
	split(dst, ms);
	magnitude(ms[0], ms[1], ms[0]);
	log(ms[0] + 1, ms[0]);
	normalize(ms[0], mag, 0, 1, CV_MINMAX);
	shiftDFT(mag, mag);
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
	genMag();
	string mag_win = "magnitude image";
	imshow(mag_win, mag);
	px = 0;
	py = 0;
	w = src.cols;
	h = src.rows;
	cvSetMouseCallback(mag_win.c_str(), mouseHandler);
	roi = Rect(px, py, w, h);
	while (1) {
		waitKey(10);
		roi.x = px;
		roi.y = py;
		roi.width = w;
		roi.height = h;
		cropped = Mat(mag, roi);
		imshow("cropped", cropped);
	}
	return 0;
}

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

const int minW = 1;
const int minH = 1;

Mat src;
Mat cropped;
Mat mag;
Mat revmag;
Mat dst;
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
	Mat Re = Mat(src.size(), CV_64F);
	src.convertTo(Re, CV_64F);
	Mat Im = Mat::zeros(src.size(), CV_64F);
	vector<Mat> ms;
	ms.push_back(Re);
	ms.push_back(Im);
	Mat complex = Mat(src.size(), CV_64FC2);
	merge(ms, complex);
	dft(complex, dst);
	split(dst, ms);
	magnitude(ms[0], ms[1], ms[0]);
	log(ms[0] + 1, ms[0]);
	normalize(ms[0], mag, 0, 1, CV_MINMAX);
	shiftDFT(dst, dst);
	shiftDFT(mag, mag);
}

void geniMag() {
	double min, max;
	Mat t = Mat(src.size(),CV_64FC2);
	t = Scalar(0);
	dst(roi).copyTo(t(roi));

	Mat s;
	vector<Mat> ms;
	idft(t, s);
	split(s, ms);
	minMaxLoc(ms[0], &min, &max);
	revmag = Mat(ms[0] * 1.0 / max, Rect(0, 0, src.cols, src.rows));
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
	px = 0;
	py = 0;
	w = src.cols;
	h = src.rows;
	roi = Rect(px, py, w, h);
	Mat red(src.rows,src.cols,CV_32FC3);
	red = Scalar(0, 0, 1);
	Mat mag32;
	mag.convertTo(mag32, CV_32F);
	Mat d(src.rows,src.cols,CV_32F);
	cvtColor(mag32, d, COLOR_GRAY2BGR);
	Mat magdst;
	d.copyTo(magdst);
	addWeighted(red(roi), 0.3, d(roi), 0.7, 0, magdst(roi));
	imshow(mag_win, magdst);
	cvSetMouseCallback(mag_win.c_str(), mouseHandler);
	while (1) {
		waitKey(10);
		if (roi.x != px || roi.y != py || roi.width != w || roi.height != h) {
			roi.x = px;
			roi.y = py;
			roi.width = w;
			roi.height = h;
			geniMag();
			d.copyTo(magdst);
			addWeighted(red(roi), 0.3, d(roi), 0.7, 0, magdst(roi));
			imshow(mag_win, magdst);
			imshow("cropped", revmag);
		}
		
	}
	return 0;
}

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "DFT_IDFT.hpp"
using namespace std;
using namespace cv;

DFT_IDFT::DFT_IDFT(Mat src)
	:
	src(src),
	px(0),
	py(0),
	w(src.cols),
	h(src.rows),
	roi(px, py, w, h),
	red(src.rows, src.cols, CV_32FC3, Scalar(0., 0., 1.)) {};



void DFT_IDFT::shiftDFT(Mat& s, Mat& d) {
	Mat tmp;
	int cx = s.cols / 2;
	int cy = s.rows / 2;

	for (int i = 0; i <= cx; i += cx) {
		Mat qs(s, Rect(i ^ cx, 0, cx, cy));
		Mat qd(d, Rect(i, cy, cx, cy));
		qs.copyTo(tmp);
		qd.copyTo(qs);
		tmp.copyTo(qd);
	}
}

bool DFT_IDFT::isROIchanged() {
	return roi.x != px || roi.y != py || roi.width != w || roi.height != h;
}

void DFT_IDFT::updateROI() {
	roi.x = px;
	roi.y = py;
	roi.width = w;
	roi.height = h;
}

void DFT_IDFT::handleROI(int event, int x, int y, int flags, void *param) {
	DFT_IDFT *app = (DFT_IDFT*)param;
	switch (event) {
	case CV_EVENT_MOUSEMOVE:
		if (flags & CV_EVENT_FLAG_LBUTTON) {
			app->w = min(max(x - app->px, app->MIN_W), max(app->src.cols - app->px, app->MIN_W));
			app->h = min(max(y - app->py, app->MIN_H), max(app->src.rows - app->py, app->MIN_H));
		}
		break;
	case CV_EVENT_LBUTTONDOWN:
		app->px = x;
		app->py = y;
		app->w = app->MIN_W;
		app->h = app->MIN_H;
		break;
	}
}

void DFT_IDFT::renderMag() {
	Mat Re = Mat(src.size(), CV_32F);
	Mat Im = Mat::zeros(src.size(), CV_32F);
	src.convertTo(Re, CV_32F);
	vector<Mat> ms;
	ms.push_back(Re);
	ms.push_back(Im);
	Mat cmp = Mat(src.size(), CV_32FC2);
	merge(ms, cmp);
	dft(cmp, dst);
	split(dst, ms);
	magnitude(ms[0], ms[1], ms[0]);
	log(ms[0] + 1, ms[0]);
	normalize(ms[0], mag, 0, 1, CV_MINMAX);
	shiftDFT(mag, mag);
	shiftDFT(dst, dst);
	cvtColor(mag, magBGR, COLOR_GRAY2BGR);
}

void DFT_IDFT::renderMagWithROI(Mat& dst) {
	magBGR.copyTo(dst);
	addWeighted(red(roi), RedRetio, dst(roi), 1. - RedRetio, 0, dst(roi));
}

void DFT_IDFT::renderIMag(Mat &imag) {
	double min, max;
	Mat t = Mat(src.size(), CV_32FC2,Scalar(0));
	dst(roi).copyTo(t(roi));
	Mat s;
	vector<Mat> ms;
	idft(t, s);
	split(s, ms);
	minMaxLoc(ms[0], &min, &max);
	imag = Mat(ms[0] * 1.0 / max, Rect(0, 0, src.cols, src.rows));
}

void DFT_IDFT::run() {
	renderMag();
	Mat m, im;
	renderMagWithROI(m);
	imshow(MAG_WINDOW, m);
	cvSetMouseCallback(MAG_WINDOW.c_str(), handleROI,this);
	while (1) {
		waitKey(10);
		if (isROIchanged()) {
			updateROI();
			renderMagWithROI(m);
			renderIMag(im);
			imshow(MAG_WINDOW, m);
			imshow(IMAG_WINDOW, im);
		}
	}
}
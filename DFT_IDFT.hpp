#pragma once
#include <iostream>
#include <opencv2/core.hpp>
using namespace std;
using namespace cv;

class DFT_IDFT
{
private:
	Mat src,mag,imag,magBGR,dst,red;
	Rect roi;
	int px, py, w, h;
	const int MIN_H = 1, MIN_W = 1;
	const double RedRetio = 0.3;
	const string MAG_WINDOW = "Magnitude image";
	const string IMAG_WINDOW = "Inverted image";
	void shiftDFT(Mat& src, Mat& dst);
	bool isROIchanged();
	void updateROI();
	static void handleROI(int event, int x, int y, int flags, void *param);
	void renderMag();
	void renderMagWithROI(Mat &dst);
	void renderIMag();
	
public:
	DFT_IDFT(Mat src);
	void run();
};
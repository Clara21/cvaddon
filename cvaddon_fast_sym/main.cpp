// Testing Fast Symmetry Code (version 2)

#include <cv.h>
#include <highgui.h>

#include "cvaddon_fast_sym_detect.h"

#include "cvaddon_draw.h"
#include "cvaddon_display.h"

#include "windows_fast_timer.h"

const int NUM_DRAW_PEAKS = 3;
const double CANNY_TH[2] = {30.0, 90.0};
const int TRIALS = 100;

#include <iostream>
using std::cerr;
using std::endl;

int main()
{
//	IplImage *in = cvLoadImage("real_img1.bmp");
//	IplImage *in = cvLoadImage("perception_vase.png");
//	IplImage *in = cvLoadImage("test_sym_90deg_y_shifted_down.PNG");
//	IplImage *in = cvLoadImage("test_sym_90_cw13_5bdeg.png");
	IplImage *in = cvLoadImage("F:/_WORK/_PhD/code_and_data/symmetry/images/pendulum_improved/edge_noise_back_new_50fps/default013.bmp");	

	IplImage *inGray = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);
	IplImage *inEdge = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);

	float rDivs = sqrtf(in->width*in->width + in->height*in->height) + 1;
	int thDivs = 180;	
	CvAddonFastSymDetector symDetector(cvGetSize(in), cvSize(rDivs, thDivs));

	cvCvtColor(in, inGray, CV_BGR2GRAY);
	cvCanny(inGray, inEdge, CANNY_TH[0], CANNY_TH[1]);

	// Clearing border of edge image
	cvAddonFillBorder(inEdge, 10, CV_RGB(0,0,0));

//	cvSaveImage("test_edge.png", inEdge);
//	exit(1);

	inEdge = cvLoadImage("test_edge.png", 0);

	// Testing r accuracy
//	inEdge = cvLoadImage("test_sym_r_neg1.png", 0);
//	inEdge = cvLoadImage("test_sym_r1.png", 0);
//	inEdge = cvLoadImage("test_sym_r0.png", 0);
//	inEdge = cvLoadImage("test_sym_r_neg_300.PNG", 0);
//	inEdge = cvLoadImage("test_sym_r_300.PNG", 0);
	
	cvAddonShowImageOnce(inEdge, "Edge Image");

#ifndef _DEBUG
	FastTimer t0;
	int i;
	t0.getLoopTime();
	for(i = 0 ; i < TRIALS; ++i) {
#endif
		symDetector.vote(inEdge, 25, 250, true, -25, 25);
#ifndef _DEBUG
	}
	cerr << "Voting Took: " << t0.getLoopTime() / (float)TRIALS << endl;
#endif

	cvAddonShowImageOnce(symDetector.H);
//	cvAddonShowImageOnce(symDetector.HMask);

	CvAddonFastSymResults symResults(25);
	
#ifndef _DEBUG
	t0.getLoopTime();
	for(i = 0 ; i < TRIALS; ++i) {
#endif	
	symDetector.getResult(1, symResults, 10, 10, false, true, -25, 25);
#ifndef _DEBUG
	}
	cerr << "Peak Find Took: " << t0.getLoopTime() / (float)TRIALS << endl;
#endif

	cvAddonShowImageOnce(symDetector.HBackUp, "Hough Backup");
	cvAddonShowImageOnce(symDetector.HMask, "Hough Mask after finding Peaks");

	for(int p = 0; p < symResults.numSym; ++p) 
	{
		cerr << "*****   " << p << "   ****" << endl;

		float r = symResults.symLines[p].r;
		float theta = symResults.symLines[p].theta;
		cerr << "Refined: " << r << "," << theta * 180.0f / CV_PI << endl;
		float rr = symDetector.getPixelFromIndex(symResults.symLines[p].rIndexRaw);
		float tt = symDetector.getRadiansFromIndex(symResults.symLines[p].thetaIndexRaw);
		cerr << "Raw: " << rr << "," << tt * 180.0f / CV_PI << endl;

		cvAddonDrawPolarLine(in, r, theta, CV_RGB(0,255,0), 1);
		cvAddonDrawPolarLine(in, rr, tt, CV_RGB(0,0,255), 1);
	}

	cvAddonShowImageOnce(in, "Detected Symmetries");

	return 0;
}
#ifndef _CVADDON_HSV_FILTER_H
#define _CVADDON_HSV_FILTER_H

////////////////////////////////////////////////////////////
//                 CvAddon HSV Colour Filter
////////////////////////////////////////////////////////////
// By Wai Ho Li
////////////////////////////////////////////////////////////
// Usage Notes
// 
// User responsible for creating and keeping track of 
// IplImage for src image, hue, saturation and destination
// images. The class will handle the histogram, its accumulation
// and blending, as well as backprojection.
//
// Supports ROI (assumes all ROI are equal in size AND location
// for a given function)
////////////////////////////////////////////////////////////

// Macro for easy definition of HSV range limits
#define CV_HSV( h, s, v )  cvScalar( (h), (s), (v) )

#include <cv.h>

#ifdef _DEBUG
	#include <iostream>
	using std::cerr;
	using std::endl;

	#include "cvaddon_display.h"
#endif

#include "cvaddon_file_io.h"

// HSV colour filtering
class CvAddonHSVFilter
{
public:
	// Normal Constructor
	CvAddonHSVFilter(const int& hBins = 45, const int& sBins = 8);

//	// Use this constructor to load histogram data from 
//	// file (generated in a previous buildHist() call)
//	CvAddonHSVFilter(const char* filename);

	~CvAddonHSVFilter();

	// ************ MAIN FUNCTIONS ************
	void buildHist(const IplImage *src, IplImage *H, IplImage *S, IplImage *V
		, const CvScalar &thresh0, const CvScalar &thresh1
		, const IplImage *mask = NULL);

	void blendHist(const IplImage *src, IplImage *H, IplImage *S, IplImage *V
		, const CvScalar &thresh0, const CvScalar &thresh1
		, const IplImage *mask = NULL, const double& alpha = 0.5f);

	void backProject(const IplImage *src, IplImage *H, IplImage *S, IplImage *V
		, IplImage *dst
		, const CvScalar &thresh0, const CvScalar &thresh1
		, const IplImage *mask = NULL);


	// ************ SUPPORT FUNCTIONS ************
//	// Writes histogram data to file (OpenCV's XML format)
//	bool save(const char* filename);

	// Paints an OpenCV image with histogram values
	void drawHist(IplImage *dst);

	// ******* Public Constants *******
	const int H_BINS;	// Number of hue bins in histogram
	const int S_BINS;	// Number of saturation bins
	
	// Provided as a public member variable for convenience (and laziness -_-)
	CvHistogram *hist;

private:
	// ******* Internal Data *********
	CvHistogram *oldHist;
	int sizes[2];
	IplImage *planes[2];
};

// Support Functions
inline void saveCvAddonHSVFilter(CvAddonHSVFilter* src, const char* path, const char* name)
{
	CvMat mat;
	cvGetMat( src->hist->bins, &mat, 0, 1 );
	cvAddonWriteCvArrXML(&mat, path, name);
}

// The hsv filter is created on the stack, so make sure to delete it when done!
inline CvAddonHSVFilter* loadCvAddonHSVFilter(const char* path, const char* name)
{
	CvAddonHSVFilter *hsv = NULL;
	CvMat blankMat;

	CvMat* mat = (CvMat*)cvAddonReadCvArrXML(path, name);

	if(mat != NULL) {
		hsv = new CvAddonHSVFilter(mat->rows, mat->cols);

		cvGetMat(hsv->hist->bins, &blankMat, 0, 1 );
		cvCopy(mat, &blankMat);
	}
	cvReleaseMat(&mat);
	return hsv;
}

#endif

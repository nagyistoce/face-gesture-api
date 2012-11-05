#pragma once
#pragma warning( disable: 4251 ) // Disable needs to have dll-interface...

#include "opencv2/core/core.hpp"

//! Class for analyzing arbitrary contours.
/*!
	\ingroup Core
*/
extern "C++" class __declspec(dllexport) Contour
{
public:
	//! Constructor
	Contour(void);

	//! Destructor
	~Contour(void);

	//! Finds the biggest contour.
	/*!
		\param frame the current frame.
		\param handContour The output vector for the contour.
	*/
	void FindLargestContour(cv::Mat& frame, std::vector<cv::Point>& handContour);

	//! Chain code representation for the contour.
	/*!
		Chain code values (8 direction):
		0: east
		1: north-east
		2: north
		3: north-west
		4: west
		5: south-west
		6: south
		7: south-east

		\param contour The contour of an object.
		\param chainCode Chain code representation of the shape.
	*/
	void ChainCode(std::vector<cv::Point>& contour, std::vector<int>& chainCode);

	//! Smoothing the chain code values trough a moving average.
	/*!
		\param chainCode The input array of chain codes.
		\param chainCodeSmoothed The smoothed output array for the chain codes.
	*/
	void SmoothChainCode(std::vector<int>& chainCode, std::vector<double>& chainCodeSmoothed);

	//! Estimating the derivate of chain values.
	/*!
		\param chainCode The input array of chain codes.
		\param chainCodeFirstDiff The output array for the first derivate.
		\param chainCodeSecondDiff The output array for the second derivate.
	*/
	void DeriveChainCode(std::vector<double>& chainCode, std::vector<double>& chainCodeFirstDiff, std::vector<double>& chainCodeSecondDiff);

	//! Draws the contour outlines or filled contours
	/*!
		\param image Destination image.
		\param contour The input contour.
		\param color Color of the contour.
		\param thickness Thickness of lines the contours are drawn with. If it is negative (for example, thickness=CV_FILLED), the contour interiors are drawn.
	*/
	void DrawContour(cv::Mat& image, std::vector<cv::Point>& contour, cv::Scalar& color, int thickness);

	//! Draws the contour outlines or filled contours
	/*!
		\param image Destination image.
		\param contour The input contour.
		\param color Color of the contour.
		\param thickness Thickness of lines the contours are drawn with. If it is negative (for example, thickness=CV_FILLED), the contour interiors are drawn.
	*/
	void DrawContour2f(cv::Mat& image, std::vector<cv::Point2f>& contour, cv::Scalar& color, int thickness);

	//! The function finds all convexity defects of the input contour.
	/*!
		\param contour The function finds all convexity defects of the input contour and fills the \param depth_, \param startPoint_, \param endPoint_ and \param depthPoint_.
		\param hull The indices of the contour points that make the hull.
		\param startPoint Points of the contour where the defect begins.
		\param endPoint Points of the contour where the defect ends.
		\param depthPoint The farthest from the convex hull point within the defect.
		\param depth Distance between the farthest point and the convex hull.
	*/
	void ILocateConvexityDefects(std::vector<cv::Point>& contour, std::vector<int>& hull, std::vector<cv::Point>& startPoint, std::vector<cv::Point>& endPoint, std::vector<cv::Point>& depthPoint, std::vector<float>& depth);

	//! Changes the contour position to minimize its energy.
	/*!
		The function updates the snake in order to minimize its total energy that is a sum of internal energy that depends on the contour shape (the smoother contour is, the smaller internal energy is) and external energy that depends on the energy field and reaches minimum at the local energy extremums, that correspond to the image edges in the case of using an image gradient.

		The parameter criteria.epsilon is used to define the minimal number of points that must be moved during any iteration to keep the iteration process running.

		If at some iteration the number of moved points is less than criteria.epsilon or the function performed criteria.max_iter iterations, the function terminates.

		\param contourMask The gray-scale image of the hand contour.
		\param activeContour The output contour points for active contour.
	*/
	void ISnakeImage(cv::Mat& contourMask, std::vector<cv::Point>& activeContour);

	//! Set the snake parameters
	/*!
		\param alpha Weight[s] of continuity energy, single float or array of length floats, one for each contour point.
		\param beta Weight[s] of curvature energy, similar to alpha.
		\param gamma Weight[s] of image energy, similar to alpha.
		\param perimeter Length of the contour.
		\param size Size of neighborhood of every point used to search the minimum, both size.width and size.height must be odd.
		\param maxIter For termination and convergation criteria.
		\param eps For termination and convergation criteria.
	*/
	void SetSnakeParameters(int alpha, int beta, int gamma, double perimeter, cv::Size size, int maxIter, double eps);
	
private:
	//! Structure for storing active contour parameters.
	struct SnakeParameters
	{
		int ialpha;					//<! Weight[s] of continuity energy, single float or array of length floats, one for each contour point.
		int ibeta;					//<! Weight[s] of curvature energy, similar to alpha.
		int igamma;					//<! Weight[s] of image energy, similar to alpha.
		double perimeter;			//<! Length of the original contour.
		int coeff_usage;			//<! Different uses of the alpha, beta, gamma parameters.
		CvSize size;				//<! Size of neighborhood of every point used to search the minimum, both size.width and size.height must be odd.
		CvTermCriteria criteria;	//<! Termination criteria.
		int calc_gradient;			//<! Gradient flag; if not 0, the function calculates the gradient magnitude for every image pixel and consideres it as the energy field, otherwise the input image itself is considered.
	};

	//! Method for processing the current hand mask.
	/*!
		\param frame The frame, on which the whole contour test run.
	*/
	void InitProcess(cv::Mat& frame);

	cv::Mat									grayFrame_;			//<! The frame for processing. 8-bit single-channel image.

	std::vector<std::vector<cv::Point> >	contours_;			//<! Detected contours over \param grayFrame_. Each contour is stored as a vector of points.
	std::vector<cv::Vec4i>					hierarchy_;			//<! Optional output vector containing information about the image topology.

	struct SnakeParameters					snakeParam_;		//<! Parameters of the active contour model.
};

#pragma once
#pragma warning( disable: 4251 4244 ) // Disable needs to have dll-interface, and int to float...

#include "opencv2/core/core.hpp"

//! Class for measuring similarity between two sequences which may vary in time or speed.
/*!
	Dynamic time warping (DTW) is an algorithm for measuring similarity between two sequences which may vary in time or speed.

	In general, DTW is a method that allows a computer to find an optimal match between two given sequences (e.g. time series) with certain restrictions. The sequences are "warped" non-linearly in the time dimension to determine a measure of their similarity independent of certain non-linear variations in the time dimension. This sequence alignment method is often used in the context of hidden Markov models.


	\ingroup Core
*/
extern "C++" class __declspec(dllexport) DTW2
{
public:
	//! Constructor
	DTW2(void);

	//! Constructor
	/*!
		\param n Length of the first sequence.
		\param m Length of the second sequence.
		\param window The maximum warping distance. Typically: "constraint = n/10", for "constraint = n" the process will be slower.
	*/
	DTW2(int n, int m);

	//! Destructor
	~DTW2(void);

	//! DTW algorithm. Currently uses euclidean distance.
	/*!
		\param v First sequence.
		\param w Second sequence.
		\param distType Type of the distance. See DtwDistance enum.
	*/
	float FastDTW(std::vector<double> &v, std::vector<double> &w, int distType = EUCLIDEAN_DST, bool showGrid = false);

	//! Enumeration for computing the distance.
	enum DtwDistance 
	{ 
		L1_DST = 0,			//<! L1 Distance
		EUCLIDEAN_DST = 1	//<! Euclidean Distance
	};

private:
	//! Inline function for minimum estimation.
	/*!
		\param x First element.
		\param y Second element.
	*/
   	static inline float Minimum(float x, float y) { return x <= y ? x : y; }

	//! Inline function for maximum estimation.
	/*!
		\param x First element.
		\param y Second element.
	*/
	static inline float Maximum(float x, float y) { return x >= y ? x : y; }

	//! L1 distance between two 2D float point.
	/*!
		\param p1 First point.
		\param p2 Second point.
	*/
	float L1Distance(double p1, double p2);

	//! Euclidean distance between two 2D float point.
	/*!
		\param p1 First point.
		\param p2 Second point.
	*/
	float EuclideanDistance(double p1, double p2);

	float**		grid_;				//<! DTW grid. Both sequences start on the bottom left of the grid.
	int			n_;					//<! Size of the first sequence.
	int			m_;					//<! Size of the second sequence.
	int			window_;			//<! The maximum warping distance.

	cv::Mat		dtwImg_;			//<! Visual representation of the grid.
};

#pragma once
#pragma warning( disable: 4251 4244 ) // Disable needs to have dll-interface, and int to float...

#include "opencv2/core/core.hpp"

//! Class for measuring similarity between two sequences which may vary in time or speed.
/*!
	Dynamic time warping (DTW) is an algorithm for measuring similarity between two sequences which may vary in time or speed.

	In general, DTW is a method that allows a computer to find an optimal match between two given sequences (e.g. time series) with certain restrictions. The sequences are "warped" non-linearly in the time dimension to determine a measure of their similarity independent of certain non-linear variations in the time dimension. This sequence alignment method is often used in the context of hidden Markov models.

	\ingroup Core
*/
extern "C++" class __declspec(dllexport) DTW
{
public:
	//! Constructor.
	/*!
		\param seq1 First sequence.
		\param seq2 Second sequence.
		\param distType Type of the distance. See DtwDistance enum.
		\param showGrid Shows the visual representation of the grid or not.
	*/
	DTW(std::vector<double> &seq1, std::vector<double> &seq2);

	//! Destructor.
	~DTW(void);

	//! Estimating the DTW distance between the two sequences.
	void ComputeDTW(void);

	//! Shows the visual representation of the grid.
	void ShowGrid(void);

	//! Gets the DTW distance between the two sequences.
	double GetDistance(void);

	//! Overloaded operator for the output information.
	/*!
		\param out Given output.
		\param o Current object
	*/
	friend std::ostream &operator<<( std::ostream &out, const DTW &o );

private:
	//! Computes a distance between two points
	/*!
		\param p1 First point.
		\param p2 Second point.
	*/
	double DistanceBetween(double p1, double p2);

	//! Finds the index of the minimum element from the given array.
	/*!
		\param a The array containing numeric values.
		\param n Array size.
	*/
	int GetIndexOfMinimum(double* a, int n);

	//! Changes the order of the warping path (increasing order).
	/*!
		\param path The warping path in reverse order.
	*/
	void ReversePath(std::vector<std::vector<int> > path);
	
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

	cv::Mat	dtwImg_;								//<! Visual representation of the grid.
	double	warpingDistance_;						//<! DTW distance.
	int		K_;										//<! max(n, m) <= K < n + m
	double	windowSize_;							//<! 
	
	std::vector<double>				seq1_;			//<! The first sequence.
	std::vector<double>				seq2_;			//<! The second sequence.
	std::vector<std::vector<int> >	warpingPath_;	//<!
};

inline std::ostream &operator<<( std::ostream &out, const DTW &o ) 
{
	out << "Warping Distance: " << o.warpingDistance_ << std::endl;
	out << "Warping Path: { ";

	for (int i = 0; i < o.K_; i++) 
	{
		out << "(" << o.warpingPath_[i][0] << ", " << o.warpingPath_[i][1] << ")";

		if(i != o.K_ - 1)
			out << ", ";
	}

	out << " }";

	return out;
}
#pragma once

#include "opencv2/opencv.hpp"

class AAM_CAM;

//! AAM Texture distribution model.
/*!
	\ingroup AAM
*/
extern "C++" class __declspec(dllexport) AAM_TDM  
{
	friend class AAM_CAM;
public:
	//! Constructor.
	AAM_TDM(void);

	//! Destructor.
	~AAM_TDM(void);

	//! Read data from stream 
	void Read(std::ifstream& is);

	//! calculate texture according to parameters lamda
	void CalcTexture(const CvMat* lamda, CvMat* texture);

	//! calculate parameters lamda according to texture sample 
	void CalcParams(const CvMat* texture, CvMat* lamda);

	//! Limit texture parameters.
    void Clamp(CvMat* lamda, double s_d = 3.0);

	//! normailize texture to mean texture
	static void AlignTextureToRef(const CvMat* refTextrure, CvMat* Texture);

	//! normalize texture make sure: sum of element is o and variance is 1 
	static void ZeroMeanUnitLength(CvMat* Texture);

	//! Get number of color-pixels in texture model 
	inline const int nPixels(void) const {
		return __MeanTexture->cols;
	}

	//! Get number of modes of texture variation
	inline const int nModes(void) const {
		return __TextureEigenVectors->rows;
	}

	//! Get mean texture
	inline const CvMat* GetMean(void) const {
		return __MeanTexture;
	}

	//! Get texture eigen-vectors of PCA (modes modes)
	inline const CvMat* GetBases(void) const {
		return __TextureEigenVectors;
	}
	
private:
	CvMat*  __MeanTexture;
	CvMat*  __TextureEigenVectors;
    CvMat*  __TextureEigenValues; 
};

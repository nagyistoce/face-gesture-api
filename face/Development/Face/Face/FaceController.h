#pragma once

/*!
	\defgroup FACE Face main module.
*/
#include "ModuleGraph.h"
#include "AlgorithmList.h"


class FaceController
{
public:
	//! Constructor.
	FaceController(void);

	//! Destructor.
	~FaceController(void);

	//! Loads all of the processes.
	/*!
		Loads the Process.xml and the modules which that file lists.
	*/
	void LoadFromProcessXml(void);

	//! Main algorithm.
	void ExecuteAlgorithmList(void);
	void ExecuteModuleGraph(void);

private:
	//! Loads global settings from the process file.
	void LoadDefaultSettings(const cv::FileStorage& fileStorage);

	void BuildAlgorithmList(const cv::FileStorage& fileStorage); 

	//! Loads the modules from the process file.
	void BuildModuleGraph(const cv::FileStorage& fileStorage); 


    bool mediaStreamStarted_; //!< DolMeHa started or not.
	ModuleGraph moduleGraph_; //!< Module graph.
	AlgorithmList algorithmList_; //!< Preprocessor object
};

#pragma once
#pragma warning( disable: 4251 ) // Disable needs to have dll-interface warning.

/*!
	\defgroup Core Core module.
*/

#include <string>

#define LocalSettingsPtr LocalSettings::GetInstance()

//! Singleton settings manager class.
/*!
	\ingroup Core

	Contains the paths of XMLs and video files.
*/
extern "C++" class __declspec(dllexport) LocalSettings
{
public:
	//! Instance getter.
	/*!
		\return The instance.
	*/
	static LocalSettings* GetInstance(void);

	//! Data directory getter.
	/*!
		\return Data directory root.
	*/
	std::string GetDataDirectory(void) const;

    //! Settings directory getter.
	/*!
		\return Settings directory root.
	*/
	std::string GetFeatureExtractorDirectory(void) const;

	//! Settings directory getter.
	/*!
		\return Settings directory root.
	*/
	std::string GetDetectorDirectory(void) const;

    //! Settings directory getter.
	/*!
		\return Settings directory root.
	*/
	std::string GetMotionDirectory(void) const;

    //! Process xml filename getter.
	/*!
		\return Settings directory root.
	*/
	std::string GetProcessXmlFileName(void) const;

private:
	//! Constructor.
	LocalSettings(void);

	//! Destructor.
	~LocalSettings(void);

	std::string	dataDirectory_;				//!< Data directory root.
    std::string	featureExtractorDirectory_;	//!< Settings directory root.
	std::string	detectorDirectory_;			//!< Settings directory root.
    std::string	motionDirectory_;			//!< Settings directory root.
	std::string	processXmlFileName_;		//!< The configuration file.
};

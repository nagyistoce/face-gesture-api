#include "Definitions.h"
#include "Similarity.h"

#include "Visualizer.h"
#include "LocalSettings.h"
#include <iomanip>

using namespace std;
using namespace cv;


struct AvgDstVectorCmp 
{
    bool operator()(const AvgDstElem& lhs, const AvgDstElem& rhs) 
    {
        return lhs.second < rhs.second;
    }
};


Similarity::Similarity(const string& databaseName, double diffThreshold)
:	databaseName_(databaseName),
	diffThreshold_(diffThreshold),
	minDst_(DBL_MAX),
	minErr_(DBL_MAX),
	clusterLabel_(UNKNOWN_CLUSTER),
    procFps_(0.0)
{
	ReadDatabase();
}


Similarity::~Similarity(void)
{
	WriteDatabase();
}


void Similarity::Predict(const vector<double>& seq)
{
	stopwatch_.Reset();
    //procTime_ = (double)cvGetTickCount();

	minDst_ = DBL_MAX;
	minErr_ = DBL_MAX;
	clusterLabel_ = UNKNOWN_CLUSTER;

    avgDistances_.clear();

	MovementClusterBuffer::const_iterator itMap;
	for(itMap = movementClusterBuffer_.begin(); itMap != movementClusterBuffer_.end(); itMap++)
	{
		double avgDst = 0.0;
		double avgErr = 0.0;
		MovementCluster::const_iterator itMc;

		for(itMc = itMap->second.begin(); itMc != itMap->second.end(); itMc++)
		{
			double err = 0.0;
			double dst = dtw_.DoDtw(seq, *itMc, err);

			avgDst += dst;
			avgErr += err;
		}

		if(itMap->second.size() > 0)
		{
			avgDst /= itMap->second.size();
			avgErr /= itMap->second.size();
		}

        avgDistances_.push_back(make_pair(itMap->first, avgDst));

		if(avgDst > 0.0 && avgDst < minDst_)
		{
			minDst_ = avgDst;
			minErr_ = avgErr;
			clusterLabel_ = itMap->first;
		}
	}

    procFps_ = stopwatch_.GetFPS();

	if (minDst_ > 0.0 && minDst_ < diffThreshold_)
	{
		cout << "- DTW (" << clusterLabel_ << "): " << minDst_ << ", " << minErr_ << endl;
		movementClusterBuffer_[clusterLabel_].push_back(seq);
		WriteDatabase();
	}
	else
	{
		cout << "- DTW : " << minDst_ << ", " << minErr_ << endl;
	}

	VisualizeRow();
	VisualizeColumn();

    //PrintSeq(seq);
}

void Similarity::VisualizeColumn()
{
	if(avgDistances_.empty())
	{
		return;
	}

	int barHeight = 30;
	double maxDst = 100.0;
	double stretch = 300.0;

	std::sort(avgDistances_.begin(), avgDistances_.end(), AvgDstVectorCmp());
	Mat output(barHeight * (avgDistances_.size()+1), 300, CV_8UC1, Scalar(0.0));

	stringstream ss;

	ss << "DTW: " << cvRound(procFps_) << " FPS.";
	VisualizerPtr->PutText(output, ss.str(), Point(5, 20));

	for(int i = 0; i < int(avgDistances_.size()); i++)
	{
		int barWidth = cvRound(avgDistances_[i].second / maxDst * stretch);
		Rect r(0, (i+1) * barHeight, barWidth, barHeight);

		rectangle(output, r, i%2 ? Scalar(100.0) : Scalar(200.0), -1, CV_AA);

		rectangle(output, r, Scalar(barWidth%255), -1, CV_AA);
		rectangle(output, r, Scalar(255.0), 1, CV_AA);

		stringstream ss;

		if(avgDistances_[i].first >= 97 && avgDistances_[i].first <= 122)
			ss << char(avgDistances_[i].first) << ": " << avgDistances_[i].second;
		else
			ss << avgDistances_[i].first << ": " << avgDistances_[i].second;

		VisualizerPtr->PutText(output, ss.str(), Point(5, (i+1) * barHeight + 20));
	}

	VisualizerPtr->ShowImage("DTW Distances", output);
}

void Similarity::VisualizeRow()
{
	if(avgDistances_.empty())
	{
		return;
	}

	cv::Vec2i resolution(2, 26); // sor, oszlop
	cv::Vec2i size(30, 50);		 // px / sor, px / oszlop

	cv::Mat output(resolution[0] * size[0], resolution[1] * size[1], CV_8UC3, cv::Scalar::all(255.0));
	std::vector<cv::Mat> splitChannels;
	for(int i = 0; i < 3; i++)
	{
		splitChannels.push_back(cv::Mat(output.size(), CV_8UC1, cv::Scalar::all(255.0)));
	}

	const int avgDistancesSize = static_cast<int>(avgDistances_.size());
	const int letterA = 'a';
	cv::Scalar gridColor(0.0, 0.0, 0.0);

	double maxVal = -DBL_MAX;
	for(int k = 0; k < avgDistancesSize; k++)
	{
		if(avgDistances_[k].second > maxVal)
		{
			maxVal = avgDistances_[k].second;
		}
	}

	// Táblázat színezése
	for(int i = 0; i <= resolution[0]; i++)
	{
		for(int j = 0; j <= resolution[1]; j++)
		{
			for(int k = 0; k < avgDistancesSize; k++)
			{
				if(avgDistances_[k].first == letterA + j)
				{
					double color = ((maxVal - avgDistances_[k].second) / maxVal) * 255.0;
					cv::rectangle(splitChannels[1], cv::Point(j * size[1], 0), cv::Point((j + 1) * size[1], output.rows), cv::Scalar::all(255 - color), -1);
					cv::rectangle(splitChannels[2], cv::Point(j * size[1], 0), cv::Point((j + 1) * size[1], output.rows), cv::Scalar::all(255 - color), -1);
					break;
				}
			}
		}
	}

	cv::merge(splitChannels, output);

	for(int i = 0; i <= resolution[0]; i++)
	{
		for(int j = 0; j <= resolution[1]; j++)
		{
			// DTW értékek kiírása
			for(int k = 0; k < avgDistancesSize; k++)
			{
				if(avgDistances_[k].first == letterA + j)
				{
					std::stringstream ss;
					ss << cvRound(avgDistances_[k].second * 10.0) / 10.0;
					VisualizerPtr->PutText(output, ss.str(), cv::Point(j * size[1] + cvRound(1.0 * size[1] / 6.0), (i + 1) * size[0] + cvRound(4.0 * size[0] / 6.0)));

					break;
				}
			}

			// Fejléc kitöltése
			if(i == 0)
			{
				std::stringstream ss;
				ss << static_cast<char>(letterA + j);
				VisualizerPtr->PutText(output, ss.str(), cv::Point(j * size[1] + cvRound(2.0 * size[1] / 6.0), i * size[0] + cvRound(4.0 * size[0] / 6.0)));
			}

			// grid kirajzolása
			cv::Point ptRow1(0, i * size[0]);
			cv::Point ptRow2(output.cols, i * size[0]);
			cv::line(output, ptRow1, ptRow2, gridColor, 1, CV_AA);

			cv::Point ptCol1(j * size[1], 0);
			cv::Point ptCol2(j * size[1], output.rows);
			cv::line(output, ptCol1, ptCol2, gridColor, 1, CV_AA);
		}
	}

	VisualizerPtr->ShowImage("DTW Distances - Row", output);
	cv::waitKey(-1);
}

void Similarity::PrintSeq(const vector<double>& seq)
{
    for (size_t i = 0; i < seq.size(); i++)
        cout << seq[i] << ". ";

    cout << endl;
}


Movement Similarity::CreateVector(int count, ...)
{
	va_list arguments;
	Movement retVector;

	va_start(arguments, count); 
	for(int i = 0; i < count; i++)
	{
		int ithAngle = va_arg(arguments, int);
		retVector.push_back(ithAngle);
	}
	va_end(arguments);

	return retVector;
}


void Similarity::WriteDatabase(void)
{
	FileStorage fs(LocalSettingsPtr->GetGestureDirectory() + databaseName_, FileStorage::WRITE);
	if(!fs.isOpened())
		CV_Error(1, "XML does not exist (" + LocalSettingsPtr->GetGestureDirectory() + databaseName_ + ")!");

	fs << "movement" << "[";
	
	MovementClusterBuffer::const_iterator itMap;
	for(itMap = movementClusterBuffer_.begin(); itMap != movementClusterBuffer_.end(); itMap++)
	{
		MovementCluster::const_iterator itMc;
		for(itMc = itMap->second.begin(); itMc != itMap->second.end(); itMc++)
			fs << "{:" << "id" << itMap->first << "angles" << *itMc << "}";
	}

	fs << "]";
	fs.release();
}


void Similarity::ReadDatabase(void)
{
	FileStorage fs(LocalSettingsPtr->GetGestureDirectory() + databaseName_, FileStorage::READ);
	if(!fs.isOpened())
		CV_Error(1, "XML does not exist (" + LocalSettingsPtr->GetGestureDirectory() + databaseName_ + ")!");

	FileNode node = fs["movement"];

	for(size_t i = 0; i < node.size(); ++i)
	{
		Movement movement;
		int id;

		node[i]["id"] >> id;
		node[i]["angles"] >> movement;

		movementClusterBuffer_[id].push_back(movement);
	}

	fs.release();
}

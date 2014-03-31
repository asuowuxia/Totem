#include "polorglcmblock.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "../../../designnet/designnet_core/data/imagedata.h"
#include "../../../designnet/designnet_core/data/matrixdata.h"
#include "../../../designnet/designnet_core/property/doublerangeproperty.h"


static const char PN_OUT_GLCMFeature[]	= "Polor GLCM Feature";
static const char PN_IN_GrayImage[]		= "Gray Image";
static const char PN_IN_BinaryImage[]	= "Binary Image";
static const char PN_IN_Centroid[]		= "Centroid";

using namespace std;
using namespace DesignNet;
namespace FlowerFeatureExtraction{

PolorGLCMBlock::PolorGLCMBlock(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent)
{
	addPort(Port::IN_PORT, DATATYPE_BINARYIMAGE, PN_IN_BinaryImage);
	addPort(Port::IN_PORT, DATATYPE_GRAYIMAGE, PN_IN_GrayImage);
	addPort(Port::IN_PORT, DATATYPE_MATRIX, PN_IN_Centroid);
	addPort(Port::OUT_PORT, DATATYPE_MATRIX, PN_OUT_GLCMFeature);
	setName(tr("Polor GLCM"));
	m_doubleRangeProperty = new DoubleRangeProperty(QLatin1String("threshold"),
		tr("Threshold"), this);
	m_binCountProperty = new DoubleRangeProperty(QLatin1String("binCount"),
		tr("Bins"), this);
	m_binCountProperty->setRange(1, 255);
	m_binCountProperty->setDecimals(0);
	m_binCountProperty->setValue(8);
	addProperty(m_doubleRangeProperty);
	addProperty(m_binCountProperty);

	m_glcm = cv::Mat::zeros(m_binCountProperty->value(),m_binCountProperty->value(), CV_32F);
	m_glcm_sobel = cv::Mat::zeros(m_binCountProperty->value(),m_binCountProperty->value(), CV_32F);
}

PolorGLCMBlock::~PolorGLCMBlock()
{

}

QString PolorGLCMBlock::title() const
{
	return tr("Polor GLCM");
}

QString PolorGLCMBlock::category() const
{
	return tr("Flowers/Feature");
}

bool PolorGLCMBlock::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	notifyDataWillChange();
	MatrixData *pBinaryMatrix = (MatrixData*)getOneData(PN_IN_BinaryImage).variant.value<IData*>();
	MatrixData *pGrayMatrix = (MatrixData*)getOneData(PN_IN_GrayImage).variant.value<IData*>();
	MatrixData *pCentroidMatrix = (MatrixData*)getOneData(PN_IN_Centroid).variant.value<IData*>();

	cv::Mat binaryImage = pBinaryMatrix->getMatrix();
	cv::Mat grayMat = pGrayMatrix->getMatrix();
	cv::Mat centroidMat = pCentroidMatrix->getMatrix();
	/// 中心
	cv::Point2d centroid(0, 0);
	centroid.x = centroidMat.at<float>(0, 0);
	centroid.y = centroidMat.at<float>(0, 1);
	vector<vector<cv::Point> > countours;
	cv::findContours(binaryImage.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
	if(counts <= 0)
		return false;/// 找到的轮廓数不能为0
	double maxarea = 0;
	cv::Rect maxrect(0, 0, 2, 2);
	size_t max_index = -1;
	for (size_t i = 0; i < counts ; ++i)
	{
		cv::Rect rect = cv::boundingRect(countours.at(i));
		if(rect.area() > maxarea)
		{
			maxarea = rect.area();
			maxrect = rect;
			max_index = i;
		}
	}
	
	/// 求重心 坐标求和后求平均值
	int iCount = 0;
	int iX = 0;
	int iY = 0;
	int iBottom = maxrect.y + maxrect.height;
	int iRight = maxrect.x + maxrect.width;
	m_centroid = centroid;
	double radius = qMin(iRight - centroid.x, iBottom - centroid.y);/// 半径
	
	/// 求灰度共生矩阵(包括原灰度图的glcm和经过sobel算子滤波后的图像的glcm)
	cv::Mat sobelMat;
	cv::Sobel(grayMat, sobelMat, -1, 1, 0);

	double B_min = radius / 3;
	double B_max = radius * 2 / 3;
	double deltaBin = 255/ m_binCountProperty->value(); /// 每个bin的范围
	cv::Point2i p1, p2;
	for (int sita = 0; sita < 360; ++sita)
	{
		for(int r = 0; r < radius; ++r)
		{
			p1.x = centroid.x + r * cos(float(sita));
			p1.y = centroid.y - r * sin(float(sita));
			p2.x = p1.x + cos(float(sita));		/// 等价于p2.x = centroid.x + (r+1) * cos(sita);
			p2.y = p1.y - sin(float(sita));		/// 等价于p2.y = centroid.y - (r+1) * sin(sita);
			if (p2.x < 0 || p2.y < 0 || p1.x < 0 || p1.y < 0 || 
				p2.x >= grayMat.cols || p1.x >= grayMat.cols ||
				p2.y >= grayMat.rows || p1.y >= grayMat.rows)
			{
				continue;
			}
			///找到P1, p2 两个点的灰度级
			int v1 = grayMat.at<uchar>(p1.y, p1.x)/deltaBin;
			int v2 = grayMat.at<uchar>(p2.y, p2.x)/deltaBin;
			v1 = v1 >= m_glcm.rows ? m_glcm.rows - 1 : v1;
			v2 = v2 >= m_glcm.cols ? m_glcm.cols - 1 : v2;
			if (v1 < 0)
			{
				v1 = 0;
			}
			if (v2 < 0)
			{
				v2 = 0;
			}
			m_glcm.at<float>(v1, v2)++;

			v1 = sobelMat.at<uchar>(p1.y, p1.x)/deltaBin;
			uchar f = sobelMat.at<uchar>(p2.y, p2.x);
			v2 = sobelMat.at<uchar>(p2.y, p2.x)/deltaBin;
			v1 = v1 >= m_glcm.rows ? m_glcm.rows - 1 : v1;
			v2 = v2 >= m_glcm.cols ? m_glcm.cols - 1 : v2;
			if (v1 < 0)
			{
				v1 = 0;
			}
			if (v2 < 0)
			{
				v2 = 0;
			}
			m_glcm_sobel.at<float>(v1, v2)++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	///计算灰度共生矩阵的特征值
	/// 归一化灰度共生矩阵
	//////////////////////////////////////////////////////////////////////////

	/// 求裴勇论文中的F[6] ,边缘轮廓点的个数/（边缘轮廓点到中心的距离之和）
	double sum = 0;
	vector<cv::Point>::iterator itr = countours.at(max_index).begin();
	vector<cv::Point>::iterator end = countours.at(max_index).end();
	while(itr != end)
	{
		sum += sqrt( ( (*itr).x - centroid.x) * ( (*itr).x - centroid.x) + ( (*itr).y - centroid.y) * ( (*itr).y - centroid.y));
		itr++;
	}
	if(qFuzzyCompare(sum, 0))//等于0要单独处理一下
		sum = 1;
	cv::Mat feature(1, 7, CV_32FC1);
	feature.at<float>(0, 0) = countours.at(max_index).size() / sum;///边缘轮廓点的个数/（边缘轮廓点到中心的距离之和）
	normalize(m_glcm);
	feature.at<float>(0, 1) = energe(m_glcm);
	feature.at<float>(0, 2) = contrast(m_glcm);
	feature.at<float>(0, 3) = homogeneity(m_glcm);
	normalize(m_glcm_sobel);
	feature.at<float>(0, 4) = energe(m_glcm_sobel);
	feature.at<float>(0, 5) = contrast(m_glcm_sobel);
	feature.at<float>(0, 6) = homogeneity(m_glcm_sobel);
	MatrixData data;
	data.setMatrix(feature);
	pushData(&data, PN_OUT_GLCMFeature);
	notifyProcess();
	return true;
}


void PolorGLCMBlock::propertyChanged( DesignNet::Property *prop )
{
	if(prop == m_binCountProperty)
	{
		m_glcm = cv::Mat::zeros(m_binCountProperty->value(),m_binCountProperty->value(), CV_32FC1);
		m_glcm_sobel = cv::Mat::zeros(m_binCountProperty->value(),m_binCountProperty->value(), CV_32FC1);
	}
	
}

void PolorGLCMBlock::normalize(cv::Mat &glcm)
{
	float sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			sum += glcm.at<float>(i,j);
		}
	}
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			glcm.at<float>(i,j) = glcm.at<float>(i,j) /sum;
		}
	}
}

float PolorGLCMBlock::energe(const cv::Mat &glcm)
{
	float sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			sum += glcm.at<float>(i,j) * glcm.at<float>(i,j);
		}
	}
	return sum;
}

void PolorGLCMBlock::centroid()
{

}

float PolorGLCMBlock::homogeneity( const cv::Mat &glcm )
{
	float sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			sum += glcm.at<float>(i,j) / (qAbs(i - j)  + 1);
		}
	}
	return sum;
}

float PolorGLCMBlock::contrast( const cv::Mat &glcm )
{
	double sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			sum += glcm.at<float>(i,j) * (i - j) * (i - j);
		}
	}
	return sum;
}



}

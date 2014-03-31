#ifndef BINARYIMAGE_H
#define BINARYIMAGE_H
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{
class DoubleRangeProperty;
}
namespace Conversion{
class BinaryImage : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(BinaryImage)

	BinaryImage(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~BinaryImage();
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数

protected:

	virtual void propertyChanged(DesignNet::Property *prop);


	DesignNet::ImageData m_binaryData;				//!< 二值图
	DesignNet::ImageData m_grayData;				//!< 灰度图
	DesignNet::DoubleRangeProperty* m_doubleRangeProperty; //!< 阈值
};

}

#endif // BINARYIMAGE_H

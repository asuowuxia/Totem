#ifndef BINARYIMAGE_H
#define BINARYIMAGE_H

#include "designnet/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnet/designnet_core/designnetbase/port.h"
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
	BinaryImage(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~BinaryImage();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ��������
protected:
	virtual void propertyChanged(DesignNet::Property *prop);
	virtual bool connectionTest(DesignNet::Port* src, DesignNet::Port* target);
	
	DesignNet::Port m_outputPort;	//!< ����˿�
	DesignNet::Port m_inputPort;	//!< ����˿�
	DesignNet::ImageData m_binaryData;				//!< ��ֵͼ
	DesignNet::ImageData m_grayData;				//!< �Ҷ�ͼ
	DesignNet::DoubleRangeProperty* m_doubleRangeProperty; //!< ��ֵ
};

}

#endif // BINARYIMAGE_H
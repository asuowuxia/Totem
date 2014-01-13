﻿#ifndef PROCESSOR_H
#define PROCESSOR_H


#include "../designnet_core_global.h"
#include "../property/propertyowner.h"
#include "../data/datatype.h"
#include "Utils/XML/xmldeserializer.h"
#include "port.h"
#include <QFutureInterface>
#include <QFutureWatcher>
#include <QObject>
#include <QIcon>
#include <QVector>
#include <QList>
#include <QMap>
#include <QReadWriteLock>

#define DECLEAR_PROCESSOR(x) \
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const \
	{ \
		return new x(space); \
	}


QT_BEGIN_NAMESPACE
class QThread;
QT_END_NAMESPACE

namespace DesignNet{

class IData;
class DesignNetSpace;
class Processor;

enum ProcessorType
{
	ProcessorType_Permanent,
	ProcessorType_Once
};

class ProcessResult
{
public:
	ProcessResult() : m_bSucessed(true), m_bNeedLoop(false)
	{
	}
	bool m_bSucessed;
	bool m_bNeedLoop;	// 是否需要重复执行
};

class ProcessorWorker : public QObject{
	Q_OBJECT
public:
	ProcessorWorker(Processor *processor);
	bool isWorking();
	Processor* m_processor;
public slots:
	void run();
	void stopped();
	void started();
protected:
	QFutureInterface<ProcessResult> futureInterface;
	bool			m_bWorking;				//!< 是否正在执行
	QMutex			m_mutex;
};

class DESIGNNET_CORE_EXPORT Processor : public QObject, public PropertyOwner
{
	friend class ProcessorWorker;
	Q_OBJECT
public:
    
	explicit Processor(DesignNetSpace *space = 0, QObject* parent = 0, ProcessorType processorType = ProcessorType_Once);
    virtual ~Processor();
	
	virtual void init() { }
	virtual Processor* create(DesignNetSpace *space = 0) const = 0;  //!< 创建Processor

	void waitForFinish();
	ProcessResult result() { return m_watcher.result(); }

	int indegree(QList<Processor*> exclusions = QList<Processor*>()) const; //!< 计算入度
	virtual bool connectionTest(Port* pOutput, Port* pInput);
	virtual bool connectionTest(Processor* father);

	QList<Processor*> getInputProcessor() const;
	QList<Processor*> getOutputProcessor() const;

	//////////////////////////////////////////////////////////////////////////

	QIcon	icon() const;
	void	setIcon(const QString &str);
    
    void    setName(const QString& name);	//!< set name
    QString name() const;					//!< get name

    void    setID(const int &id);			//!< 在一个DesignNetSpace中的唯一ID
    int     id() const;						//!< getter

	DesignNetSpace *space() const{ return m_space; }
	void	setSpace(DesignNetSpace *space) ;

    virtual Core::Id typeID() const;		//!< 返回类型ID
    virtual QString category() const;		//!< 返回种类
    
	//////////////////////////////////////////////////////////////////////////

	bool	addPort(Port::PortType pt, DataType dt, QString sLabel);
	Port*	getPort(Port::PortType pt, DataType dt);
	Port*	getPort(Port::PortType pt, QString sLabel);
	QList<Port*> getPorts(Port::PortType pt) const;
	
	
	QList<ProcessData*> getData(QString sLabel);

	void pushData(const ProcessData &pd, QString strLabel);
	void pushData(QVariant &var, DataType dataType, QString strLabel = "", int iProcessId = -1);
	void pushData(IData* data, QString strLabel = "", int iProcessId = -1);

	QList<ProcessData> getOutputData(DataType dt = DATATYPE_INVALID);
	QList<ProcessData> getInputData(DataType dt = DATATYPE_INVALID);

	//////////////////////////////////////////////////////////////////////////

	void start();

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;
	virtual QString serializableType() const;
	virtual Utils::XmlSerializable* createSerializable() const;
	
	bool isRunning();

	virtual bool prepareProcess() { return true; }

	bool connectTo(Processor* child);
	bool disconnect(Processor* pChild);
	void detach();

	QReadWriteLock m_workingLock;

signals:
	void logout(QString log);
	void connected(Processor* father, Processor* pChild);
	void disconnected(Processor* father, Processor* pChild);

public slots:

	void run(QFutureInterface<ProcessResult> &future);//!< 线程执行该函数

	void onPropertyChanged_internal();

protected:

	virtual void propertyChanged(Property *prop);
	virtual void propertyAdded(Property* prop);

	virtual bool beforeProcess(QFutureInterface<ProcessResult> &future);		//!< 处理之前的准备,这里会确保数据已经准备好了
	virtual bool process(QFutureInterface<ProcessResult> &future) = 0;			//!< 正式处理
	virtual void afterProcess(bool status = true);		//!< 完成处理
	virtual bool finishProcess() { return true; }
	
	//////////////////////////////////////////////////////////////////////////

	QIcon			m_icon;
	QString			m_title;				//!< 种类title
	int				m_id;
	ProcessorType	m_eType;

	QList<Port*>	m_outputPort;			//!< 所有的输出端口
	QList<Port*>	m_inputPort;			//!< 输入端口

	ProcessorWorker m_worker;
	DesignNetSpace* m_space;				//!< DesignNetSpace

	QFutureWatcher<ProcessResult> m_watcher;	//!< 用于控制进度
	QThread*  m_thread;
};
}

#endif // PROCESSOR_H

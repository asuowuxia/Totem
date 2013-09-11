#include "operations.h"
#include "joinprocessor.h"
#include "bufferprocessor.h"
#include "FeatureSaver.h"
#include "featuresaverconfig.h"
#include "designnetbase/processorconfigmanager.h"
#include <QtPlugin>
using namespace DesignNet;

Operations::Operations()
{
	m_join			= 0;
	m_buffer		= 0;
	m_featureSaver	= 0;
}

Operations::~Operations()
{

}

bool Operations::initialize( const QStringList &arguments, QString *errorString )
{
	return true;
}

void Operations::extensionsInitialized()
{
	m_join = new JoinProcessor(0);
	m_buffer = new BufferProcessor(0);
	m_featureSaver = new FeatureSaver(0);
	ProcessorConfigManager::instance()->registerConfigWidget(m_featureSaver->typeID().toString(), new FeatureSaverConfig(0, 0));

	addAutoReleasedObject(m_featureSaver);
	addAutoReleasedObject(m_join);
	addAutoReleasedObject(m_buffer);
}
Q_EXPORT_PLUGIN2(Operations, Operations)
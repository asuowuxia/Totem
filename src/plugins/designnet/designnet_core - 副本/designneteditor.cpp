#include "designneteditor.h"
#include <QtConcurrent/QtConcurrent>
#include <QAction>
#include <QMessageBox>
#include <QTextEdit>
#include <QToolBar>
#include "designnetbase/designnetspace.h"
#include "Utils/XML/xmldeserializer.h"
#include "designnetconstants.h"
#include "designnetdocument.h"
#include "designnetfrontwidget.h"
#include "designnetview.h"
#include "processorgraphicsblock.h"


namespace DesignNet{

class DesignNetEditorPrivate
{
public:
	DesignNetEditorPrivate();
	~DesignNetEditorPrivate();
	DesignNetDocument*			m_file;
	DesignNetFrontWidget*		m_designNetWidget;
	DesignNetView*	m_designNetView;
	QToolBar*		m_toolBar;
};

DesignNetEditorPrivate::DesignNetEditorPrivate()
{
	m_file			= 0;
	m_designNetView = new DesignNetView(0);
}

DesignNetEditorPrivate::~DesignNetEditorPrivate()
{
}

DesignNetEditor::DesignNetEditor(QObject *parent)
	: Core::IEditor(parent),
	d(new DesignNetEditorPrivate())
{
	d->m_file = new DesignNetDocument(this);
	d->m_designNetWidget = new DesignNetFrontWidget(this);
	setWidget(d->m_designNetWidget);
	
	d->m_designNetView->setDesignNetSpace(d->m_file->designNetSpace());
	d->m_toolBar = new QToolBar(tr("Build"), d->m_designNetView);

	QAction *pRunAction = new QAction(this);
	QIcon icon(":/media/start.png");
	pRunAction->setIcon(icon);
	d->m_toolBar->addAction(pRunAction);

	connect(d->m_file, SIGNAL(changed()), this, SIGNAL(changed()));
	connect(d->m_file, SIGNAL(deserialized(Utils::XmlDeserializer &)), 
		this, SLOT(onDeserialized(Utils::XmlDeserializer &)));
	connect(d->m_file, SIGNAL(serialized(Utils::XmlSerializer &)), 
		this, SLOT(onSerialized(Utils::XmlSerializer &)));
	createCommand();
}

DesignNetEditor::~DesignNetEditor()
{
	delete d;
}

bool DesignNetEditor::createNew( const QString &contents /*= QString()*/ )
{
	return true;
}

bool DesignNetEditor::open( QString *errorString, const QString &fileName, const QString &realFileName )
{
	bool bret = d->m_file->open(errorString, fileName, realFileName);
	connect(d->m_file->designNetSpace(), SIGNAL(processFinished()), this, SIGNAL(designNetFinished()));
	return bret;
}

Core::IDocument * DesignNetEditor::document()
{
	return d->m_file;
}

Core::Id DesignNetEditor::id() const
{
	return DesignNet::Constants::NETEDITOR_ID;
}

QString DesignNetEditor::displayName() const
{
	return d->m_file->fileName();
}

void DesignNetEditor::setDisplayName( const QString &title )
{

}

QByteArray DesignNetEditor::saveState() const
{
	return QByteArray();
}

bool DesignNetEditor::restoreState( const QByteArray &state )
{
	return true;
}

bool DesignNetEditor::isTemporary() const
{
	return false;
}

QWidget * DesignNetEditor::toolBar()
{
	return d->m_toolBar;
}

Core::Id DesignNetEditor::preferredModeType() const
{
	return DesignNet::Constants::DESIGNNET_MODETYPE;
}

DesignNetView * DesignNetEditor::designNetView()
{
	return d->m_designNetView;
}

void DesignNetEditor::onDeserialized( Utils::XmlDeserializer &x )
{
	d->m_designNetView->deserialize(x);
}

void DesignNetEditor::onSerialized( Utils::XmlSerializer &s )
{
	d->m_designNetView->serialize(s);
}

void DesignNetEditor::createCommand()
{

}

bool DesignNetEditor::run()
{
	if (d->m_file->designNetSpace()->isRunning())
		return false;

	QFutureWatcher<bool> bWatcher;
	bWatcher.setFuture(QtConcurrent::run(d->m_file->designNetSpace(), &DesignNetSpace::prepareProcess));
	bWatcher.waitForFinished();
	if (bWatcher.result() == false)
		return false;
	
	d->m_file->designNetSpace()->start();
	return true;
}

void DesignNetEditor::onFinish()
{
	int i = 0;
	i++;
}

}

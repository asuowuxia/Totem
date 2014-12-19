// Author: miaojinquan
#pragma once

#include <QWidget>
#include <QVector>
#include <QTabWidget>
#include <QScrollArea>
#include "CustomUI/uicollapsiblewidget.h"

namespace DesignNet
{
class DesignNetEditor;
class DesignNetSpace;
class ProcessorFrontWidget;
class Processor;

class DesignNetFrontWidget : public QScrollArea
{
	Q_OBJECT

public:

	DesignNetFrontWidget(DesignNetEditor* pEditor);
	~DesignNetFrontWidget();

public slots:

	void onDesignNetFinished();
	void onProcessorAdded(Processor* processor);
	void onProcessorRemoved(Processor* processor);

protected:

	 void resizeEvent(QResizeEvent* e);

private:

	DesignNetEditor*	m_pEditor;
	DesignNetSpace*		m_pSpace;
	QVector<ProcessorFrontWidget*> m_vecWidget;

	CustomUI::uiCollapsibleWidget m_widget;
};
}
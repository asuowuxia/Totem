#ifndef DOUBLERANGEPROPERTY_H
#define DOUBLERANGEPROPERTY_H

#include "property.h"


namespace DesignNet
{
class DoubleRangePropertyPrivate;
class DESIGNNET_CORE_EXPORT DoubleRangeProperty : public Property
{
	Q_OBJECT

	DECLARE_PROPERTY_TYPE(Propety_Bool)

public:

	DoubleRangeProperty(const QString& id, const QString& name, QObject* parent = 0);
	~DoubleRangeProperty();
	void setRange(const double& dMin, const double& dMax);

	void	setMinimum(const double& dmin);
	double	minimum() const;
	void	setMaximum(const double& dmax);
	double	maximum() const;
	void	setStep(const double& step);
	double	step() const;

	void	setDecimals(int prec);
	int		decimals() const;

	void	setValue(const double& v);
	double	value() const;

signals:

	void rangeChanged(double dmin, double dmax);
	void valueChanged(int value);

public slots:

	void onValueChanged(double value);

private:


	double	m_value;		//!< double 类型的数据
	double	m_step;			//!< 步长
	double	m_maximum;		//!< 最大值
	double	m_minimum;		//!< 最小值
	int		m_decimals;		//!< 精度
};
}
#endif // DOUBLERANGEPROPERTY_H

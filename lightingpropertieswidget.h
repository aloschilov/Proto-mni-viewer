#ifndef LIGHTINGPROPERTIESWIDGET_H
#define LIGHTINGPROPERTIESWIDGET_H

#include <QWidget>

class QDoubleSpinBox;
class QCheckBox;

class LightingPropertiesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LightingPropertiesWidget(QWidget *parent = 0);

    QDoubleSpinBox *ambientDoubleSpinBox;
    QDoubleSpinBox *specularDoubleSpinBox;
    QDoubleSpinBox *diffuseDoubleSpinBox;
    QDoubleSpinBox *opacityDoubleSpinBox;

    QCheckBox *enableLightingCheckbox;
    QCheckBox *lightingWidgetVisibilityCheckbox;
    QCheckBox *enableDepthSortingCheckbox;
signals:
    
public slots:

private:
};

#endif // LIGHTINGPROPERTIESWIDGET_H

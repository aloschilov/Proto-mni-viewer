#include <QtGui>

#include "lightingpropertieswidget.h"

LightingPropertiesWidget::LightingPropertiesWidget(QWidget *parent) :
    QWidget(parent)
{
    ambientDoubleSpinBox = new QDoubleSpinBox();
    ambientDoubleSpinBox->setRange(0.0, 1.0);
    ambientDoubleSpinBox->setSingleStep(0.1);
    diffuseDoubleSpinBox = new QDoubleSpinBox();
    diffuseDoubleSpinBox->setRange(0.0, 1.0);
    diffuseDoubleSpinBox->setSingleStep(0.1);
    specularDoubleSpinBox = new QDoubleSpinBox();
    specularDoubleSpinBox->setRange(0.0, 1.0);
    specularDoubleSpinBox->setSingleStep(0.1);
    opacityDoubleSpinBox = new QDoubleSpinBox();
    opacityDoubleSpinBox->setRange(0.0, 1.0);
    opacityDoubleSpinBox->setSingleStep(0.1);

    QLabel *ambientLabel = new QLabel(tr("&Ambient lighting coefficient"));
    QLabel *diffuseLabel = new QLabel(tr("&Diffuse lighting coefficient"));
    QLabel *specularLabel = new QLabel(tr("&Specular lighting coefficient"));
    QLabel *opacityLabel = new QLabel(tr("&Opacity"));

    ambientLabel->setBuddy(ambientDoubleSpinBox);
    diffuseLabel->setBuddy(diffuseDoubleSpinBox);
    specularLabel->setBuddy(specularDoubleSpinBox);
    opacityLabel->setBuddy(opacityDoubleSpinBox);

    enableLightingCheckbox = new QCheckBox(tr("Enable &lighting"));

    lightingWidgetVisibilityCheckbox = new QCheckBox(tr("Show lighing &position widget"));

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(ambientLabel, 0, 0);
    gridLayout->addWidget(ambientDoubleSpinBox, 0, 1);

    gridLayout->addWidget(diffuseLabel, 1, 0);
    gridLayout->addWidget(diffuseDoubleSpinBox, 1, 1);

    gridLayout->addWidget(opacityLabel, 2, 0);
    gridLayout->addWidget(opacityDoubleSpinBox, 2, 1);

    gridLayout->addWidget(specularLabel, 3, 0);
    gridLayout->addWidget(specularDoubleSpinBox, 3, 1);

    gridLayout->addWidget(enableLightingCheckbox, 4, 0, 1, 2);
    gridLayout->addWidget(lightingWidgetVisibilityCheckbox, 5, 0, 1, 2);

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

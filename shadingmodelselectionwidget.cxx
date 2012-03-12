#include <QtGui>
#include "shadingmodelselectionwidget.h"


ShadingModelSelectionWidget::ShadingModelSelectionWidget(QWidget *parent) :
    QWidget(parent)
{
    QGroupBox* simpleShadingModelsGroupBox = new QGroupBox();
    simpleShadingModelsGroupBox->setTitle(tr("Select pre-defined shading model."));

    flatShadingModelRadioButton = new QRadioButton(tr("Flat"), simpleShadingModelsGroupBox);
    gouraundShadingModelRadioButton = new QRadioButton(tr("Gouraud"), simpleShadingModelsGroupBox);
    phongShadingModelRadioButton = new QRadioButton(tr("Phong"), simpleShadingModelsGroupBox);

    flatShadingModelRadioButton->setChecked(true);

    QVBoxLayout *simpleShadingModelsGroupBoxLayout = new QVBoxLayout();
    simpleShadingModelsGroupBoxLayout->addWidget(flatShadingModelRadioButton);
    simpleShadingModelsGroupBoxLayout->addWidget(gouraundShadingModelRadioButton);
    simpleShadingModelsGroupBoxLayout->addWidget(phongShadingModelRadioButton);

    simpleShadingModelsGroupBox->setLayout(simpleShadingModelsGroupBoxLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(simpleShadingModelsGroupBox);
    mainLayout->addStretch();

    setLayout(mainLayout);

    connect(flatShadingModelRadioButton, SIGNAL(clicked()),
            this, SLOT(processModelChanged()));
    connect(gouraundShadingModelRadioButton, SIGNAL(clicked()),
            this, SLOT(processModelChanged()));
    connect(phongShadingModelRadioButton, SIGNAL(clicked()),
            this, SLOT(processModelChanged()));
}

void ShadingModelSelectionWidget::processModelChanged()
{
    if(flatShadingModelRadioButton->isChecked())
    {
        emit shadingModelChangedToFlat();
    } else if (gouraundShadingModelRadioButton->isChecked())
    {
        emit shadingModelChangedToGouraud();
    } else if (phongShadingModelRadioButton->isChecked())
    {
        emit shadingModelChangedToPhong();
    }
}

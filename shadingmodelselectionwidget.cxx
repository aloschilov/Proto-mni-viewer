#include <QtGui>
#include "shadingmodelselectionwidget.h"


ShadingModelSelectionWidget::ShadingModelSelectionWidget(QWidget *parent) :
    QWidget(parent)
{
    QGroupBox* simpleShadingModelsGroupBox = new QGroupBox();
    simpleShadingModelsGroupBox->setTitle(tr("Select pre-defined shading model."));

    gouraundShadingModelRadioButton = new QRadioButton(tr("Gouraud"), simpleShadingModelsGroupBox);
    flatShadingModelRadioButton = new QRadioButton(tr("Flat"), simpleShadingModelsGroupBox);

    gouraundShadingModelRadioButton->setChecked(true);

    QVBoxLayout *simpleShadingModelsGroupBoxLayout = new QVBoxLayout();
    simpleShadingModelsGroupBoxLayout->addWidget(gouraundShadingModelRadioButton);
    simpleShadingModelsGroupBoxLayout->addWidget(flatShadingModelRadioButton);


    simpleShadingModelsGroupBox->setLayout(simpleShadingModelsGroupBoxLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(simpleShadingModelsGroupBox);
    mainLayout->addStretch();

    setLayout(mainLayout);

    connect(flatShadingModelRadioButton, SIGNAL(clicked()),
            this, SLOT(processModelChanged()));
    connect(gouraundShadingModelRadioButton, SIGNAL(clicked()),
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
    }
}

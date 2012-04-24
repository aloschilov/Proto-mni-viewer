#include <QtGui>

#include "surfaceselectionwidget.h"

SurfaceSelectionWidget::SurfaceSelectionWidget(QWidget *parent) :
    QWidget(parent)
{
    enableSurfaceSelectorCheckbox = new QCheckBox(tr("Enable &selector"));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(enableSurfaceSelectorCheckbox);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

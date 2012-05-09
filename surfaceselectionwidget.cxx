#include <QtGui>

#include "surfaceselectionwidget.h"

SurfaceSelectionWidget::SurfaceSelectionWidget(QWidget *parent) :
    QWidget(parent)
{
    enableSurfaceSelectorCheckbox = new QCheckBox(tr("Enable &selector"));

    openContour = new QPushButton(tr("Open selection"));
    saveContour = new QPushButton(tr("Save selection"));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(enableSurfaceSelectorCheckbox);
    mainLayout->addWidget(openContour);
    mainLayout->addWidget(saveContour);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

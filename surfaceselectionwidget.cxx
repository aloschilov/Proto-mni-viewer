#include <QtGui>

#include "surfaceselectionwidget.h"

SurfaceSelectionWidget::SurfaceSelectionWidget(QWidget *parent) :
    QWidget(parent)
{
    openSelectedPoints = new QPushButton(tr("Open selection"));
    saveSelectedPoints = new QPushButton(tr("Save selection"));
    clearSelection = new QPushButton(tr("Clear selection"));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(openSelectedPoints);
    mainLayout->addWidget(saveSelectedPoints);
    mainLayout->addWidget(clearSelection);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

#include <QtGui>

#include "lookuptableselectionwidget.h"

LookupTableSelectionWidget::LookupTableSelectionWidget(QWidget *parent) :
    QWidget(parent)
{
    lookupTablesButtonGroup =  new QButtonGroup(this);

    addLookupTableByImageFilename(":colormaps/fire.bmp");
    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Fire")), 0);

    addLookupTableByImageFilename(":colormaps/grayscale.bmp");
    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Grayscale")), 1);

    addLookupTableByImageFilename(":colormaps/rainbow.bmp");
    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Rainbow")),2);

    addLookupTableByImageFilename(":colormaps/royal.bmp");
    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Royal")),3);

    addLookupTableByImageFilename(":colormaps/topograph.bmp");
    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Topograph")),4);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QAbstractButton *button;
    lookupTablesButtonGroup->button(0)->setChecked(true);
    foreach (button, lookupTablesButtonGroup->buttons())
    {
        mainLayout->addWidget(button);
    }
    mainLayout->addStretch();
    setLayout(mainLayout);

    connect(lookupTablesButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(processButtonGroupButtonClicked(int)));
}

vtkSmartPointer<vtkLookupTable > LookupTableSelectionWidget::getCurrentLookupTable()
{
    return builtinLookupTables[lookupTablesButtonGroup->checkedId()];
}

void LookupTableSelectionWidget::processButtonGroupButtonClicked(int /*id*/)
{
    emit currentLookupTableChanged(builtinLookupTables[lookupTablesButtonGroup->checkedId()]);
}

void LookupTableSelectionWidget::addLookupTableByImageFilename(const QString &filename)
{
    QImage imageToPrepareScalePoints;
    imageToPrepareScalePoints.load(filename);
    int width = imageToPrepareScalePoints.width();

    vtkSmartPointer<vtkLookupTable > lookupTableToAdd = vtkSmartPointer<vtkLookupTable >::New();
    lookupTableToAdd->SetNumberOfTableValues(width);

    for(int i=0; i<width; ++i)
    {
        QColor color(imageToPrepareScalePoints.pixel(i,0));
        lookupTableToAdd->SetTableValue(i, color.redF(), color.greenF(), color.blueF());
    }

    lookupTableToAdd->Build();
    builtinLookupTables.push_back(lookupTableToAdd);
}

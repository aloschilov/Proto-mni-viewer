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

    QGroupBox *customColormapGroupBox = createCustomColormapGroupBox();

    mainLayout = new QVBoxLayout();

    QAbstractButton *button;
    lookupTablesButtonGroup->button(0)->setChecked(true);

    foreach (button, lookupTablesButtonGroup->buttons())
    {
        mainLayout->addWidget(button);
    }

    mainLayout->addWidget(customColormapGroupBox);

    mainLayout->addStretch();
    setLayout(mainLayout);

    connect(lookupTablesButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(processButtonGroupButtonClicked(int)));

    connect(specifyFileToUseAsColormap, SIGNAL(clicked()),
            this, SLOT(processSpecifyFileToUseAsColormap()));
    connect(addCustomColormapToList, SIGNAL(clicked()),
            this, SLOT(processAddCustomColormapToList()));
}

vtkSmartPointer<vtkLookupTable > LookupTableSelectionWidget::getCurrentLookupTable()
{
    return builtinLookupTables[lookupTablesButtonGroup->checkedId()];
}

void LookupTableSelectionWidget::processButtonGroupButtonClicked(int /*id*/)
{
    emit currentLookupTableChanged(builtinLookupTables[lookupTablesButtonGroup->checkedId()]);
}

void LookupTableSelectionWidget::processAddCustomColormapToList()
{
    if(!pathToColormapFileLineEdit->text().isEmpty())
    {
        addLookupTableByImageFilename(pathToColormapFileLineEdit->text());

        lookupTablesButtonGroup->addButton(new QRadioButton(pathToColormapFileLineEdit->text()),
                                                            lookupTablesButtonGroup->buttons().size());
        relayoutColormaps();
    }
}

void LookupTableSelectionWidget::processSpecifyFileToUseAsColormap()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select a file to use as colormap."),
                                                    "",
                                                    tr("Image file (*.png *.bmp *.jpg)"));
    if(fileName.isEmpty())
    {
        return;
    }

    QImage imageToTestFileContent;

    if(imageToTestFileContent.load(fileName))
    {
        pathToColormapFileLineEdit->setText(fileName);
        addCustomColormapToList->setEnabled(true);
    }
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

QGroupBox *LookupTableSelectionWidget::createCustomColormapGroupBox()
{
    QGroupBox *groupBox = new QGroupBox(tr("Custom colormap"));

    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBox->setLayout(groupBoxLayout);

    pathToColormapFileLineEdit = new QLineEdit();
    pathToColormapFileLineEdit->setReadOnly(true);

    specifyFileToUseAsColormap = new QToolButton();
    specifyFileToUseAsColormap->setIcon(QIcon(":/images/open.png"));

    addCustomColormapToList = new QToolButton();
    addCustomColormapToList->setIcon(QIcon(":/images/add.png"));
    addCustomColormapToList->setEnabled(false);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(specifyFileToUseAsColormap);
    buttonsLayout->addWidget(addCustomColormapToList);

    groupBoxLayout->addWidget(pathToColormapFileLineEdit);
    groupBoxLayout->addLayout(buttonsLayout);

    return groupBox;
}

void LookupTableSelectionWidget::relayoutColormaps()
{
    QAbstractButton *button;

    foreach (button, lookupTablesButtonGroup->buttons())
    {
        mainLayout->removeWidget(button);
    }

    int id=0;

    foreach (button, lookupTablesButtonGroup->buttons())
    {
        mainLayout->insertWidget(id, button);

        ++id;
    }
}

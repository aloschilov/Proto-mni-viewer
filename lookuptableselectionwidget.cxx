#include <string>

#include <QtGui>

#include "lookuptableselectionwidget.h"

using namespace std;

LookupTableSelectionWidget::LookupTableSelectionWidget(QWidget *parent) :
    QWidget(parent)
{
    lookupTablesButtonGroup =  new QButtonGroup(this);

    insertPoint =  0;

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
    QGroupBox *customDirectRgbGroupBox = createCustomDirectRgbGroupBox();

    showLegendCheckbox = new QCheckBox(tr("Show legend"));
    showLegendCheckbox->setChecked(true);

    mainLayout = new QVBoxLayout();

    QAbstractButton *button;
    lookupTablesButtonGroup->button(0)->setChecked(true);

    foreach (button, lookupTablesButtonGroup->buttons())
    {
        mainLayout->addWidget(button);
    }

    mainLayout->addWidget(customColormapGroupBox);
    mainLayout->addWidget(customDirectRgbGroupBox);
    mainLayout->addWidget(showLegendCheckbox);

    minValue = new QDoubleSpinBox();
    minValue->setRange(-10000, 10000);
    QHBoxLayout *minValueLayout = new QHBoxLayout;
    QLabel *minValueLabel = new QLabel(tr("Lookup table min value:"));
    minValueLayout->addWidget(minValueLabel);
    minValueLayout->addWidget(minValue);
    mainLayout->addLayout(minValueLayout);

    maxValue = new QDoubleSpinBox();
    maxValue->setRange(-10000, 10000);
    QHBoxLayout *maxValueLayout = new QHBoxLayout;
    QLabel *maxValueLabel = new QLabel(tr("Lookup table max value:"));
    maxValueLayout->addWidget(maxValueLabel);
    maxValueLayout->addWidget(maxValue);
    mainLayout->addLayout(maxValueLayout);

    resetRangeToDefaultButton = new QPushButton(tr("Reset range"));
    mainLayout->addWidget(resetRangeToDefaultButton);

    mainLayout->addStretch();
    setLayout(mainLayout);

    connect(lookupTablesButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(processButtonGroupButtonClicked(int)));

    connect(specifyFileToUseAsColormap, SIGNAL(clicked()),
            this, SLOT(openColormapFile()));
    connect(addCustomColormapToList, SIGNAL(clicked()),
            this, SLOT(processAddCustomColormapToList()));
    connect(specifyPathToRgbFile, SIGNAL(clicked()),
            this, SLOT(openPerPointRgbFile()));
    connect(addCustomDirectRgbToList, SIGNAL(clicked()),
            this, SLOT(processAddCustomDirectRgbToList()));
    connect(showLegendCheckbox, SIGNAL(stateChanged(int)),
            this, SLOT(processShowLegendStateChanged(int)));
    connect(minValue, SIGNAL(valueChanged(double)),
            this, SLOT(processMinValueChanged(double)));
    connect(maxValue, SIGNAL(valueChanged(double)),
            this, SLOT(processMaxValueChanged(double)));
    connect(resetRangeToDefaultButton, SIGNAL(clicked()),
            this, SLOT(resetRangeToDefault()));

    defaultMaxValue = 1.0;
    defaultMinValue = 0.0;
}

vtkSmartPointer<vtkLookupTable > LookupTableSelectionWidget::getCurrentLookupTable()
{
    return lookupTables[lookupTablesButtonGroup->checkedId()];
}

void LookupTableSelectionWidget::processButtonGroupButtonClicked(int /*id*/)
{
    if(lookupTables.contains(lookupTablesButtonGroup->checkedId()))
    {
        emit currentLookupTableChanged(lookupTables[lookupTablesButtonGroup->checkedId()]);

        showLegendCheckbox->setEnabled(true);

        if(showLegendCheckbox->checkState() == Qt::Checked)
        {
            emit showLegend();
        }
        else
        {
            emit hideLegend();
        }
    }

    if(perVertexColors.contains(lookupTablesButtonGroup->checkedId()))
    {
        emit currentPerVertexColorsChanged(perVertexColors[lookupTablesButtonGroup->checkedId()]);
        showLegendCheckbox->setEnabled(false);
        emit hideLegend();
    }
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

void LookupTableSelectionWidget::openColormapFile()
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

void LookupTableSelectionWidget::processAddCustomDirectRgbToList()
{
    if(!pathToRgbFileLineEdit->text().isEmpty())
    {
        addDirectRgbColors(pathToRgbFileLineEdit->text());

        lookupTablesButtonGroup->addButton(new QRadioButton(pathToRgbFileLineEdit->text()),
                                                            lookupTablesButtonGroup->buttons().size());
        relayoutColormaps();
    }
}

void LookupTableSelectionWidget::openPerPointRgbFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open colormap as per vertex RGB."),
                                                    "",
                                                    tr("RGB file(*.rgb)"));
    if(fileName.isEmpty())
    {
        return;
    }

    pathToRgbFileLineEdit->setText(fileName);
    addCustomDirectRgbToList->setEnabled(true);
}

void LookupTableSelectionWidget::savePerPointRgbFile()
{

}

void LookupTableSelectionWidget::processShowLegendStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        emit showLegend();
    }
    else
    {
        emit hideLegend();
    }
}

void LookupTableSelectionWidget::setScalarRange(double min, double max)
{
    qDebug() << "void LookupTableSelectionWidget::setScalarRange(double min, double max)";
    qDebug() << min;
    qDebug() << max;
    minValue->setValue(min);
    maxValue->setValue(max);
}

void LookupTableSelectionWidget::setDefaultRangeValues(double minDefault, double maxDefault)
{
    defaultMinValue = minDefault;
    defaultMaxValue = maxDefault;
}

void LookupTableSelectionWidget::resetRangeToDefault()
{
    minValue->setValue(defaultMinValue);
    maxValue->setValue(defaultMaxValue);
}

void LookupTableSelectionWidget::processMinValueChanged(double value)
{
    emit scalarRangeChanged(minValue->value(), maxValue->value());
}

void LookupTableSelectionWidget::processMaxValueChanged(double value)
{
    emit scalarRangeChanged(minValue->value(), maxValue->value());
}

void LookupTableSelectionWidget::addDirectRgbColors(const QString &filename)
{
    ifstream in(filename.toStdString().c_str(), ios::in);

    // Setup colors

    vtkSmartPointer<vtkUnsignedCharArray> colors =
      vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(4);
    colors->SetName ("Colors");

    while(!in.eof())
    {
        double rgba[4];

        in >> rgba[0];
        in >> rgba[1];
        in >> rgba[2];
        in >> rgba[3];

        unsigned char value[4];

        value[0] = rgba[0]*255;
        value[1] = rgba[1]*255;
        value[2] = rgba[2]*255;
        value[3] = rgba[3]*255;

        colors->InsertNextTupleValue(value);
    }

    perVertexColors[insertPoint++] = colors;
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

    lookupTables[insertPoint++] = lookupTableToAdd.GetPointer();
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

QGroupBox *LookupTableSelectionWidget::createCustomDirectRgbGroupBox()
{
    QGroupBox *groupBox = new QGroupBox(tr("Direct rgb"));

    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBox->setLayout(groupBoxLayout);

    pathToRgbFileLineEdit = new QLineEdit();
    pathToRgbFileLineEdit->setReadOnly(true);

    specifyPathToRgbFile = new QToolButton();
    specifyPathToRgbFile->setIcon(QIcon(":/images/open.png"));

    addCustomDirectRgbToList = new QToolButton();
    addCustomDirectRgbToList->setIcon(QIcon(":/images/add.png"));
    addCustomDirectRgbToList->setEnabled(false);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(specifyPathToRgbFile);
    buttonsLayout->addWidget(addCustomDirectRgbToList);

    groupBoxLayout->addWidget(pathToRgbFileLineEdit);
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

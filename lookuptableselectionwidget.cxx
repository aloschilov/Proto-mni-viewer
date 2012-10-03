#include <string>

#include <QtGui>

#include "lookuptableselectionwidget.h"
#include "gradients.h"
#include "jsoncpp/include/json/reader.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

LookupTableSelectionWidget::LookupTableSelectionWidget(QWidget *parent) :
    QWidget(parent)
{
    gradientDialog = new GradientDialog(this);

    lookupTablesButtonGroup =  new QButtonGroup(this);

    insertPoint =  0;

    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Default lookup table")), insertPoint);
    addDefaultLookupTable();
    insertPoint++;

    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Gradient")), insertPoint);
    addGradientLookupTable();
    insertPoint++;

    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Fire")), insertPoint);
    addLookupTableByImageFilename(":colormaps/fire.bmp");

    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Grayscale")), insertPoint);
    addLookupTableByImageFilename(":colormaps/grayscale.bmp");

    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Rainbow")), insertPoint);
    addLookupTableByImageFilename(":colormaps/rainbow.bmp");

    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Royal")), insertPoint);
    addLookupTableByImageFilename(":colormaps/royal.bmp");

    lookupTablesButtonGroup->addButton(new QRadioButton(tr("Topograph")), insertPoint);
    addLookupTableByImageFilename(":colormaps/topograph.bmp");

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

    chooseLookupTableAsGradientButton = new QPushButton(tr("Customize gradient"));
    saveCurrentLookupTableAsDefault = new QPushButton(tr("Save lookup table as default"));

    mainLayout->addWidget(customColormapGroupBox);
    mainLayout->addWidget(customDirectRgbGroupBox);
    mainLayout->addWidget(chooseLookupTableAsGradientButton);
    mainLayout->addWidget(saveCurrentLookupTableAsDefault);
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

    textPropertyEditor = new TextPropertyEditor(QString("legend"));

    mainLayout->addWidget(textPropertyEditor);
    
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
    connect(chooseLookupTableAsGradientButton, SIGNAL(clicked()),
            this, SLOT(chooseLookupTableAsGradientButtonClicked()));
    connect(saveCurrentLookupTableAsDefault, SIGNAL(clicked()),
            this, SLOT(saveCurrentLookupTableAsDefaultClicked()));
    connect(textPropertyEditor, SIGNAL(textPropertyChanged(vtkSmartPointer<vtkTextProperty>)),
            this, SIGNAL(currentLabelTextPropertyChanged(vtkSmartPointer<vtkTextProperty>)));

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


        pathToRgbFileLineEdit->setText("");
        addCustomDirectRgbToList->setEnabled(false);
    }
}

void LookupTableSelectionWidget::openPerPointRgbFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open colormap as per vertex RGB."),
                                                    "",
                                                    tr("RGB file(*.rgb *.txt)"));
    if(fileName.isEmpty())
    {
        pathToRgbFileLineEdit->setText("");
        addCustomDirectRgbToList->setEnabled(false);
        return;
    }

    if(validateDirectRgbColorsFile(fileName) == false)
    {
        pathToRgbFileLineEdit->setText("");
        addCustomDirectRgbToList->setEnabled(false);

        QMessageBox::warning(this, tr("MNI object viewer"),
                             tr("The format of per vertex RGB file is incorrect."),
                             QMessageBox::Ok);
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

void LookupTableSelectionWidget::chooseLookupTableAsGradientButtonClicked()
{
    if(gradientDialog->exec() == QDialog::Accepted)
    {
        addGradientLookupTable();
        processButtonGroupButtonClicked(1);
    }
}

void LookupTableSelectionWidget::saveCurrentLookupTableAsDefaultClicked()
{
    if(lookupTables.contains(lookupTablesButtonGroup->checkedId()))
    {
        int numberOfTableValues = lookupTables[lookupTablesButtonGroup->checkedId()]->GetNumberOfTableValues();

        QVector<double> allInRawColorComponents;

        ostringstream out;

        for(int i; i < numberOfTableValues; ++i)
        {
            double r = lookupTables[lookupTablesButtonGroup->checkedId()]->GetTableValue(i)[0];
            double g = lookupTables[lookupTablesButtonGroup->checkedId()]->GetTableValue(i)[1];
            double b = lookupTables[lookupTablesButtonGroup->checkedId()]->GetTableValue(i)[2];
            out << r << " " << g << " " << b << " ";
        }

        settings.setValue("default_lookup_table", QString::fromStdString(out.str()));

        addDefaultLookupTable();
    }
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

bool LookupTableSelectionWidget::validateDirectRgbColorsFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QRegExp rx("([-+]?(\\d+(\\.\\d*)?|\\.\\d+)([eE][-+]?\\d+)?)\\s+"
               "([-+]?(\\d+(\\.\\d*)?|\\.\\d+)([eE][-+]?\\d+)?)\\s+"
               "([-+]?(\\d+(\\.\\d*)?|\\.\\d+)([eE][-+]?\\d+)?)\\s+"
               "([-+]?(\\d+(\\.\\d*)?|\\.\\d+)([eE][-+]?\\d+)?)\\s*");

    while (!file.atEnd())
    {
        QByteArray line = file.readLine();

        if (rx.exactMatch(line) == false)
        {
            return false;
        }
    }
    return true;
}

void LookupTableSelectionWidget::addDefaultLookupTable()
{
    if(settings.allKeys().contains("default_lookup_table"))
    {
        string s = settings.value("default_lookup_table").toString().toStdString();
        istringstream i(s);
        QVector<double> allInRowColorComponents;

        double x;

        while(i >> x)
        {
            allInRowColorComponents.append(x);
        }

        int width = allInRowColorComponents.size()/3;

        vtkSmartPointer<vtkLookupTable > lookupTableToAdd = vtkSmartPointer<vtkLookupTable >::New();
        lookupTableToAdd->SetNumberOfTableValues(width);

        for(int i=0; i<width; ++i)
        {
            lookupTableToAdd->SetTableValue(i, allInRowColorComponents[i*3+0], allInRowColorComponents[i*3+1], allInRowColorComponents[i*3+2]);
        }

        lookupTableToAdd->Build();

        lookupTables[0] = lookupTableToAdd.GetPointer();

    } else {
        vtkSmartPointer<vtkLookupTable > lookupTableToAdd = vtkSmartPointer<vtkLookupTable >::New();
        lookupTableToAdd->SetNumberOfTableValues(2);
        lookupTableToAdd->SetTableValue(0, 1.0, 1.0, 1.0);
        lookupTableToAdd->SetTableValue(1, 1.0, 1.0, 1.0);

        lookupTableToAdd->Build();

        lookupTables[0] = lookupTableToAdd.GetPointer();
    }
}

QVariantList LookupTableSelectionWidget::toVariantList(const QVector<double> &vectorToConvert)
{
    QVariantList listToReturn;

    foreach (const double &intValue, vectorToConvert)
    {
        listToReturn << QVariant(intValue);
    }

    return listToReturn;
}

QVector<double> LookupTableSelectionWidget::fromVariantList(const QVariantList &listToConvert)
{
    QVector<double> vectorToReturn;

    int sizeOfVariantList = listToConvert.size();

    for(int i=0; i<sizeOfVariantList; ++i)
    {
        vectorToReturn.append(listToConvert.at(i).toDouble());
    }

    return vectorToReturn;
}

void LookupTableSelectionWidget::addGradientLookupTable()
{
    QImage imageToPrepareScalePoints = gradientDialog->getGradientImage();
    int width = imageToPrepareScalePoints.width();

    vtkSmartPointer<vtkLookupTable > lookupTableToAdd = vtkSmartPointer<vtkLookupTable >::New();
    lookupTableToAdd->SetNumberOfTableValues(width);

    for(int i=0; i<width; ++i)
    {
        QColor color(imageToPrepareScalePoints.pixel(i,0));
        lookupTableToAdd->SetTableValue(i, color.redF(), color.greenF(), color.blueF());
    }

    lookupTableToAdd->Build();

    lookupTables[1] = lookupTableToAdd.GetPointer();
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
    specifyFileToUseAsColormap->setFixedSize(24, 24);

    addCustomColormapToList = new QToolButton();
    addCustomColormapToList->setIcon(QIcon(":/images/add.png"));
    addCustomColormapToList->setFixedSize(24, 24);
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
    specifyPathToRgbFile->setFixedSize(24, 24);

    addCustomDirectRgbToList = new QToolButton();
    addCustomDirectRgbToList->setIcon(QIcon(":/images/add.png"));
    addCustomDirectRgbToList->setEnabled(false);
    addCustomDirectRgbToList->setFixedSize(24, 24);

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

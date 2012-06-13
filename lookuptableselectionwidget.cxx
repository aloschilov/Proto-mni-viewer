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

    mainLayout = new QVBoxLayout();

    QAbstractButton *button;
    lookupTablesButtonGroup->button(0)->setChecked(true);

    foreach (button, lookupTablesButtonGroup->buttons())
    {
        mainLayout->addWidget(button);
    }

    mainLayout->addWidget(customColormapGroupBox);
    mainLayout->addWidget(customDirectRgbGroupBox);

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
    }

    if(perVertexColors.contains(lookupTablesButtonGroup->checkedId()))
    {
        qDebug() << "emit currentPerVertexColorsChanged(perVertexColors[lookupTablesButtonGroup->checkedId()]);";
        emit currentPerVertexColorsChanged(perVertexColors[lookupTablesButtonGroup->checkedId()]);
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

void LookupTableSelectionWidget::addDirectRgbColors(const QString &filename)
{
    ifstream in(filename.toStdString().c_str(), ios::in);

    // Setup colors

    vtkSmartPointer<vtkUnsignedCharArray> colors =
      vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName ("Colors");

    while(!in.eof())
    {
        int rgb[3];

        in >> rgb[0];
        in >> rgb[1];
        in >> rgb[2];

        unsigned char value[3];

        value[0] = rgb[0];
        value[1] = rgb[1];
        value[2] = rgb[2];

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

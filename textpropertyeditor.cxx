#include <QtGui>

#include "textpropertyeditor.h"


TextPropertyEditor::TextPropertyEditor(QString _settingsPrefix, QWidget *parent) :
    settingsPrefix(_settingsPrefix)
{
    configuredTextProperty = vtkSmartPointer<vtkTextProperty>::New();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(createFontFamilyGroupBox());
    mainLayout->addWidget(createTypefaceGroupBox());
    mainLayout->addWidget(createFontSizeGroupBox());
    mainLayout->addWidget(createFontColorGroupBox());

    readSettings();

    connect(fontFamilyComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(processAnyChangeOfTheTextParameters()));
    connect(italicCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(processAnyChangeOfTheTextParameters()));
    connect(boldCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(processAnyChangeOfTheTextParameters()));
    connect(shadowCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(processAnyChangeOfTheTextParameters()));
    connect(fontSizeSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(processAnyChangeOfTheTextParameters()));
    connect(selectColorToolButton, SIGNAL(clicked()),
            this, SLOT(processSelectColor()));

    setLayout(mainLayout);
}

void TextPropertyEditor::processSelectColor()
{
    QColor color;
    color = QColorDialog::getColor(Qt::black, this);

    if(color.isValid())
    {
        selectColorToolButton->setIcon(getIconFilledWithColor(color));
        fontColor = color;
        configuredTextProperty->SetColor(fontColor.redF(), fontColor.greenF(), fontColor.blueF());
        emit textPropertyChanged(configuredTextProperty);
        writeSettings();
    }
}

void TextPropertyEditor::processAnyChangeOfTheTextParameters()
{
    switch(fontFamilyComboBox->currentIndex())
    {
    case 0:
        configuredTextProperty->SetFontFamilyToArial();
        break;
    case 1:
        configuredTextProperty->SetFontFamilyToCourier();
        break;
    case 2:
        configuredTextProperty->SetFontFamilyToTimes();
        break;
    }

    if(italicCheckBox->checkState() == Qt::Checked)
    {
        configuredTextProperty->ItalicOn();
    }
    else
    {
        configuredTextProperty->ItalicOff();
    }

    if(boldCheckBox->checkState() == Qt::Checked)
    {
        configuredTextProperty->BoldOn();
    }
    else
    {
        configuredTextProperty->BoldOff();
    }

    if(shadowCheckBox->checkState() == Qt::Checked)
    {
        configuredTextProperty->ShadowOn();
    }
    else
    {
        configuredTextProperty->ShadowOff();
    }

    configuredTextProperty->SetFontSize(fontSizeSpinBox->value());

    emit textPropertyChanged(configuredTextProperty);
    writeSettings();
}

QIcon TextPropertyEditor::getIconFilledWithColor(QColor color)
{
    QPixmap pixmap(24, 24);
    pixmap.fill(color);
    return QIcon(pixmap);
}

QGroupBox *TextPropertyEditor::createFontFamilyGroupBox()
{
    QGroupBox *groupBox = new QGroupBox();
    groupBox->setTitle(tr("Font family"));

    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBox->setLayout(groupBoxLayout);

    fontFamilyComboBox = new QComboBox();
    fontFamilyComboBox->addItem("Arial");
    fontFamilyComboBox->addItem("Courier");
    fontFamilyComboBox->addItem("Times");
    groupBoxLayout->addWidget(fontFamilyComboBox);

    return groupBox;
}

QGroupBox *TextPropertyEditor::createTypefaceGroupBox()
{
    QGroupBox *groupBox = new QGroupBox();
    groupBox->setTitle(tr("Typeface"));

    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBox->setLayout(groupBoxLayout);

    italicCheckBox = new QCheckBox("Italic");
    boldCheckBox = new QCheckBox("Bold");
    shadowCheckBox = new QCheckBox("Text shadow");
    shadowCheckBox->setVisible(false);

    groupBoxLayout->addWidget(italicCheckBox);
    groupBoxLayout->addWidget(boldCheckBox);
    groupBoxLayout->addWidget(shadowCheckBox);

    return groupBox;
}

QGroupBox *TextPropertyEditor::createFontSizeGroupBox()
{
    QGroupBox *groupBox = new QGroupBox();
    groupBox->setVisible(false);
    groupBox->setTitle(tr("Size"));

    fontSizeSpinBox = new QSpinBox();

    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBox->setLayout(groupBoxLayout);
    groupBoxLayout->addWidget(fontSizeSpinBox);

    return groupBox;
}

QGroupBox *TextPropertyEditor::createFontColorGroupBox()
{
    QGroupBox *groupBox = new QGroupBox();
    groupBox->setTitle(tr("Color"));

    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBox->setLayout(groupBoxLayout);

    selectColorToolButton = new QToolButton();
    groupBoxLayout->addWidget(selectColorToolButton);

    return groupBox;
}

void TextPropertyEditor::readSettings()
{
    QString fontFamily = settings.value(settingsPrefix + "/fontFamily", QString("Arial")).toString();
    if(fontFamily == "Arial")
    {
        fontFamilyComboBox->setCurrentIndex(0);
    } else if(fontFamily == "Courier")
    {
        fontFamilyComboBox->setCurrentIndex(1);
    } else if(fontFamily == "Times")
    {
        fontFamilyComboBox->setCurrentIndex(2);
    }

    bool italic = settings.value(settingsPrefix + "/italic", false).toBool();
    italicCheckBox->setChecked(italic);

    bool bold = settings.value(settingsPrefix + "/bold", false).toBool();
    boldCheckBox->setChecked(bold);

    bool shadow = settings.value(settingsPrefix + "/shadow", false).toBool();
    shadowCheckBox->setChecked(shadow);

    QColor color = settings.value(settingsPrefix + "/color", QColor::fromRgbF(1.0, 1.0, 1.0)).value<QColor>();

    selectColorToolButton->setIcon(getIconFilledWithColor(color));
    fontColor = color;
    configuredTextProperty->SetColor(fontColor.redF(), fontColor.greenF(), fontColor.blueF());

    processAnyChangeOfTheTextParameters();
}

void TextPropertyEditor::writeSettings()
{
    settings.setValue(settingsPrefix + "/fontFamily", configuredTextProperty->GetFontFamilyAsString());
    settings.setValue(settingsPrefix + "/italic", bool(configuredTextProperty->GetItalic()));
    settings.setValue(settingsPrefix + "/bold", bool(configuredTextProperty->GetBold()));
    settings.setValue(settingsPrefix + "/shadow", bool(configuredTextProperty->GetShadow()));
    settings.setValue(settingsPrefix + "/color", fontColor);
}

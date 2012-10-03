#ifndef TEXTPROPERTYEDITOR_H
#define TEXTPROPERTYEDITOR_H

#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QToolButton>
#include <QSettings>

QT_FORWARD_DECLARE_CLASS(QGroupBox)



class TextPropertyEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TextPropertyEditor(QString _settingsPrefix, QWidget *parent = 0);
    vtkSmartPointer<vtkTextProperty> getTextProperty() {return configuredTextProperty;}

signals:
    void textPropertyChanged(vtkSmartPointer<vtkTextProperty> textProperty);

private slots:
    void processSelectColor();
    void processAnyChangeOfTheTextParameters();
    
private:
    
    QIcon getIconFilledWithColor(QColor color);
    QGroupBox *createFontFamilyGroupBox();
    QGroupBox *createTypefaceGroupBox();
    QGroupBox *createFontSizeGroupBox();
    QGroupBox *createFontColorGroupBox();

    void readSettings();
    void writeSettings();
      
    vtkSmartPointer<vtkTextProperty> configuredTextProperty;

    QComboBox *fontFamilyComboBox;
    QCheckBox *italicCheckBox;
    QCheckBox *boldCheckBox;
    QCheckBox *shadowCheckBox;

    QSpinBox *fontSizeSpinBox;
    QToolButton *selectColorToolButton;

    QColor fontColor;

    QSettings settings;
    QString settingsPrefix;
};

#endif // TEXTPROPERTYEDITOR_H

#ifndef LOOKUPTABLESELECTIONWIDGET_H
#define LOOKUPTABLESELECTIONWIDGET_H

#include <QWidget>
#include <QMap>
#include <QtCore>

#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkTextProperty.h>

#include "textpropertyeditor.h"

#include <vector>

QT_FORWARD_DECLARE_CLASS(QButtonGroup)
QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(QVBoxLayout)
QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QDoubleSpinBox)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(GradientDialog)



class LookupTableSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LookupTableSelectionWidget(QWidget *parent = 0);

    vtkSmartPointer<vtkLookupTable > getCurrentLookupTable();
    vtkSmartPointer<vtkTextProperty> getCurrentLabelTextProperty() {return textPropertyEditor->getTextProperty();}

signals:
    void currentLookupTableChanged(vtkSmartPointer<vtkLookupTable > lookupTable);
    void currentPerVertexColorsChanged(vtkSmartPointer<vtkUnsignedCharArray> colors);
    void showLegend();
    void hideLegend();
    void scalarRangeChanged(double min, double max);
    void currentLabelTextPropertyChanged(vtkSmartPointer<vtkTextProperty>);
    void numberOfLabelsChanged(int numberOfLabels);

public slots:

    void processButtonGroupButtonClicked(int id);

    void processAddCustomColormapToList();
    void openColormapFile();

    void processAddCustomDirectRgbToList();
    void openPerPointRgbFile();
    void savePerPointRgbFile();

    void processShowLegendStateChanged(int state);
    void setScalarRange(double min, double max);
    void setDefaultRangeValues(double minDefault, double maxDefault);
    void resetRangeToDefault();

private slots:
    void processMinValueChanged(double value);
    void processMaxValueChanged(double value);
    void chooseLookupTableAsGradientButtonClicked();
    void saveCurrentLookupTableAsDefaultClicked();

private:
    void addDefaultLookupTable();
    void addGradientLookupTable();
    void addLookupTableByImageFilename(const QString &filename);
    void addDirectRgbColors(const QString &filename);

    QVariantList toVariantList(const QVector<double> &vectorToConvert);
    QVector<double> fromVariantList(const QVariantList &listToConvert);

    bool validateDirectRgbColorsFile(const QString &filename);

    QGroupBox *createCustomColormapGroupBox();
    QGroupBox *createCustomDirectRgbGroupBox();
    QGroupBox *createScalarsGroupBox();

    void relayoutColormaps();

    QMap<int, vtkSmartPointer<vtkLookupTable> > lookupTables;
    QMap<int, vtkSmartPointer<vtkUnsignedCharArray> > perVertexColors;

    int insertPoint;

    QButtonGroup *lookupTablesButtonGroup;

    QLineEdit *pathToColormapFileLineEdit;
    QToolButton *specifyFileToUseAsColormap;
    QToolButton *addCustomColormapToList;

    QLineEdit *pathToRgbFileLineEdit;
    QToolButton *specifyPathToRgbFile;
    QToolButton *addCustomDirectRgbToList;

    QLineEdit *pathToScalarsLineEdit;
    QToolButton *specifyPathToScalars;

    QPushButton *chooseLookupTableAsGradientButton;
    QPushButton *saveCurrentLookupTableAsDefault;

    QDoubleSpinBox *minValue;
    QDoubleSpinBox *maxValue;

    QSpinBox *numberOfLabelsSpinBox;

    QCheckBox *showLegendCheckbox;

    QVBoxLayout *mainLayout;

    double defaultMinValue;
    double defaultMaxValue;

    QPushButton *resetRangeToDefaultButton;

    GradientDialog *gradientDialog;
    TextPropertyEditor *textPropertyEditor;

    QSettings settings;
};

#endif // LOOKUPTABLESELECTIONWIDGET_H

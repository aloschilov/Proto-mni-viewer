#ifndef LOOKUPTABLESELECTIONWIDGET_H
#define LOOKUPTABLESELECTIONWIDGET_H

#include <QWidget>
#include <QMap>

#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>

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

signals:
    void currentLookupTableChanged(vtkSmartPointer<vtkLookupTable > lookupTable);
    void currentPerVertexColorsChanged(vtkSmartPointer<vtkUnsignedCharArray> colors);
    void showLegend();
    void hideLegend();
    void scalarRangeChanged(double min, double max);

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

private:
    void addGradientLookupTable();
    void addLookupTableByImageFilename(const QString &filename);
    void addDirectRgbColors(const QString &filename);

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

    QDoubleSpinBox *minValue;
    QDoubleSpinBox *maxValue;

    QCheckBox *showLegendCheckbox;

    QVBoxLayout *mainLayout;

    double defaultMinValue;
    double defaultMaxValue;

    QPushButton *resetRangeToDefaultButton;

    GradientDialog *gradientDialog;
};

#endif // LOOKUPTABLESELECTIONWIDGET_H

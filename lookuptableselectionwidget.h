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



class LookupTableSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LookupTableSelectionWidget(QWidget *parent = 0);

    vtkSmartPointer<vtkLookupTable > getCurrentLookupTable();

signals:
    void currentLookupTableChanged(vtkSmartPointer<vtkLookupTable > lookupTable);
    void currentPerVertexColorsChanged(vtkSmartPointer<vtkUnsignedCharArray> colors);

public slots:

    void processButtonGroupButtonClicked(int id);

    void processAddCustomColormapToList();
    void openColormapFile();

    void processAddCustomDirectRgbToList();
    void openPerPointRgbFile();
    void savePerPointRgbFile();

private:
    void addLookupTableByImageFilename(const QString &filename);
    void addDirectRgbColors(const QString &filename);

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

    QVBoxLayout *mainLayout;
};

#endif // LOOKUPTABLESELECTIONWIDGET_H

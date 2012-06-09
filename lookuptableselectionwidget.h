#ifndef LOOKUPTABLESELECTIONWIDGET_H
#define LOOKUPTABLESELECTIONWIDGET_H

#include <QWidget>

#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>

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

public slots:

    void processButtonGroupButtonClicked(int id);
    void processAddCustomColormapToList();
    void processSpecifyFileToUseAsColormap();

private:
    void addLookupTableByImageFilename(const QString &filename);

    QGroupBox *createCustomColormapGroupBox();
    void relayoutColormaps();

    std::vector<vtkSmartPointer<vtkLookupTable > > builtinLookupTables;
    QButtonGroup *lookupTablesButtonGroup;

    QLineEdit *pathToColormapFileLineEdit;
    QToolButton *specifyFileToUseAsColormap;
    QToolButton *addCustomColormapToList;

    QVBoxLayout *mainLayout;
};

#endif // LOOKUPTABLESELECTIONWIDGET_H

#ifndef LOOKUPTABLESELECTIONWIDGET_H
#define LOOKUPTABLESELECTIONWIDGET_H

#include <QWidget>
#include <vector>
#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>

class QButtonGroup;

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

private:
    void addLookupTableByImageFilename(const QString &filename);

    std::vector<vtkSmartPointer<vtkLookupTable > > builtinLookupTables;
    QButtonGroup *lookupTablesButtonGroup;
};

#endif // LOOKUPTABLESELECTIONWIDGET_H

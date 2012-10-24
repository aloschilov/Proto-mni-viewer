#include <QtGui>

#include <math.h>

#include "mainwindow.h"

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkVectorText.h>
#include <vtkMNIObjectReader.h>
#include <QVTKWidget.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPolyDataCollection.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkPolyDataReader.h>

#include <vtkPolyDataWriter.h>
#include <vtkPNGWriter.h>

#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>
#include <vtkDepthSortPolyData.h>

// vtkWidgets

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>


#include "lookuptableselectionwidget.h"
#include "shadingmodelselectionwidget.h"
#include "lightingpropertieswidget.h"
#include "surfaceselectionwidget.h"
#include "animationmanagementwidget.h"
#include "textpropertyeditor.h"


#define VTK_CREATE(type, name) \
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

using namespace std;

double string_to_double( const std::string& s ) {
    istringstream i(s);
    double x;
    if (!(i >> x))
        return 0;

    return x;
}

/**
 * Generate a bunch of overlapping spheres within one poly data set:
 * one big sphere evenly surrounded by four small spheres that intersect the
 * centered sphere.
 * @param theta sphere sampling resolution (THETA)
 * @param phi sphere sampling resolution (PHI)
 * @return the set of spheres within one logical poly data set
 **/
vtkSmartPointer<vtkAppendPolyData> GenerateOverlappingBunchOfSpheres(int theta,
                                                                     int phi)
{
    vtkSmartPointer<vtkAppendPolyData> appendData =
            vtkSmartPointer<vtkAppendPolyData>::New();

    for (int i = 0; i < 5; i++)
    {
        vtkSmartPointer<vtkSphereSource> sphereSource =
                vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetThetaResolution(theta);
        sphereSource->SetPhiResolution(phi);
        sphereSource->SetRadius(0.5); // all spheres except the center
        // one should have radius = 0.5
        switch (i)
        {
        case 0:
            sphereSource->SetRadius(1);
            sphereSource->SetCenter(0, 0, 0); break;
        case 1:
            sphereSource->SetCenter(1, 0, 0); break;
        case 2:
            sphereSource->SetCenter(-1, 0, 0); break;
        case 3:
            sphereSource->SetCenter(0, 1, 0); break;
        case 4:
            sphereSource->SetCenter(0, -1, 0); break;
        }
        sphereSource->Update();
        appendData->AddInputConnection(sphereSource->GetOutputPort());
    }

    return appendData;
}

/**
 * Setup the rendering environment for depth peeling (general depth peeling
 * support is requested).
 * @see IsDepthPeelingSupported()
 * @param renderWindow a valid openGL-supporting render window
 * @param renderer a valid renderer instance
 * @param maxNoOfPeels maximum number of depth peels (multi-pass rendering)
 * @param occulusionRation the occlusion ration (0.0 means a perfect image,
 * >0.0 means a non-perfect image which in general results in faster rendering)
 * @return TRUE if depth peeling could be set up
 */
bool SetupEnvironmentForDepthPeeling(
  vtkSmartPointer<vtkRenderWindow> renderWindow,
  vtkSmartPointer<vtkRenderer> renderer, int maxNoOfPeels,
  double occlusionRatio)
{
  if (!renderWindow || !renderer)
    return false;

  // 1. Use a render window with alpha bits (as initial value is 0 (false)):
  renderWindow->SetAlphaBitPlanes(true);

  // 2. Force to not pick a framebuffer with a multisample buffer
  // (as initial value is 8):
  renderWindow->SetMultiSamples(0);

  // 3. Choose to use depth peeling (if supported) (initial value is 0 (false)):
  renderer->SetUseDepthPeeling(true);

  // 4. Set depth peeling parameters
  // - Set the maximum number of rendering passes (initial value is 4):
  renderer->SetMaximumNumberOfPeels(maxNoOfPeels);
  // - Set the occlusion ratio (initial value is 0.0, exact image):
  renderer->SetOcclusionRatio(occlusionRatio);

  return true;
}


/**
 * Unsetup the rendering environment for depth peeling (general depth peeling
 * support is requested).
 * @see IsDepthPeelingSupported()
 * @param renderWindow a valid openGL-supporting render window
 * @param renderer a valid renderer instance
 * @param maxNoOfPeels maximum number of depth peels (multi-pass rendering)
 * @param occulusionRation the occlusion ration (0.0 means a perfect image,
 * >0.0 means a non-perfect image which in general results in faster rendering)
 * @return TRUE if depth peeling could be set up
 */
bool UnsetupEnvironmentForDepthPeeling(
  vtkSmartPointer<vtkRenderWindow> renderWindow,
  vtkSmartPointer<vtkRenderer> renderer)
{
  if (!renderWindow || !renderer)
    return false;

  renderWindow->SetAlphaBitPlanes(false);
  renderWindow->SetMultiSamples(8);
  renderer->SetUseDepthPeeling(false);
  renderer->SetMaximumNumberOfPeels(4);
  renderer->SetOcclusionRatio(0.0);

  return true;
}

/**
 * Find out whether this box supports depth peeling. Depth peeling requires
 * a variety of openGL extensions and appropriate drivers.
 * @param renderWindow a valid openGL-supporting render window
 * @param renderer a valid renderer instance
 * @param doItOffscreen do the test off screen which means that nothing is
 * rendered to screen (this requires the box to support off screen rendering)
 * @return TRUE if depth peeling is supported, FALSE otherwise (which means
 * that another strategy must be used for correct rendering of translucent * geometry, e.g. CPU-based depth sorting)
 */
bool IsDepthPeelingSupported(vtkSmartPointer<vtkRenderWindow> renderWindow,
                             vtkSmartPointer<vtkRenderer> renderer,
                             bool doItOffScreen)
{
  if (!renderWindow || !renderer)
    {
    return false;
    }

  bool success = true;

  // Save original renderer / render window state
  bool origOffScreenRendering = renderWindow->GetOffScreenRendering() == 1;
  bool origAlphaBitPlanes = renderWindow->GetAlphaBitPlanes() == 1;
  int origMultiSamples = renderWindow->GetMultiSamples();
  bool origUseDepthPeeling = renderer->GetUseDepthPeeling() == 1;
  int origMaxPeels = renderer->GetMaximumNumberOfPeels();
  double origOcclusionRatio = renderer->GetOcclusionRatio();

  // Activate off screen rendering on demand
  renderWindow->SetOffScreenRendering(doItOffScreen);

  // Setup environment for depth peeling (with some default parametrization)
  success = success && SetupEnvironmentForDepthPeeling(renderWindow, renderer,
                                                       100, 0.1);

  // Do a test render
  renderWindow->Render();

  // Check whether depth peeling was used
  success = success && renderer->GetLastRenderingUsedDepthPeeling();

  // recover original state
  renderWindow->SetOffScreenRendering(origOffScreenRendering);
  renderWindow->SetAlphaBitPlanes(origAlphaBitPlanes);
  renderWindow->SetMultiSamples(origMultiSamples);
  renderer->SetUseDepthPeeling(origUseDepthPeeling);
  renderer->SetMaximumNumberOfPeels(origMaxPeels);
  renderer->SetOcclusionRatio(origOcclusionRatio);

  return success;
}

bool getDepthPeelingSupported ()
{
    int theta = 100;
    int phi = 100;
    int maxPeels = 50;
    double occulusionRatio = 0.1;

    // Generate a translucent sphere poly data set that partially overlaps:
    vtkSmartPointer<vtkAppendPolyData> translucentGeometry =
            GenerateOverlappingBunchOfSpheres(theta, phi);

    // generate a basic Mapper and Actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(translucentGeometry->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.5); // translucent !!!
    actor->GetProperty()->SetColor(1, 0, 0);
    actor->RotateX(-72); // put the objects in a position where it is easy to see
    // different overlapping regions

    // Create the RenderWindow, Renderer and RenderWindowInteractor
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the renderer, set the background and size
    renderer->AddActor(actor);
    renderer->SetBackground(1, 1, 1);
    renderWindow->SetSize(600, 400);

    // Setup view geometry
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Zoom(2.2); // so the object is larger

    // Answer the key question: Does this box support GPU Depth Peeling?
    bool useDepthPeeling = IsDepthPeelingSupported(renderWindow, renderer, true);
    return useDepthPeeling;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    leftMouseButtonIsPressed = false;
    isInPaintMode = false;

    setWindowTitle(tr("MNI object viewer"));

    qvtkWidget = new QVTKWidget();

    lookupTableSelectionWidget = new LookupTableSelectionWidget();
    lookupTableSelectionDockWidget = new QDockWidget(tr("Lookup table properties"), this);
    QScrollArea *lookupTableSelectionScrollArea = new QScrollArea;
    lookupTableSelectionScrollArea->setWidget(lookupTableSelectionWidget);
    lookupTableSelectionScrollArea->setWidgetResizable(true);
    lookupTableSelectionDockWidget->setWidget(lookupTableSelectionScrollArea);
    lookupTableSelectionDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    lookupTableSelectionDockWidget->setVisible(true);
    addDockWidget(Qt::RightDockWidgetArea, lookupTableSelectionDockWidget, Qt::Horizontal);

    shadingModelSelectionWidget = new ShadingModelSelectionWidget();
    shadingModelSelectionDockWidget = new QDockWidget(tr("Shading model properties"), this);
    QScrollArea *shadingModelSelectionScrollArea = new QScrollArea;
    shadingModelSelectionScrollArea->setWidget(shadingModelSelectionWidget);
    shadingModelSelectionDockWidget->setWidget(shadingModelSelectionScrollArea);
    shadingModelSelectionDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    shadingModelSelectionDockWidget->setVisible(true);
    addDockWidget(Qt::RightDockWidgetArea, shadingModelSelectionDockWidget, Qt::Horizontal);

    lightingPropertiesWidget = new LightingPropertiesWidget();
    lightingPropertiesDockWidget = new QDockWidget(tr("Lighting properties"), this);
    QScrollArea *lightingPropertiesWidgetScrollArea = new QScrollArea;
    lightingPropertiesWidgetScrollArea->setWidget(lightingPropertiesWidget);
    lightingPropertiesDockWidget->setWidget(lightingPropertiesWidgetScrollArea);
    lightingPropertiesDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    lightingPropertiesDockWidget->setVisible(true);
    addDockWidget(Qt::RightDockWidgetArea, lightingPropertiesDockWidget, Qt::Horizontal);


    surfaceSelectionWidget = new SurfaceSelectionWidget();
    surfaceSelectionDockWidget = new QDockWidget(tr("Surface selection"), this);
    QScrollArea *surfaceSelectionWidgetScrollArea = new QScrollArea;
    surfaceSelectionWidgetScrollArea->setWidget(surfaceSelectionWidget);
    surfaceSelectionDockWidget->setWidget(surfaceSelectionWidgetScrollArea);
    surfaceSelectionDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, surfaceSelectionDockWidget, Qt::Horizontal);


    animationManagementWidget = new AnimationManagementWidget();
    animationManagementDockWidget = new QDockWidget(tr("Animation"), this);
    QScrollArea *animationManagementWidgetScrollArea = new QScrollArea;
    animationManagementWidgetScrollArea->setWidget(animationManagementWidget);
    animationManagementDockWidget->setWidget(animationManagementWidgetScrollArea);
    animationManagementDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, animationManagementDockWidget, Qt::Horizontal);

    setCentralWidget(qvtkWidget);

    createActions();
    createMenu();
    createToolbar();
    initializeVtk();
    initializeStateMachine();

    connect(lookupTableSelectionWidget, SIGNAL(currentLookupTableChanged(vtkSmartPointer<vtkLookupTable>)),
            this, SLOT(updateLookupTable(vtkSmartPointer<vtkLookupTable>)));
    connect(lookupTableSelectionWidget, SIGNAL(currentPerVertexColorsChanged(vtkSmartPointer<vtkUnsignedCharArray> )),
            this, SLOT(updateDirectRgbColors(vtkSmartPointer<vtkUnsignedCharArray> )));
    connect(lookupTableSelectionWidget, SIGNAL(showLegend()),
            this, SLOT(showLegend()));
    connect(lookupTableSelectionWidget, SIGNAL(hideLegend()),
            this, SLOT(hideLegend()));
    connect(lookupTableSelectionWidget, SIGNAL(numberOfLabelsChanged(int)),
            this, SLOT(processNumberOfLabelsChanged(int)));

    connect(shadingModelSelectionWidget, SIGNAL(shadingModelChangedToFlat()),
            this, SLOT(setFlatShadingModel()));
    connect(shadingModelSelectionWidget, SIGNAL(shadingModelChangedToGouraud()),
            this, SLOT(setGouraudShadingModel()));
    connect(lightingPropertiesWidget->enableLightingCheckbox, SIGNAL(stateChanged(int)),
            this, SLOT(processLightingStateChanged(int)));
    connect(lightingPropertiesWidget->ambientDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processAmbientChanged(double)));
    connect(lightingPropertiesWidget->diffuseDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processDiffuseChanged(double)));
    connect(lightingPropertiesWidget->specularDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processSpecularChanged(double)));
    connect(lightingPropertiesWidget->opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processOpacityChanged(double)));
    connect(lightingPropertiesWidget->lightingWidgetVisibilityCheckbox, SIGNAL(stateChanged(int)),
            this, SLOT(processLightingWidgetStateChanged(int)));
    connect(lightingPropertiesWidget->enableDepthSortingCheckbox, SIGNAL(stateChanged(int)),
            this, SLOT(processDepthSortingCheckstateChanged(int)));
    //    connect(surfaceSelectionWidget->enableSurfaceSelectorCheckbox, SIGNAL(stateChanged(int)),
    //            this, SLOT(processSurfaceSelectorStateChanged(int)));
    connect(surfaceSelectionWidget->openSelectedPoints, SIGNAL(clicked()),
            this, SLOT(openSelection()));
    connect(surfaceSelectionWidget->saveSelectedPoints, SIGNAL(clicked()),
            this, SLOT(saveSelection()));
    connect(surfaceSelectionWidget->clearSelection, SIGNAL(clicked()),
            this, SLOT(clearSelection()));

    connect(animationManagementWidget->saveCurrentStateAsStartPointButton, SIGNAL(clicked()),
            this, SLOT(saveObjectStateAsFirstAnimationPoint()));
    connect(animationManagementWidget->saveCurrentStateAsEndPointButton, SIGNAL(clicked()),
            this, SLOT(saveObjectStateAsSecondAnimationPoint()));
    connect(animationManagementWidget, SIGNAL(currentTimeChanged()),
            this, SLOT(processCurrentTimeChanged()));

    connect(animationManagementWidget, SIGNAL(writingToAviInitiated()),
            this, SLOT(processWritingToAviInitiated()), Qt::DirectConnection);
    connect(animationManagementWidget, SIGNAL(currentAviFilenameChanged(QString)),
            this, SLOT(processWritingAviFilenameChanged(QString)));
    connect(animationManagementWidget, SIGNAL(currentWritingFrameChanged()),
            this, SLOT(processCurrentWritingFrameChanged()));
    connect(animationManagementWidget, SIGNAL(writingToAviCompleted()),
            this, SLOT(processWritingToAviCompleted()));
    connect(animationManagementWidget, SIGNAL(currentPngFilenameChanged(QString)),
            this, SLOT(processWritingPngFilenameChanged(QString)));
    connect(animationManagementWidget, SIGNAL(pngWritingRequested()),
            this, SLOT(processWritePng()));

    connect(lookupTableSelectionWidget, SIGNAL(scalarRangeChanged(double, double)),
            this, SLOT(processScalarRangeChanged(double,double)));

    connect(animationManagementWidget, SIGNAL(tranformationResetionRequested()),
            this, SLOT(processTransformationResetion()));

    connect(lookupTableSelectionWidget, SIGNAL(currentLabelTextPropertyChanged(vtkSmartPointer<vtkTextProperty>)),
            this, SLOT(updateScalarBarActorLabelTextProperty(vtkSmartPointer<vtkTextProperty>)));

    updateScalarBarActorLabelTextProperty(lookupTableSelectionWidget->getCurrentLabelTextProperty());

    QColor backroundColor = settings.value("backgroundColor", QColor(Qt::black) ).value<QColor>();
    ren->SetBackground(backroundColor.redF(), backroundColor.greenF(), backroundColor.blueF());
}

void MainWindow::initializeVtk()
{
    vtkObject::GlobalWarningDisplayOff();

    scalars = 0;
    min = 0.0;
    max = 1.0;

    reader = vtkSmartPointer<vtkMNIObjectReader>::New();

    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->ImmediateModeRenderingOn();
    mapper->SetColorModeToMapScalars();
    mapper->SetLookupTable(lookupTableSelectionWidget->getCurrentLookupTable());

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    mniObjectTransfrom = vtkTransform::New();
    actor->SetUserTransform(mniObjectTransfrom);

    markedAreaTransfrom = vtkSmartPointer<vtkTransform>::New();
    markedAreaTransfrom->SetInput(mniObjectTransfrom);

    scalar_bar = vtkSmartPointer<vtkScalarBarActor >::New();
    scalar_bar->SetLookupTable(lookupTableSelectionWidget->getCurrentLookupTable());
    scalar_bar->SetNumberOfLabels(settings.value("numberOfLabels", 2).toInt());

    ren = vtkSmartPointer<vtkRenderer>::New();

    // Does this box support GPU Depth Peeling?
    isDepthPeelingSupported = getDepthPeelingSupported();
    qDebug() << "DEPTH PEELING SUPPORT: " << (isDepthPeelingSupported ? "YES" : "NO");

    ren->AddActor(actor);
    ren->AddActor(scalar_bar);

    qvtkWidget->GetRenderWindow()->AddRenderer(ren);
    qvtkWidget->GetInteractor()->LightFollowCameraOff();

    sphereWidget = vtkSphereWidget::New();
    sphereWidget->SetInteractor(qvtkWidget->GetInteractor());
    sphereWidget->SetPlaceFactor(4);
    sphereWidget->TranslationOff();
    sphereWidget->ScaleOff();
    sphereWidget->KeyPressActivationOff();
    sphereWidget->HandleVisibilityOn();

    light = ren->MakeLight();
    ren->RemoveAllLights();

    //    coneSource = vtkSmartPointer<vtkConeSource>::New();
    //    coneSource->CappingOn();
    //    coneSource->SetHeight(12);
    //    coneSource->SetRadius(5);
    //    coneSource->SetResolution(31);
    //    coneSource->SetCenter(6,0,0);
    //    coneSource->SetDirection(-1,0,0);

    //    coneMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    //    coneMapper->SetInputConnection(coneSource->GetOutputPort());

    //    pencilActor = vtkSmartPointer<vtkActor>::New();
    //    pencilActor->PickableOff();
    //    pencilActor->SetMapper(coneMapper);
    //    pencilActor->GetProperty()->SetColor(0,0,0);
    
    //    ren->AddViewProp(pencilActor);

    volumePicker = vtkSmartPointer<vtkVolumePicker>::New();
    volumePicker->SetTolerance(1e-6);

    pointPicker = vtkSmartPointer<vtkPointPicker>::New();

    windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(ren->GetRenderWindow());

    ffmpegWriter = vtkSmartPointer<vtkFFMPEGWriter>::New();
    ffmpegWriter->SetInputConnection(windowToImageFilter->GetOutputPort());

    pngWriter = vtkSmartPointer<vtkPNGWriter>::New();
    pngWriter->SetInputConnection(windowToImageFilter->GetOutputPort());

    //

    // Create a sphere

    vtkSmartPointer<vtkSphereSource> markedAreaPin =
            vtkSmartPointer<vtkSphereSource>::New();
    markedAreaPin->Update();

    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> markedAreaPinMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    markedAreaPinMapper->SetInputConnection(markedAreaPin->GetOutputPort());

    markedAreaPinActor =
            vtkSmartPointer<vtkActor>::New();
    markedAreaPinActor->SetMapper(markedAreaPinMapper);
    markedAreaPinActor->SetUserTransform(markedAreaTransfrom);
    markedAreaPinActor->SetPickable(false);

    markedAreaPinActor->SetVisibility(false);

    ren->AddActor(markedAreaPinActor);

    Connections = vtkEventQtSlotConnect::New();
    Connections->Connect(sphereWidget,
                         vtkCommand::InteractionEvent,
                         this,
                         SLOT(processSphereWidgetInteractionEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*)) );
    Connections->Connect(qvtkWidget->GetInteractor(),
                         vtkCommand::MouseMoveEvent,
                         this,
                         SLOT(processMouseMoveEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*)) );
    Connections->Connect(qvtkWidget->GetInteractor(),
                         vtkCommand::LeftButtonPressEvent,
                         this,
                         SLOT(processLeftButtonPressEvent(vtkObject*,ulong,void*,void*,vtkCommand*)));
    Connections->Connect(qvtkWidget->GetInteractor(),
                         vtkCommand::LeftButtonReleaseEvent,
                         this,
                         SLOT(processLeftButtonReleaseEvent(vtkObject*,ulong,void*,void*,vtkCommand*)));

    lightingPropertiesWidget->ambientDoubleSpinBox->setValue(actor->GetProperty()->GetAmbient());
    lightingPropertiesWidget->diffuseDoubleSpinBox->setValue(actor->GetProperty()->GetDiffuse());
    lightingPropertiesWidget->specularDoubleSpinBox->setValue(actor->GetProperty()->GetSpecular());
    lightingPropertiesWidget->opacityDoubleSpinBox->setValue(actor->GetProperty()->GetOpacity());





}

void MainWindow::initializeStateMachine()
{
    machine.addState(&animationModeState);
    machine.addState(&cameraModeState);
    machine.addState(&paintModeState);

    cameraModeState.addTransition(activateObjectAnimationModeAction, SIGNAL(triggered()), &animationModeState);
    cameraModeState.addTransition(activatePaintModeAction, SIGNAL(triggered()), &paintModeState);

    animationModeState.addTransition(activateCameraModeAction, SIGNAL(triggered()), &cameraModeState);
    animationModeState.addTransition(activatePaintModeAction, SIGNAL(triggered()), &paintModeState);

    paintModeState.addTransition(activateCameraModeAction, SIGNAL(triggered()), &cameraModeState);
    paintModeState.addTransition(activateObjectAnimationModeAction, SIGNAL(triggered()), &animationModeState);

    machine.setInitialState(&cameraModeState);

    connect(&cameraModeState, SIGNAL(entered()), this, SLOT(processCameraModeStateEntered()));
    connect(&cameraModeState, SIGNAL(exited()), this, SLOT(processCameraModeStateExited()));

    connect(&animationModeState, SIGNAL(entered()), this, SLOT(processAnimationModeStateEntered()));
    connect(&animationModeState, SIGNAL(exited()), this, SLOT(processAnimationModeStateExited()));

    connect(&paintModeState, SIGNAL(entered()), this, SLOT(processPaintModeStateEntered()));
    connect(&paintModeState, SIGNAL(exited()), this, SLOT(processPaintModeStateExited()));
}

void MainWindow::updateScalarBarActorTitleTextProperty(vtkSmartPointer<vtkTextProperty> textProperty)
{
    scalar_bar->SetTitleTextProperty(textProperty);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::updateScalarBarActorLabelTextProperty(vtkSmartPointer<vtkTextProperty> textProperty)
{
    scalar_bar->SetLabelTextProperty(textProperty);
    qvtkWidget->GetRenderWindow()->Render();
}

MainWindow::~MainWindow()
{

}

void MainWindow::showEvent ( QShowEvent * event )
{
    Q_UNUSED(event)
    machine.start();

    int num = qApp->argc() ;

    mniObjectTransfrom->Identity();

    // Parse --object-position option

    for ( int i = 0; i < num; i++ ) {
        QString s = qApp->argv()[i] ;
        if ( s.startsWith( "--object-position" ) )
        {
            qDebug() << "parsed --object-position";

            if(num - (i+1) < 3)
            {
                qDebug() << "Insufficient number of components specified for object position.";
                return;
            }

            bool parsingResult;

            ++i;
            QString x = qApp->argv()[i];
            double xDouble = x.toDouble(&parsingResult);

            if(parsingResult == false)
            {
                qDebug() << "Wrong 1-st parameter is specified for object position.";
                return;
            }

            ++i;
            QString y = qApp->argv()[i];
            double yDouble = y.toDouble(&parsingResult);

            if(parsingResult == false)
            {
                qDebug() << "Wrong 2-nd parameter is specified for object position.";
                return;
            }

            ++i;
            QString z = qApp->argv()[i];
            double zDouble = z.toDouble(&parsingResult);

            if(parsingResult == false)
            {
                qDebug() << "Wrong 3-rd parameter is specified for object position.";
                return;
            }

            double pos[3];
            pos[0] = xDouble;
            pos[1] = yDouble;
            pos[2] = zDouble;

            mniObjectTransfrom->Translate(xDouble, yDouble, zDouble);

            std::stringstream ss;
            vtkSmartPointer<vtkMatrix4x4> matrixCurrentState = vtkSmartPointer<vtkMatrix4x4>::New();

            mniObjectTransfrom->GetMatrix(matrixCurrentState);
            ss << "mniObjectTransfrom->Translate(xDouble, yDouble, zDouble): " << xDouble << yDouble << zDouble <<endl;
            matrixCurrentState->Print(ss);

            mniObjectTransfrom->GetPosition(pos);

            qDebug() << QString::fromStdString(ss.str());

            qDebug() << "pos[0]:" << pos[0];
            qDebug() << "pos[1]:" << pos[1];
            qDebug() << "pos[2]:" << pos[2];
        }
    }

    // Parse --object-rotation option

    for ( int i = 0; i < num; i++ )
    {
        QString s = qApp->argv()[i] ;
        if ( s.startsWith( "--object-rotation" ) )
        {
            qDebug() << "parsed --object-rotation";

            if(num - (i+1) < 3)
            {
                qDebug() << "Insufficient number of components specified for object rotation.";
                return;
            }

            bool parsingResult;

            ++i;
            QString rotX = qApp->argv()[i];
            double rotXDouble = rotX.toDouble(&parsingResult);

            if(parsingResult == false)
            {
                qDebug() << "Wrong 1-st parameter is specified for object rotation.";
                return;
            }

            ++i;
            QString rotY = qApp->argv()[i];
            double rotYDouble = rotY.toDouble(&parsingResult);

            if(parsingResult == false)
            {
                qDebug() << "Wrong 2-nd parameter is specified for object rotation.";
                return;
            }

            ++i;
            QString rotZ = qApp->argv()[i];
            double rotZDouble = rotZ.toDouble(&parsingResult);

            if(parsingResult == false)
            {
                qDebug() << "Wrong 3-rd parameter is specified for object rotation.";
                return;
            }

            double o[3];
            o[0] = rotXDouble;
            o[1] = rotYDouble;
            o[2] = rotZDouble;

            std::stringstream ss;

            vtkSmartPointer<vtkMatrix4x4> matrixCurrentState = vtkSmartPointer<vtkMatrix4x4>::New();


            mniObjectTransfrom->GetMatrix(matrixCurrentState);
            ss << "mniObjectTransfrom->Identity();" << endl;
            matrixCurrentState->Print(ss);

            // Rot(y) Rot(x) Rot (z)

            mniObjectTransfrom->RotateZ(o[2]);
            mniObjectTransfrom->GetMatrix(matrixCurrentState);
            ss << "mniObjectTransfrom->RotateY(o[2]): " << o[2] << endl;
            matrixCurrentState->Print(ss);

            mniObjectTransfrom->RotateX(o[0]);
            mniObjectTransfrom->GetMatrix(matrixCurrentState);
            ss << "mniObjectTransfrom->RotateX(o[0]): " << o[0] << endl;
            matrixCurrentState->Print(ss);

            mniObjectTransfrom->RotateY(o[1]);
            mniObjectTransfrom->GetMatrix(matrixCurrentState);
            ss << "mniObjectTransfrom->RotateY(o[1]): " << o[1] << endl;
            matrixCurrentState->Print(ss);





            qDebug() << QString::fromStdString(ss.str());
        }
    }

    // Parse --transformation option

    for ( int i = 0; i < num; i++ )
    {
        QString s = qApp->argv()[i];

        if ( s.startsWith( "--transformation" ) )
        {
            if(num - (i+1) >= 1)
            {
                ++i;
                QString fileName = qApp->argv()[i];
                if(QFile::exists(fileName))
                {
                    openTransformationByName(fileName);
                    qDebug() << "parsed --tranformation" << fileName;
                    break;
                }
                else
                {
                    qDebug() << "Wrong file specified";
                }
            }
        }
    }

    // Parse --object-to-load option

    for ( int i = 0; i < num; i++ )
    {
        QString s = qApp->argv()[i];

        if ( s.startsWith( "--object-to-load" ) )
        {
            if(num - (i+1) >= 1)
            {
                ++i;
                QString fileName = qApp->argv()[i];
                if(QFile::exists(fileName))
                {
                    openMeshFileByName(fileName);
                    qDebug() << "parsed --object-to-load" << fileName;
                    break;
                }
                else
                {
                    qDebug() << "Wrong file specified";
                }
            }
        }
    }

    // Parse --scalars-to-load option

    for ( int i = 0; i < num; i++ )
    {
        QString s = qApp->argv()[i];

        if ( s.startsWith( "--scalars-to-load" ) )
        {
            if(num - (i+1) >= 1)
            {
                ++i;
                QString fileName = qApp->argv()[i];
                if(QFile::exists(fileName))
                {
                    openPerPointScalarsByFilename(fileName);
                    qDebug() << "parsed --scalars-to-load" << fileName;
                    break;
                }
                else
                {
                    qDebug() << "Wrong file specified";
                }
            }
        }
    }

    // Parse --animation-name option

    for ( int i = 0; i < num; i++ )
    {
        QString s = qApp->argv()[i];

        if ( s.startsWith( "--animation-name" ) )
        {
            if(num - (i+1) >= 1)
            {
                ++i;
                QString fileName = qApp->argv()[i];
                qDebug() << "parsed --animation-name: " << fileName;
                animationManagementWidget->setAviFilename(fileName);
            }
        }
    }

    // Parse --screenshot-name option

    for( int i = 0; i < num; ++i )
    {

        QString s = qApp->argv()[i];

        if( s.startsWith( "--screenshot-name" ))
        {
            if(num - (i+1) >= 1)
            {
                ++i;
                QString fileName = qApp->argv()[i];
                animationManagementWidget->setPngFilename(fileName);
                qDebug() << "parsed --screenshot-name " << fileName;
                break;
            }
        }
    }
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
    settings.setValue("width", event->size().width());
    settings.setValue("height", event->size().height());
    QMainWindow::resizeEvent(event);
}

void MainWindow::openMeshFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select MNI object file."),
                                                    "",
                                                    tr("MNI object files(*.obj)"));
    if(fileName.isEmpty())
    {
        return;
    }

    openMeshFileByName(fileName);
}

void MainWindow::openMeshFileByName(const QString &fileName)
{
    if(reader->CanReadFile(fileName.toStdString().c_str()))
    {
        reader->SetFileName(fileName.toStdString().c_str());
        reader->Update();
        mapper->SetInputConnection(reader->GetOutputPort());

        setWindowTitle(tr("MNI object viewer - %1").arg(fileName));
        statusBar()->showMessage(tr("MNI object file was successfully opened."));
        ren->ResetCamera();
        sphereWidget->SetProp3D(actor);
        sphereWidget->PlaceWidget();
        light->SetFocalPoint(reader->GetOutput()->GetCenter());
    }
    else
    {
        statusBar()->showMessage(tr("The file seems not to contain MNI object."));
    }

    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::openPerPointScalarsFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select file with per point scalars."),
                                                    "",
                                                    tr("Scalars file (*.txt *.map)"));
    if(fileName.isEmpty())
    {
        return;
    }

    openPerPointScalarsByFilename(fileName);
}

void MainWindow::openPerPointScalarsByFilename(QString fileName)
{
    scalars = vtkFloatArray::New();
    ifstream in(fileName.toStdString().c_str(), ios::in);

    string current_token;
    int i =0;

    while(!in.eof()) {
        in >> current_token;
        scalars->InsertTuple1(i, string_to_double(current_token));
        if(i==0)
        {
            max = min = string_to_double(current_token);
        }
        else
        {
            max = (string_to_double(current_token)>max) ? string_to_double(current_token) : max;
            min = (string_to_double(current_token)<min) ? string_to_double(current_token) : min;
        }
        ++i;
    }

    reader->GetOutput()->GetPointData()->SetScalars(scalars);
    mapper->SetScalarRange(min, max);
    lookupTableSelectionWidget->setScalarRange(min, max);
    lookupTableSelectionWidget->setDefaultRangeValues(min, max);
}

void MainWindow::saveCurrentTransformation()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save transformation"),
                                                    "",
                                                    tr("Transformation (*.transformation)"));
    if(fileName.isEmpty())
    {
        return;
    }

    double elements[16];
    vtkMatrix4x4::DeepCopy(elements, mniObjectTransfrom->GetMatrix());

    ofstream transformationFile;
    transformationFile.open(fileName.toStdString().c_str());

    for(int i=0; i<16; ++i)
    {
        transformationFile << elements[i] << " ";
    }

    transformationFile.close();
}

void MainWindow::openTransformation()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open transformation"),
                                                    "",
                                                    tr("Transformation (*.transformation)"));
    if(fileName.isEmpty())
    {
        return;
    }

    openTransformationByName(fileName);

    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::openTransformationByName(QString fileName)
{
    ifstream in(fileName.toStdString().c_str(), ios::in);

    double elements[16];

    for(int i=0; i<16; ++i)
    {
        in >> elements[i];
    }

    mniObjectTransfrom->GetMatrix()->DeepCopy(elements);

    in.close();
}

void MainWindow::updateLookupTable(vtkSmartPointer<vtkLookupTable> lookupTable)
{
    mapper->SetLookupTable(lookupTable);
    mapper->SetColorModeToMapScalars();
    scalar_bar->SetLookupTable(lookupTable);
    reader->GetOutput()->GetPointData()->SetScalars(scalars);
    mapper->SetScalarRange(min, max);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::updateDirectRgbColors(vtkSmartPointer<vtkUnsignedCharArray> colors)
{
    mapper->SetLookupTable(0);
    mapper->SetColorModeToDefault();

    // Detect number of color tuples to add

    int numberOfPointTuples = reader->GetOutput()->GetPointData()->GetNumberOfTuples();
    int numberOfRgbTuplesToAdd = numberOfPointTuples - colors->GetNumberOfTuples();

    for(int i = 0; i < numberOfRgbTuplesToAdd; ++i)
    {
        unsigned char value[4];

        value[0] = 255;
        value[1] = 255;
        value[2] = 255;
        value[3] = 255;

        colors->InsertNextTupleValue(value);
    }

    reader->GetOutput()->GetPointData()->SetScalars(colors);

    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::createActions()
{
    // Open MNI-object file action
    openMeshFileAction = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    openMeshFileAction->setShortcut(QKeySequence::Open);
    connect(openMeshFileAction, SIGNAL(triggered()), this, SLOT(openMeshFile()));

    // Open scalars file corresponding to MNI-object
    openPerPointScalarsFileAction = new QAction(tr("Open &scalars file"), this);
    connect(openPerPointScalarsFileAction, SIGNAL(triggered()), this, SLOT(openPerPointScalarsFile()));

    // Open transformation

    loadTransformationAction = new QAction(tr("Load transformation"), this);
    connect(loadTransformationAction, SIGNAL(triggered()), this, SLOT(openTransformation()));

    // Camera mode
    activateCameraModeAction = new QAction(tr("Camera mode"), this);
    activateCameraModeAction->setIcon(QIcon(":/images/eye_point.svg"));
    activateCameraModeAction->setCheckable(true);

    // Animation mode
    activateObjectAnimationModeAction = new QAction(tr("Object animation mode"), this);
    activateObjectAnimationModeAction->setIcon(QIcon(":/images/camera.svg"));
    activateObjectAnimationModeAction->setCheckable(true);

    // Paint mode
    activatePaintModeAction = new QAction(tr("Paint mode"), this);
    activatePaintModeAction->setIcon(QIcon(":/images/pencil.svg"));
    activatePaintModeAction->setCheckable(true);

    // Create color selection button
    selectPencilColorToolButton = new QToolButton(this);
    selectPencilColorToolButton->setIcon(getIconFilledWithColor(QColor(Qt::black)));

    // Create background color selection button

    selectBackgroundColorToolButton = new QToolButton(this);
    selectBackgroundColorToolButton->setIcon(QIcon(":/images/background_color.svg"));

    // Create save transformation action

    saveTransformationAction = new QAction(tr("Save transformation"), this);

    connect(selectPencilColorToolButton, SIGNAL(clicked()),
            this, SLOT(processSelectColorForPencil()));
    connect(selectBackgroundColorToolButton, SIGNAL(clicked()),
            this, SLOT(processSelectBackgroundColor()));
    connect(saveTransformationAction, SIGNAL(triggered()),
            this, SLOT(saveCurrentTransformation()));
}

void MainWindow::createMenu()
{
    QMenuBar *menubar = menuBar();
    QMenu *file = menubar->addMenu("&File");
    file->addAction(openMeshFileAction);
    file->addAction(openPerPointScalarsFileAction);
    file->addAction(saveTransformationAction);
    file->addAction(loadTransformationAction);

    QMenu *view = menubar->addMenu("&View");
    lookupTableSelectionDockWidget->setVisible(false);
    shadingModelSelectionDockWidget->setVisible(false);
    lightingPropertiesDockWidget->setVisible(false);
    surfaceSelectionDockWidget->setVisible(false);
    animationManagementDockWidget->setVisible(false);

    view->addAction(lookupTableSelectionDockWidget->toggleViewAction());
    view->addAction(shadingModelSelectionDockWidget->toggleViewAction());
    view->addAction(lightingPropertiesDockWidget->toggleViewAction());
    view->addAction(surfaceSelectionDockWidget->toggleViewAction());
    view->addAction(animationManagementDockWidget->toggleViewAction());
}

void MainWindow::createToolbar()
{
    QToolBar *modesToolBar = addToolBar(tr("Modes"));
    modesToolBar->addAction(activateCameraModeAction);
    modesToolBar->addAction(activateObjectAnimationModeAction);
    modesToolBar->addAction(activatePaintModeAction);
    modesToolBar->addWidget(selectPencilColorToolButton);
    modesToolBar->addWidget(selectBackgroundColorToolButton);
}

void MainWindow::setFlatShadingModel()
{
    actor->GetProperty()->SetInterpolationToFlat();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::setGouraudShadingModel()
{
    actor->GetProperty()->SetInterpolationToGouraud();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processSphereWidgetInteractionEvent(vtkObject *caller, unsigned long, void*, void*, vtkCommand*)
{
    vtkSphereWidget *widget = reinterpret_cast<vtkSphereWidget*>(caller);
    light->SetPosition(widget->GetHandlePosition());
}

void MainWindow::processMouseMoveEvent(vtkObject *caller, unsigned long, void*, void*, vtkCommand*)
{
    vtkRenderWindowInteractor *iren = reinterpret_cast<vtkRenderWindowInteractor*>(caller);

    int pos[2];
    iren->GetEventPosition(pos);
    volumePicker->Pick(pos[0], pos[1], 0, ren);
    double p[3];
    double n[3];
    volumePicker->GetPickPosition(p);
    vtkIdType pointId = volumePicker->GetPointId();

    //pointPicker->Pick(pos[0], pos[1], 0, ren);
    //picker->GetCellId()
    volumePicker->GetPickNormal(n);
    /*pencilActor->SetPosition(p);
    pointCone(pencilActor, n[0], n[1], n[2]);*/

    if(pointId == -1)
    {
        statusBar()->showMessage(tr("Current position is (%1, %2, %3)").arg(p[0]).arg(p[1]).arg(p[2]));
    }
    else
    {
        if(scalars)
        {
            double scalarValue;
            scalarValue = scalars->GetTuple1(pointId);
            statusBar()->showMessage(tr("Current position is (%1, %2, %3). Scalar value %4 .")
                                     .arg(p[0])
                                     .arg(p[1])
                                     .arg(p[2])
                                     .arg(scalarValue));
        }
        else
        {
            statusBar()->showMessage(tr("Current position is (%1, %2, %3)").arg(p[0]).arg(p[1]).arg(p[2]));
        }

        if(isInPaintMode)
        {
            double v[3];
            reader->GetOutput()->GetPoints()->GetPoint(pointId, v);
            markedAreaPinActor->SetPosition(v);

            if(leftMouseButtonIsPressed)
            {
                vtkSmartPointer<vtkSphereSource> markedAreaPin =
                        vtkSmartPointer<vtkSphereSource>::New();
                markedAreaPin->Update();

                vtkSmartPointer<vtkPolyDataMapper> markedAreaPinMapper =
                        vtkSmartPointer<vtkPolyDataMapper>::New();
                markedAreaPinMapper->SetInputConnection(markedAreaPin->GetOutputPort());

                vtkSmartPointer<vtkActor> pointToPlaceActor =
                        vtkSmartPointer<vtkActor>::New();
                pointToPlaceActor->SetMapper(markedAreaPinMapper);
                pointToPlaceActor->SetUserTransform(markedAreaTransfrom);
                pointToPlaceActor->GetProperty()->SetColor(pencilColor.redF(), pencilColor.greenF(), pencilColor.blueF());
                pointToPlaceActor->SetPosition(v);
                pointToPlaceActor->SetPickable(false);

                if(placedPoints.contains(pointId))
                {
                    ren->RemoveActor(placedPoints[pointId]);
                }

                placedPoints[pointId] = pointToPlaceActor;

                ren->AddActor(pointToPlaceActor);
            }
        }
    }

    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processLeftButtonPressEvent(vtkObject *caller, unsigned long, void*, void*, vtkCommand*)
{
    qDebug() << "processLeftButtonPressEvent(vtkObject *caller, unsigned long, void*, void*, vtkCommand*)";
    leftMouseButtonIsPressed = true;

    vtkRenderWindowInteractor *iren = reinterpret_cast<vtkRenderWindowInteractor*>(caller);

    int pos[2];
    iren->GetEventPosition(pos);
    volumePicker->Pick(pos[0], pos[1], 0, ren);
    double p[3];
    double n[3];
    volumePicker->GetPickPosition(p);

    vtkIdType pointId = volumePicker->GetPointId();

    if(pointId != -1)
    {
        if(scalars)
        { double scalarValue;
            double color[3];
            scalarValue = scalars->GetTuple1(pointId);
            mapper->GetLookupTable()->GetColor(scalarValue, color);
            cout << "id " << pointId << " (" << p[0] << " " << p[1] << " " << p[2] << ") " << scalarValue <<
                    " (" << color[0] << " " << color[1] << " " << color[2] << ") "  << endl;
        }
        else
        {
            cout << "id " << pointId << " (" << p[0] << " " << p[1] << " " << p[2] << ") " << endl;
        }

        if(isInPaintMode)
        {
            double v[3];
            reader->GetOutput()->GetPoints()->GetPoint(pointId, v);
            markedAreaPinActor->SetPosition(v);

            if(leftMouseButtonIsPressed)
            {
                vtkSmartPointer<vtkSphereSource> markedAreaPin =
                        vtkSmartPointer<vtkSphereSource>::New();
                markedAreaPin->Update();

                vtkSmartPointer<vtkPolyDataMapper> markedAreaPinMapper =
                        vtkSmartPointer<vtkPolyDataMapper>::New();
                markedAreaPinMapper->SetInputConnection(markedAreaPin->GetOutputPort());

                vtkSmartPointer<vtkActor> pointToPlaceActor =
                        vtkSmartPointer<vtkActor>::New();
                pointToPlaceActor->SetMapper(markedAreaPinMapper);
                pointToPlaceActor->SetUserTransform(markedAreaTransfrom);
                pointToPlaceActor->GetProperty()->SetColor(pencilColor.redF(), pencilColor.greenF(), pencilColor.blueF());
                pointToPlaceActor->SetPosition(v);
                pointToPlaceActor->SetPickable(false);

                if(placedPoints.contains(pointId))
                {
                    ren->RemoveActor(placedPoints[pointId]);
                }

                placedPoints[pointId] = pointToPlaceActor;

                ren->AddActor(pointToPlaceActor);
            }
        }
    }
}

void MainWindow::processLeftButtonReleaseEvent(vtkObject *caller, unsigned long, void*, void*, vtkCommand*)
{
    qDebug() << "processLeftButtonReleaseEvent(vtkObject *caller, unsigned long, void*, void*, vtkCommand*)";
    leftMouseButtonIsPressed = false;
}

void MainWindow::pointCone(vtkActor* actorToRotate, double nx, double ny, double nz)
{
    actorToRotate->SetOrientation(0.0, 0.0, 0.0);
    double n = sqrt(nx*nx + ny*ny + nz*nz);
    if (nx < 0.0)
    {
        actorToRotate->RotateWXYZ(180, 0, 1, 0);
        n = -n;
    }
    actorToRotate->RotateWXYZ(180, (nx+n)*0.5, ny*0.5, nz*0.5);
}

void MainWindow::processLightingStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        enableLighting();
    } else if (state == Qt::Unchecked)
    {
        disableLighting();
    }
}

void MainWindow::enableLighting()
{
    ren->RemoveAllLights();
    ren->AddLight(light);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::disableLighting()
{
    ren->RemoveAllLights();
    qvtkWidget->GetRenderWindow()->Render();
}

QIcon MainWindow::getIconFilledWithColor(QColor color)
{
    QPixmap pixmap(24, 24);
    pixmap.fill(color);
    return QIcon(pixmap);
}

void MainWindow::processAmbientChanged(double value)
{
    actor->GetProperty()->SetAmbient(value);
    //light->SetAmbientColor(value, value, value);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processSpecularChanged(double value)
{
    actor->GetProperty()->SetSpecular(value);
    //light->SetSpecularColor(value, value, value);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processDiffuseChanged(double value)
{
    //    actor->GetProperty()->SetDiffuse(value);
    light->SetDiffuseColor(value, value, value);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processOpacityChanged(double value)
{
    actor->GetProperty()->SetOpacity(value);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processLightingWidgetStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        sphereWidget->On();
    } else if (state == Qt::Unchecked)
    {
        sphereWidget->Off();
    }
}

void MainWindow::openSelection()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open selected points"),
                                                    "",
                                                    tr("Selected points(*.pts)"));
    if(fileName.isEmpty())
    {
        return;
    }

    ifstream in(fileName.toStdString().c_str(), ios::in);

    vtkIdType pointId;
    pointId = 0;

    while(!in.eof()) {
        double r;
        double g;
        double b;
        double a;

        in >> r;
        in >> g;
        in >> b;
        in >> a;

        if(r != 1 && g != -1 && b != -1 && a != -1 )
        {
            double v[3];
            reader->GetOutput()->GetPoints()->GetPoint(pointId, v);

            vtkSmartPointer<vtkSphereSource> markedAreaPin =
                    vtkSmartPointer<vtkSphereSource>::New();
            markedAreaPin->Update();

            vtkSmartPointer<vtkPolyDataMapper> markedAreaPinMapper =
                    vtkSmartPointer<vtkPolyDataMapper>::New();
            markedAreaPinMapper->SetInputConnection(markedAreaPin->GetOutputPort());

            vtkSmartPointer<vtkActor> pointToPlaceActor =
                    vtkSmartPointer<vtkActor>::New();
            pointToPlaceActor->SetMapper(markedAreaPinMapper);
            pointToPlaceActor->SetUserTransform(markedAreaTransfrom);
            pointToPlaceActor->GetProperty()->SetColor(r, g, b);
            pointToPlaceActor->SetPosition(v);
            pointToPlaceActor->SetPickable(false);

            if(!placedPoints.keys().contains(pointId))
            {
                placedPoints[pointId] = pointToPlaceActor;
                ren->AddActor(pointToPlaceActor);
            }
        }

        pointId++;
    }

    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::saveSelection()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save selected points"),
                                                    "",
                                                    tr("Selected points(*.pts)"));

    if(fileName.isEmpty())
    {
        return;
    }

    ofstream outfile;
    outfile.open (fileName.toStdString().c_str());

    int size = reader->GetOutput()->GetNumberOfPoints();

    for(int i=0; i<size; ++i)
    {
        if(placedPoints.contains(i))
        {
            double rgb[3];
            placedPoints[i]->GetProperty()->GetColor(rgb);
            outfile << rgb[0] << " " << rgb[1] << " " << rgb[2] << " " << 1.0 << " " << endl;
        }
        else
        {
            outfile << -1.0 << " " << -1.0 << " " << -1.0 << " " << 1.0 << " " << endl;
        }
    }

    outfile.close();
}

void MainWindow::clearSelection()
{
    for (int i = 0; i < placedPoints.keys().size(); ++i) {
        ren->RemoveActor(placedPoints[placedPoints.keys().at(i)]);
    }

    placedPoints.clear();

    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::saveObjectStateAsFirstAnimationPoint()
{
    double pos[3];
    double o[3];
    double scale[3];

    mniObjectTransfrom->GetPosition(pos);
    mniObjectTransfrom->GetOrientation(o);
    mniObjectTransfrom->GetScale(scale);
    animationManagementWidget->saveCurrentObjectStateAsStartPoint(
                pos[0],
                pos[1],
                pos[2],
                o[0],
                o[1],
                o[2]);
}

void MainWindow::saveObjectStateAsSecondAnimationPoint()
{
    double pos[3];
    double o[3];
    double scale[3];

    mniObjectTransfrom->GetPosition(pos);
    mniObjectTransfrom->GetOrientation(o);
    mniObjectTransfrom->GetScale(scale);
    animationManagementWidget->saveCurrentObjectStateAsEndPoint(
                pos[0],
                pos[1],
                pos[2],
                o[0],
                o[1],
                o[2]);
}


void MainWindow::processCurrentTimeChanged()
{
    double pos[3];
    pos[0] = animationManagementWidget->getCurrentX();
    pos[1] = animationManagementWidget->getCurrentY();
    pos[2] = animationManagementWidget->getCurrentZ();
    mniObjectTransfrom->Identity();
    mniObjectTransfrom->Translate(pos);

    qDebug() << "pos[0]:" << pos[0];
    qDebug() << "pos[1]:" << pos[1];
    qDebug() << "pos[2]:" << pos[2];

    double o[3];
    o[0] = animationManagementWidget->getCurrentRotX();
    o[1] = animationManagementWidget->getCurrentRotY();
    o[2] = animationManagementWidget->getCurrentRotZ();

    qDebug() << "o[0]:" << o[0];
    qDebug() << "o[1]:" << o[1];
    qDebug() << "o[2]:" << o[2];
    mniObjectTransfrom->RotateZ(animationManagementWidget->getCurrentRotZ());
    mniObjectTransfrom->RotateX(animationManagementWidget->getCurrentRotX());
    mniObjectTransfrom->RotateY(animationManagementWidget->getCurrentRotY());

    ren->ResetCameraClippingRange();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processCurrentWritingFrameChanged()
{
    processCurrentTimeChanged();
    qDebug() << "windowToImageFilter->Modified();";
    windowToImageFilter->Modified();
    qDebug() << "BEGIN ffmpegWriter->Write();";
    ffmpegWriter->Write();
    qDebug() << "END ffmpegWriter->Write();";

    //    vtkSmartPointer<vtkPNGWriter> writer =
    //        vtkSmartPointer<vtkPNGWriter>::New();
    //    writer->SetFileName("screenshot.png");
    //    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    //    writer->Write();
    qDebug() << "animationManagementWidget->processFrameIsWritten();";
    animationManagementWidget->processFrameIsWritten();
}

void MainWindow::processWritingToAviInitiated()
{
    qDebug() << "void MainWindow::processWritingToAviInitiated()";
    ffmpegWriter->SetQuality(2);
    ffmpegWriter->SetRate(24);
    //    ffmpegWriter->SetBitRate(100000);
    //    ffmpegWriter->SetBitRateTolerance(100000);
    ffmpegWriter->Start();
}

void MainWindow::processWritingToAviCompleted()
{
    qDebug() << "void MainWindow::processWritingToAviCompleted()";
    //ffmpegWriter->Delete();
    ffmpegWriter->End();
    //delete ffmpegWriter->GetPointer();
    //ffmpegWriter->Delete();
    //ffmpegWriter = vtkSmartPointer<vtkFFMPEGWriter>::New();
    //ffmpegWriter->SetInputConnection(windowToImageFilter->GetOutputPort());
}

void MainWindow::processWritingAviFilenameChanged(QString filename)
{
    qDebug() << "void MainWindow::processWritingAviFilenameChanged(QString filename)";
    ffmpegWriter->SetFileName(filename.toStdString().c_str());
}

void MainWindow::processWritingPngFilenameChanged(QString filename)
{
    qDebug() << "void MainWindow::processWritingPngFilenameChanged(QString filename)";
    pngWriter->SetFileName(filename.toStdString().c_str());
}

void MainWindow::processWritePng()
{
    qDebug() << "void MainWindow::processWritePng()";
    windowToImageFilter->Modified();
    pngWriter->Write();
}

void MainWindow::processAnimationModeStateEntered()
{
    qDebug() << "processAnimationModeStateEntered()";

    vtkSmartPointer<vtkInteractorStyleTrackballActor> style =
            vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();

    qvtkWidget->GetInteractor()->SetInteractorStyle(style);
    lightingPropertiesDockWidget->setEnabled(false);
    animationManagementDockWidget->setEnabled(true);


    activateObjectAnimationModeAction->setChecked(true);
    activateObjectAnimationModeAction->setEnabled(false);

    lightingPropertiesWidget->lightingWidgetVisibilityCheckbox->setChecked(false);
}

void MainWindow::processAnimationModeStateExited()
{
    qDebug() << "processAnimationModeStateExited()";

    activateObjectAnimationModeAction->setChecked(false);
    activateObjectAnimationModeAction->setEnabled(true);
}

void MainWindow::processCameraModeStateEntered()
{
    qDebug() << "processCameraModeStateEntered()";

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
            vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

    qvtkWidget->GetInteractor()->SetInteractorStyle(style);
    lightingPropertiesDockWidget->setEnabled(true);
    animationManagementDockWidget->setEnabled(false);
    activateCameraModeAction->setChecked(true);
    activateCameraModeAction->setEnabled(false);
}

void MainWindow::processCameraModeStateExited()
{
    qDebug() << "processCameraModeStateExited()";
    activateCameraModeAction->setChecked(false);
    activateCameraModeAction->setEnabled(true);
}

void MainWindow::processPaintModeStateEntered()
{
    qDebug() << "processPaintModeStateEntered()";

    vtkSmartPointer<vtkInteractorStyle> style =
            vtkSmartPointer<vtkInteractorStyle>::New();

    qvtkWidget->GetInteractor()->SetInteractorStyle(style);
    lightingPropertiesDockWidget->setEnabled(false);
    animationManagementDockWidget->setEnabled(false);
    activatePaintModeAction->setChecked(true);
    activatePaintModeAction->setEnabled(false);

    isInPaintMode = true;
    leftMouseButtonIsPressed = false;

    markedAreaPinActor->SetVisibility(true);
}

void MainWindow::processPaintModeStateExited()
{
    qDebug() << "processPaintModeStateExited()";

    activatePaintModeAction->setChecked(false);
    activatePaintModeAction->setEnabled(true);

    isInPaintMode = false;

    markedAreaPinActor->SetVisibility(false);
}


void MainWindow::processSelectColorForPencil()
{
    QColor color;
    color = QColorDialog::getColor(Qt::black, this);

    if(color.isValid())
    {
        selectPencilColorToolButton->setIcon(getIconFilledWithColor(color));
        pencilColor = color;
        markedAreaPinActor->GetProperty()->SetColor(pencilColor.redF(), pencilColor.greenF(), pencilColor.blueF());
    }
}

void MainWindow::processSelectBackgroundColor()
{
    QColor color;
    color = QColorDialog::getColor(settings.value("backgroundColor", QColor(Qt::black) ).value<QColor>(), this);

    if(color.isValid())
    {
        settings.setValue("backgroundColor", color);
        ren->SetBackground(color.redF(), color.greenF(), color.blueF());
        qvtkWidget->GetRenderWindow()->Render();
    }
}


void MainWindow::showLegend()
{
    scalar_bar->VisibilityOn();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::hideLegend()
{
    scalar_bar->VisibilityOff();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processScalarRangeChanged(double min, double max)
{
    this->min = min;
    this->max = max;
    mapper->SetScalarRange(min, max);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processNumberOfLabelsChanged(int numberOfLabels)
{
    scalar_bar->SetNumberOfLabels(numberOfLabels);
    settings.setValue("numberOfLabels", numberOfLabels);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processTransformationResetion()
{
    mniObjectTransfrom->Identity();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processDepthSortingCheckstateChanged(int state)
{
    if(state == Qt::Checked)
    {
        if(isDepthPeelingSupported)
        {
            enableDepthPeeling();
        }
        else
        {
            enableCpuDepthSortingPipeline();
        }

    } else if (state == Qt::Unchecked)
    {
        disableCpuDepthSortingPipeline();
        disableDepthPeeling();
    }
}

void MainWindow::enableCpuDepthSortingPipeline()
{
    vtkSmartPointer<vtkDepthSortPolyData> depthSort =
            vtkSmartPointer<vtkDepthSortPolyData>::New();
    depthSort->SetInputConnection(reader->GetOutputPort());
    depthSort->SetDirectionToBackToFront();
    depthSort->SetVector(1, 1, 1);
    depthSort->SetCamera(ren->GetActiveCamera());
    mapper->SetInputConnection(depthSort->GetOutputPort());
    depthSort->Update();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::disableCpuDepthSortingPipeline()
{
    mapper->SetInputConnection(reader->GetOutputPort());
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::enableDepthPeeling()
{
    SetupEnvironmentForDepthPeeling(qvtkWidget->GetRenderWindow(), ren, 100, 0.1);
}

void MainWindow::disableDepthPeeling()
{
    UnsetupEnvironmentForDepthPeeling(qvtkWidget->GetRenderWindow(), ren);
}

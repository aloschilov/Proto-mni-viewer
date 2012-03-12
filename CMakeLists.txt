CMAKE_MINIMUM_REQUIRED(VERSION 2.6.3 FATAL_ERROR)

PROJECT(ProtoMniViewer)

IF(NOT VTK_BINARY_DIR)
FIND_PACKAGE(VTK)
IF(NOT VTK_DIR)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(NOT VTK_DIR)
INCLUDE(${VTK_USE_FILE})
ENDIF(NOT VTK_BINARY_DIR)

# use what QVTK built with
SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
FIND_PACKAGE(Qt4)
IF(QT_USE_FILE)
    INCLUDE(${QT_USE_FILE})
ELSE(QT_USER_FILE)
    SET(QT_LIBRARIES ${QT_QT_LIBRARY})
ENDIF(QT_USE_FILE)

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(${VTK_TDX_INCLUDE_PATH})

# Set your files and resources here  
SET(ProtoMniViewerSrcs main.cxx mainwindow.cxx lookuptableselectionwidget.cxx shadingmodelselectionwidget.cxx lightingpropertieswidget.cxx)
SET(ProtoMniViewerHeaders mainwindow.h lookuptableselectionwidget.h shadingmodelselectionwidget.h lightingpropertieswidget.h)
SET(ProtoMniViewerResources protomniviewer.qrc)

# The rest should just work (sure...)
# QT4_WRAP_UI(UISrcs ${ProtoMniViewerUI})
QT4_WRAP_CPP(MOCSrcs ${ProtoMniViewerHeaders} )
QT4_ADD_RESOURCES(ResourceSrcs ${ProtoMniViewerResources})

SOURCE_GROUP("Resources" FILES
  ${ProtoMniViewerResources}
  ${EXE_ICON}
)

SOURCE_GROUP("Generated" FILES
  ${MOCSrcs}
  ${ResourceSrcs}
  ${RCS_SOURCES}
)

ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT)
SET_SOURCE_FILES_PROPERTIES(${ProtoMniViewerSrcs} PROPERTIES
                            OBJECT_DEPENDS "${UISrcs}")

# It's nice to have the ui in the windows project file...just double click on it
# and designer comes up on that ui file :)
IF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
  SET (ProtoMniViewerSrcs ${ProtoMniViewerSrcs} ${ProtoMniViewerUI})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
IF (${CMAKE_BUILD_TOOL} MATCHES "devenv")
  SET (ProtoMniViewerSrcs ${ProtoMniViewerSrcs} ${ProtoMniViewerUI})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "devenv")

ADD_EXECUTABLE( ProtoMniViewer MACOSX_BUNDLE ${ProtoMniViewerSrcs} ${UISrcs} ${MOCSrcs} ${ResourceSrcs})

TARGET_LINK_LIBRARIES( ProtoMniViewer
  QVTK
  ${QT_LIBRARIES}
  vtkRendering
  vtkGraphics
  vtkIO
  vtkCommon
  vtkInfovis
  vtkViews
)

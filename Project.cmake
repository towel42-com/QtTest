SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QTSCRIPT TRUE )
SET( QT_USE_QTSQL TRUE )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTNETWORK TRUE )

include(${QT_USE_FILE})

IF(WIN32)
SET(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /RTC1 /Gm /MDd /ZI /W3 /Od")
SET(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /RTC1 /Gm /MDd /ZI /W3 /Od")
ENDIF(WIN32)

QT4_WRAP_UI(qtproject_UIS_H ${qtproject_UIS})
QT4_WRAP_CPP(qtproject_MOC_SRCS ${qtproject_H})

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/UI)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/QuickTimeSDK/CIncludes)
include_directories(${CMAKE_SOURCE_DIR}/ConvertToMP4Utils)
include_directories(${CMAKE_SOURCE_DIR}/zlib)

source_group("Generated Files" FILES ${qtproject_UIS_H} ${qtproject_MOC_SRCS} ${qtproject_QRC_SRCS} )
source_group("Resource Files" FILES ${qtproject_QRC} )
source_group("Designer Files" FILES ${qtproject_UIS} )
source_group("Header Files" FILES ${qtproject_H} )


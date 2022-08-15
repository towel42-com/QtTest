project(qttest)

cmake_minimum_required(VERSION 2.8)
find_package(Qt5 REQUIRED COMPONENTS Core Network Sql Gui Widgets Xml XmlPatterns)
FUNCTION(AddQtIncludes ModuleName )
        if ( DEFINED Qt5${ModuleName}_INCLUDE_DIRS )
            foreach( dir ${Qt5${ModuleName}_INCLUDE_DIRS} )
                include_directories( ${dir} )
            endforeach()
        else()
            message( WARNING "Qt Module: ${ModuleName} is not defined (possibly not loaded)" )
        endif()
    ENDFUNCTION()

AddQtIncludes(Core)
#AddQtIncludes(XmlPatterns)

#set(qtproject_SRCS main.cpp TreeWidget.cpp)
#set(qtproject_H TreeWidget.h)
set(qtproject_UIS )
set(qtproject_QRC )


#QT5_WRAP_CPP( qtproject_MOC_SRCS ${qtproject_H})

add_executable(qttest main.cpp
    ${qtproject_SRCS} 
    ${qtproject_UIS_H} 
    ${qtproject_MOC_SRCS} 
    ${qtproject_H} 
    ${qtproject_QRC_SRCS}
) 


SET( EXTRA_LIBS
    Rpcrt4.lib
    Iphlpapi.lib
    psapi.lib
    shell32.lib
    Advapi32.lib
)

target_link_libraries( qttest Qt::Core
)


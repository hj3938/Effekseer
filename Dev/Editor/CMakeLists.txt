
enable_language(CSharp)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../release")

# specify C# framework
set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION v4.6.1)

file(GLOB files_core
    EffekseerCore/*.cs
    EffekseerCore/Binary/*.cs
    EffekseerCore/Binary/RenderData/*.cs
    EffekseerCore/Command/*.cs
    EffekseerCore/Data/*.cs
    EffekseerCore/Data/Group/*.cs
    EffekseerCore/Data/Value/*.cs
    EffekseerCore/Exporter/*.cs
    EffekseerCore/InternalScript/*.cs
    EffekseerCore/Properties/*.cs
    EffekseerCore/Properties/*.cs
    EffekseerCore/Script/*.cs
    EffekseerCore/Utl/*.cs
    EffekseerCore/IO/*.cs	
    EffekseerCore/Utils/*.cs
    EffekseerCore/Data/ProcedualModelValues.cs
    EffekseerCore/Data/Value/Int2.cs
    EffekseerCore/Data/Value/ObjectCollection.cs
)

list(APPEND files_core
    EffekseerCore/IO/EfkEfc.cs
    EffekseerCore/Utils/ResourceCache.cs
    EffekseerCore/Utils/CsvReader.cs
    EffekseerCore/Utils/ProjectVersionUpdater.cs
)

add_library(EffekseerCore
  SHARED
  ${files_core}
)

set_target_properties(EffekseerCore PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.IO.Compression;System.IO.Compression.FileSystem")

#set_target_properties(EffekseerCore PROPERTIES
#    VS_DOTNET_REFERENCE_IronPython ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/IronPython.dll
#    VS_DOTNET_REFERENCE_IronPython_Modules ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/IronPython.Modules.dll
#    VS_DOTNET_REFERENCE_Microsoft_Dynamic ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Dynamic.dll
#    VS_DOTNET_REFERENCE_Microsoft_Scripting ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Scripting.dll
#    VS_DOTNET_REFERENCE_Microsoft_Scripting_Metadata ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Scripting.Metadata.dll
#	)

set_property(TARGET EffekseerCore PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_12.0.1;DynamicLanguageRuntime_1.2.2;IronPython_2.7.9") 

target_compile_options(EffekseerCore PRIVATE "/unsafe")


file(GLOB files 
    Effekseer/*.cs
    Effekseer/Data/*.cs
    Effekseer/GUI/*.cs
    Effekseer/GUI/Component/*.cs
    Effekseer/GUI/Dialog/*.cs
    Effekseer/GUI/Dock/*.cs
    Effekseer/GUI/Menu/*.cs
    Effekseer/Plugin/*.cs
    Effekseer/Properties/*.cs
    Effekseer/swig/*.cs
    Effekseer/Utils/*.cs
    Effekseer/Process/*.cs
    Effekseer/GUI/Dock/ProcedualModel.cs
    Effekseer/GUI/Component/Int2.cs
	Effekseer/GUI/Component/ObjectReference.cs
    Effekseer/IO/MaterialCacheGenerator.cs
    Effekseer/IO/mqoToEffekseerModelConverter/*.cs
    Effekseer/IO/mqoToEffekseerModelConverter/mqoIO/*.cs
    Effekseer/IO/mqoToEffekseerModelConverter/mqoIO/Realtime/*.cs
    )

list(APPEND files
    Effekseer/swig/IO.cs
    Effekseer/swig/IOCallback.cs
    Effekseer/swig/SystemLanguage.cs
    Effekseer/swig/MainWindow.cs
    Effekseer/swig/MainWindowState.cs
    Effekseer/GUI/Dock/Environment.cs
    Effekseer/Configs/WindowConfig.cs
    Effekseer/GUI/ThumbnailManager.cs
)

add_executable(EffekseerEditor
    ${files}
)

set_target_properties(EffekseerEditor PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.Text.RegularExpressions;Microsoft.CSharp;System.IO.Compression")

add_dependencies(EffekseerEditor EffekseerCore Viewer)
target_link_libraries(EffekseerEditor EffekseerCore Viewer)

target_compile_options(EffekseerEditor PRIVATE "/unsafe")
set_property(TARGET EffekseerEditor PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/Effekseer/icon.ico")

target_compile_definitions(EffekseerCore PRIVATE SCRIPT_ENABLED)

# specify a startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT EffekseerEditor)

if (MSVC)
    set_target_properties(EffekseerEditor PROPERTIES OUTPUT_NAME_DEBUG "Effekseer" )
    set_target_properties(EffekseerEditor PROPERTIES OUTPUT_NAME_RELEASE "Effekseer" )
endif()


if (BUILD_TEST)
    file(GLOB files_test
        TestCSharp/*.cs
        TestCSharp/Misc/*.cs
        TestCSharp/Tests/*.cs
    )
    add_executable(TestCSharp ${files_test})
    
    set_target_properties(TestCSharp PROPERTIES VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.Text.RegularExpressions;Microsoft.CSharp;System.IO.Compression")
    add_dependencies(TestCSharp EffekseerCore)
    target_link_libraries(TestCSharp EffekseerCore)

    if (MSVC)
        set_target_properties(TestCSharp PROPERTIES OUTPUT_NAME_DEBUG "Effekseer.TestCSharp" )
        set_target_properties(TestCSharp PROPERTIES OUTPUT_NAME_RELEASE "Effekseer.TestCSharp" )
    endif()
endif()

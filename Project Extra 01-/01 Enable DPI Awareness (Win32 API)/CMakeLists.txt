add_executable(01_Enable_DPI_Awareness_Win32_API WIN32)
set_project_target_common_properties(01_Enable_DPI_Awareness_Win32_API)
target_sources(01_Enable_DPI_Awareness_Win32_API PRIVATE
    Main.cpp
    HighDPI.hpp
    HighDPI.cpp
)
set_target_properties(01_Enable_DPI_Awareness_Win32_API PROPERTIES FOLDER "Project Extra 01-")
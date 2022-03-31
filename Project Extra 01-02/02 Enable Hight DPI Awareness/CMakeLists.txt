add_executable(02_Enable_Hight_DPI_Awareness WIN32)
set_project_target_common_properties(02_Enable_Hight_DPI_Awareness)
target_sources(02_Enable_Hight_DPI_Awareness PRIVATE
    main.cpp
)
#project_target_copy_to_output_directory(02_Enable_Hight_DPI_Awareness 02_Enable_Hight_DPI_Awareness)
set_target_properties(02_Enable_Hight_DPI_Awareness PROPERTIES FOLDER "Project Extra 01-02")
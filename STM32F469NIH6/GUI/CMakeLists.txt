SET(SOURCES screens/ui_Main.c
    screens/ui_Settings.c
    screens/ui_Statistics.c
    screens/ui_Profiles.c
    screens/ui_Status.c
    screens/ui_Calander.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_habonero_icon_scaled_png.c)

add_library(ui ${SOURCES})

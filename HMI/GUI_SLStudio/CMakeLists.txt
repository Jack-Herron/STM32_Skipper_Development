SET(SOURCES screens/ui_Main.c
    screens/ui_Settings.c
    screens/ui_Statistics.c
    screens/ui_Status.c
    screens/ui_Calendar.c
    screens/ui_Profiles.c
    screens/ui_Water.c
    screens/ui_Tools.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_carolina_reaper_icon_250x250_png.c
    images/ui_img_onion_icon_250x250_png.c
    images/ui_img_pause_icon_60x60_png.c
    images/ui_img_play_icon_60x60_png.c
    images/ui_img_toolbox_icon_60x60_png.c
    images/ui_img_statistics_icon_60x60_png.c
    images/ui_img_settings_icon_60x60_png.c
    images/ui_img_carolina_reaper_icon_130x130_png.c
    images/ui_img_onion_icon_130x130_png.c
    images/ui_img_duck_80x80_png.c)

add_library(ui ${SOURCES})

#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/text-lookup.h>
#include <util/util.hpp>
#include <QMainWindow>
#include <QApplication>
#include <QMessageBox>
#include "timer-dock.hpp"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-speech-timer", "en-US")

static lookup_t *lookup = nullptr;

MODULE_EXPORT bool obs_module_load(void)
{
    blog(LOG_INFO, "[obs-speech-timer] Loading plugin version %s", "1.0.0");
    
    try {
        if (!qApp) {
            blog(LOG_ERROR, "[obs-speech-timer] Qt Application not initialized");
            return false;
        }

        blog(LOG_INFO, "[obs-speech-timer] Qt version: %s", qVersion());

        blog(LOG_INFO, "[obs-speech-timer] Attempting to push UI translation...");
        obs_frontend_push_ui_translation(obs_module_get_string);

        blog(LOG_INFO, "[obs-speech-timer] Getting main window...");
        void* main_window = obs_frontend_get_main_window();
        if (!main_window) {
            blog(LOG_ERROR, "[obs-speech-timer] Failed to get main window");
            return false;
        }

        blog(LOG_INFO, "[obs-speech-timer] Creating timer dock");
        TimerDock* dock = new TimerDock(static_cast<QWidget*>(main_window));
        if (!dock) {
            blog(LOG_ERROR, "[obs-speech-timer] Failed to create timer dock");
            return false;
        }

        blog(LOG_INFO, "[obs-speech-timer] Setting dock properties");
        dock->setObjectName("SpeechTimerDock");
        dock->setWindowTitle("Speech Timer");

        blog(LOG_INFO, "[obs-speech-timer] Adding dock to frontend");
        try {
            if (!obs_frontend_add_dock(dock)) {
                blog(LOG_ERROR, "[obs-speech-timer] Failed to add dock to frontend");
                delete dock;
                return false;
            }
            blog(LOG_INFO, "[obs-speech-timer] Dock added successfully");
        } catch (const std::exception& e) {
            blog(LOG_ERROR, "[obs-speech-timer] Failed to add dock: %s", e.what());
            delete dock;
            return false;
        }

        blog(LOG_INFO, "[obs-speech-timer] Plugin loaded successfully");
        return true;
    } catch (const std::exception& e) {
        blog(LOG_ERROR, "[obs-speech-timer] Exception during plugin load: %s", e.what());
        return false;
    } catch (...) {
        blog(LOG_ERROR, "[obs-speech-timer] Unknown exception during plugin load");
        return false;
    }
}

MODULE_EXPORT void obs_module_unload(void)
{
    blog(LOG_INFO, "[obs-speech-timer] Plugin unloaded");
}

MODULE_EXPORT const char *obs_module_name(void)
{
    return "Speech Timer";
}

MODULE_EXPORT const char *obs_module_description(void)
{
    return "A speech timer plugin for OBS Studio";
}

extern "C" EXPORT const char *obs_module_version(void)
{
    return "1.0.0";
}

extern "C" EXPORT const char *obs_module_author(void)
{
    return "Your Name";
} 
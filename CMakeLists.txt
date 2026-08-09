cmake_minimum_required(VERSION 3.10)
project(DetectNotesChord)

# Utiliser C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Trouver Qt6 pour l'interface graphique et multimédia
find_package(Qt6 COMPONENTS Widgets Multimedia REQUIRED)

# Inclure les bibliothèques audio
find_package(FFTW3 REQUIRED)
find_package(SndFile REQUIRED)

# Définir les fichiers sources
add_executable(DetectNotesChord
    main_qt.cpp
    detection_notes.cpp
)

# Lier les bibliothèques nécessaires
target_link_libraries(DetectNotesChord
    Qt6::Widgets
    Qt6::Multimedia
    fftw3
    sndfile
)

# --- Configuration Android ---
if(ANDROID)
    message(STATUS "Compilation pour Android détectée")

    # Définir le nom du package et la version
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    set(ANDROID_ABI "arm64-v8a")
    set(ANDROID_PLATFORM 33)

    # Spécifier les permissions Android
    set_property(TARGET DetectNotesChord PROPERTY ANDROID_PERMISSIONS
        "android.permission.READ_EXTERNAL_STORAGE"
        "android.permission.WRITE_EXTERNAL_STORAGE"
        "android.permission.RECORD_AUDIO"
    )

    # Définir l'icône et le nom de l'application
    set_property(TARGET DetectNotesChord PROPERTY ANDROID_APP_NAME "Detect Notes & Chords")
    set_property(TARGET DetectNotesChord PROPERTY ANDROID_APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.png")
endif()

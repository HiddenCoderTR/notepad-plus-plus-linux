cmake_minimum_required(VERSION 3.16)
project(NotepadPlusPlus)

# Windows, Linux, and macOS share this tree. The application UI is the existing
# Win32 Notepad++ interface; on Unix a GTK-backed compatibility layer implements it.
add_subdirectory(PowerEditor/src)

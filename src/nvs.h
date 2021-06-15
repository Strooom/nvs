#pragma once
#include "Arduino.h"
#include "SPIFFS.h"

class nvsFile {
  public:
    nvsFile(const char *filename);
    static constexpr int maxFilenameLength{32};
    char filename[maxFilenameLength + 1]{};

    int32_t size() const;                                          //
    bool erase() const;                                            // deletes the file - not calling it delete() because this is a reserved keyword in C++
    bool read(char *content, uint32_t contentLength) const;        //
    bool save(const char *content) const;                          //
    void dump() const;                                             //

    static bool isMounted;           // remembers if the fileSystem is mounted
    static bool initialize();        // mount the fileSystem

    static void list();                               // list all present files to the logging
    static void format();                             // format the fileSystem
    static bool exists(const char *aFilename);        //
};

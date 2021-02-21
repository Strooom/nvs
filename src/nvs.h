#pragma once
#include "Arduino.h"

class nvsFile {
  public:
    nvsFile(const char *filename);
    static constexpr int maxFilenameLength{32};
    char filename[maxFilenameLength + 1]{};

    int32_t size() const;                                          //
    bool erase() const;                                            // deletes the settings file - not call it delete() because this is a reserved keyword in C++
    bool read(char *content, uint32_t contentLength) const;        //
    bool save(char *content) const;                                //
    void dump() const;                                             //
};

#pragma once
#include <stdint.h>

class nvsFile {
  public:
    nvsFile(const char *filename);                                 // constructor
    static constexpr int maxFilenameLength{32};                    //
    static bool mount(bool formatOnFail = false);                  // mount the fileSystem
    static bool unMount();                                         // unMount the fileSystem
    static bool isMounted();                                       // is it mounted ?
    static bool format();                                          // format the fileSystem
    static void list();                                            // list all present files to the logging
    static bool exists(const char *aFilename);                     //
    int32_t size() const;                                          //
    bool erase() const;                                            // deletes the file - not calling it delete() because this is a reserved keyword in C++
    bool read(char *content, uint32_t contentLength) const;        //
    bool save(const char *content) const;                          //
    void dump() const;                                             //

#ifndef unitTest
  private:
#endif
    char filename[maxFilenameLength]{};        // fileName stored as c-style string, so needs a terminating zero
    static bool isFilesystemMounted;           // remembers if the fileSystem is mounted
};

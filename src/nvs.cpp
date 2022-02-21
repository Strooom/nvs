#include "Arduino.h"
#include "nvs.h"
#include "logging.h"        // we rely on the logging library to log any errors occuring in the fileSystem during runtime
#include "subsystems.h"

#ifndef fileSystemType
#define fileSystemType SPIFFS
#include <SPIFFS.h>
//#define fileSystemType LittleFS
//#include "FS.h"
//#include <LittleFS.h>
#endif

extern uLog theLog;

bool nvsFile::isFilesystemMounted = false;        // initialize it here because it is a static member

nvsFile::nvsFile(const char* theFilename) {
    strlcpy(filename, theFilename, maxFilenameLength);
}

bool nvsFile::mount(bool formatOnFail) {
    isFilesystemMounted = false;
    if (fileSystemType.begin()) {
        isFilesystemMounted = true;
    } else {
        if (formatOnFail) {
            (void)format();
            if (fileSystemType.begin()) {        // try again to mount, this time after formatting
                isFilesystemMounted = true;
            }
        }
    }
    if (isMounted()) {
        theLog.output(subSystems::filesystem, loggingLevel::Info, "FileSystem Mounted");
    } else {
        theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not mount FileSystem");
    }
    return isFilesystemMounted;
}

bool nvsFile::unMount() {
    fileSystemType.end();
    isFilesystemMounted = false;
    return isFilesystemMounted;
}

bool nvsFile::isMounted() {
    return isFilesystemMounted;
}

bool nvsFile::format() {
    // Important Note : formatting a large partition can take several minutes
    // so be sure to disable the hardware watchdog to prevent the controller to restart in the middle of a lenghty format
    // this might create endless reboot sequence
    bool success{false};
    if (fileSystemType.format()) {
        theLog.output(subSystems::filesystem, loggingLevel::Info, "FileSystem formatted");
        success = true;
    } else {
        theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not format FileSystem");
    }
    return success;
}

int32_t nvsFile::size() const {
    if (fileSystemType.begin(true)) {
        if (fileSystemType.exists(filename)) {
            File theFile = fileSystemType.open(filename);
            if (theFile) {
                int32_t theFilesize = theFile.size();
                theFile.close();
                return theFilesize;
            } else {
                theLog.snprintf(subSystems::filesystem, loggingLevel::Error, "Could not open file %s", filename);
                return -1;
            }
        } else {
            theLog.snprintf(subSystems::filesystem, loggingLevel::Error, "File %s does not exist", filename);
            return -1;
        }
    } else {
        theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not mount FS");
        return -1;
    }
}

bool nvsFile::erase() const {
    if (fileSystemType.begin(true)) {
        if (fileSystemType.exists(filename)) {
            if (fileSystemType.remove(filename)) {
                theLog.snprintf(subSystems::filesystem, loggingLevel::Debug, "Erased file %s", filename);
                return true;
            } else {
                theLog.snprintf(subSystems::filesystem, loggingLevel::Error, "File %s exists but could not erase it", filename);
                return false;
            }
        } else {
            theLog.snprintf(subSystems::filesystem, loggingLevel::Error, "File %s does not exist, cannot erase it", filename);
            return true;        // result is as good as if we deleted the file, so we return 'true'
        }
    } else {
        theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not mount FS");
        return false;
    }
}

bool nvsFile::save(const char* theContent) const {
    if (fileSystemType.begin(true)) {
        if (fileSystemType.exists(filename)) {            // if the file already exist
            if (fileSystemType.remove(filename)) {        // delete the old file first, otherwise writing to it will append..
                theLog.output(subSystems::filesystem, loggingLevel::Debug, "Removed existing file");
            } else {
                theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not delete existing file");
                return false;
            }
        }
        // now write the new one
        File newFile = fileSystemType.open(filename, FILE_WRITE);
        if (newFile) {
            newFile.print(theContent);
            theLog.snprintf(subSystems::filesystem, loggingLevel::Debug, "%s saved", filename);
            newFile.close();
            return true;
        } else {
            theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not create file");
            return false;
        }
    } else {
        theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not mount FS");
        return false;
    }
}

bool nvsFile::read(char* theContent, uint32_t maxContentLength) const {
    if (fileSystemType.begin(true)) {
        if (fileSystemType.exists(filename)) {
            File theFile = fileSystemType.open(filename, FILE_READ);
            if (theFile) {
                int32_t bytesToRead = theFile.size();
                if (bytesToRead > maxContentLength) {
                    theLog.snprintf(subSystems::filesystem, loggingLevel::Warning, "Filesize = %d > buffersize = %d", bytesToRead, maxContentLength);
                    bytesToRead = maxContentLength;
                }
                theFile.read((uint8_t*)theContent, bytesToRead);
                theContent[bytesToRead] = 0x00;
                theFile.close();
                return true;
            } else {
                theLog.snprintf(subSystems::filesystem, loggingLevel::Error, "Cannot open file %s", filename);
                return false;
            }
        } else {
            theLog.snprintf(subSystems::filesystem, loggingLevel::Error, "File %s not found", filename);
            return false;
        }
    } else {
        theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not mount FS");
        return false;
    }
}

void nvsFile::dump() const {
    char tmpString[513];
    read(tmpString, 512);
    theLog.output(subSystems::filesystem, loggingLevel::Debug, tmpString);
}

void nvsFile::list() {
    if (isMounted()) {
        theLog.output(subSystems::filesystem, loggingLevel::Info, "----- File listing --------------------");
        int fileIndex{0};
        File root = fileSystemType.open("/");
        root.rewindDirectory();
        File tmpFile = root.openNextFile();

        while (tmpFile) {
            theLog.snprintf(subSystems::filesystem, loggingLevel::Info, "file[%d] [%s] [%d] bytes", fileIndex, tmpFile.name(), tmpFile.size());
            tmpFile = root.openNextFile();
            fileIndex++;
        }
        theLog.output(subSystems::filesystem, loggingLevel::Info, "---------------------------------------");
    }
}

bool nvsFile::exists(const char* aFilename) {
    if (fileSystemType.begin(true)) {
        if (fileSystemType.exists(aFilename)) {
            return true;
        } else {
            return false;
        }
    } else {
        theLog.output(subSystems::filesystem, loggingLevel::Error, "Could not mount FS");
        return false;
    }
}

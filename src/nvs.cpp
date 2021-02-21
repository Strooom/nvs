#include "Arduino.h"
#include "nvs.h"
#include "SPIFFS.h"
#include "logging.h"

extern uLog theLog;

nvsFile::nvsFile(const char* theFilename) {
    strlcpy(filename, theFilename, maxFilenameLength);
}

int32_t nvsFile::size() const {
    if (SPIFFS.begin(true)) {
        if (SPIFFS.exists(filename)) {
            File theFile = SPIFFS.open(filename);
            if (theFile) {
                int32_t theFilesize = theFile.size();
                theFile.close();
                return theFilesize;
            } else {
                theLog.snprintf(loggingLevel::Error, "Could not open file %s", filename);
                return -1;
            }
        } else {
            theLog.snprintf(loggingLevel::Error, "File %s does not exist", filename);
            return -1;
        }
    } else {
        theLog.output(loggingLevel::Error, "Could not mount SPIFFS");
        return -1;
    }
}

bool nvsFile::erase() const {
    if (SPIFFS.begin(true)) {
        if (SPIFFS.exists(filename)) {
            if (SPIFFS.remove(filename)) {
                theLog.snprintf(loggingLevel::Debug, "Erased file %s", filename);
                return true;
            } else {
                theLog.snprintf(loggingLevel::Error, "File %s exists but could not erase it", filename);
                return false;
            }
        } else {
            theLog.snprintf(loggingLevel::Error, "File %s does not exist, cannot erase it", filename);
            return true;        // result is as good as if we deleted the file, so we return 'true'
        }
    } else {
        theLog.output(loggingLevel::Error, "Could not mount SPIFFS");
        return false;
    }
}

bool nvsFile::save(char* theContent) const {
    if (SPIFFS.begin(true)) {
        if (SPIFFS.exists(filename)) {            // if the file already exist
            if (SPIFFS.remove(filename)) {        // delete the old file first, otherwise writing to it will append..
                theLog.output(loggingLevel::Debug, "Removed existing file");
            } else {
                theLog.output(loggingLevel::Error, "Could not delete existing file");
                return false;
            }
        }
        // now write the new one
        File newFile = SPIFFS.open(filename, FILE_WRITE);
        if (newFile) {
            newFile.print(theContent);
            theLog.snprintf(loggingLevel::Debug, "%s saved", filename);
            newFile.close();
            return true;
        } else {
            theLog.output(loggingLevel::Error, "Could not create file");
            return false;
        }
    } else {
        theLog.output(loggingLevel::Error, "Could not mount SPIFFS");
        return false;
    }
}

bool nvsFile::read(char* theContent, uint32_t maxContentLength) const {
    if (SPIFFS.begin()) {
        if (SPIFFS.exists(filename)) {
            File theFile = SPIFFS.open(filename, FILE_READ);
            if (theFile) {
                int32_t bytesToRead = theFile.size();
                if (bytesToRead > maxContentLength) {
                    bytesToRead = maxContentLength;
                }
                theFile.read((uint8_t*)theContent, bytesToRead);
                theContent[bytesToRead] = 0x00;
                theFile.close();
                return true;
            } else {
                theLog.snprintf(loggingLevel::Error, "Cannot open file %s", filename);
                return false;
            }
        } else {
            theLog.snprintf(loggingLevel::Error, "File %s not found", filename);
            return false;
        }
    } else {
        theLog.output(loggingLevel::Error, "Could not mount SPIFFS");
        return false;
    }
}

void nvsFile::dump() const {
    char tmpString[513];
    read(tmpString, 512);
    theLog.output(loggingLevel::Debug, tmpString);
}
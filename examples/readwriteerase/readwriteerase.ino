#include "Arduino.h"
#include "nvs.h"
#include "logging.h"

uLog theLog;

void setup() {
    Serial.begin(115200);
    delay(500);
    theLog.setOutputIsAvailable(true);

    nvsFile aFile        = nvsFile("/demo.txt");        // Note how on SPIFFS, files saved in the root need to have / in front of their name
    uint32_t theFileSize = aFile.size();                // size() returns -1 if the file does not exist
    theLog.snprintf(loggingLevel::Debug, "[%s] has size [%d]", aFile.filename, aFile.size());

    char contents[] = "This contents will be written to the file";
    aFile.save(contents);

    theFileSize = aFile.size();
    theLog.snprintf(loggingLevel::Debug, "[%s] has size [%d]", aFile.filename, aFile.size());

    char readContents[128];
    aFile.read(readContents, 128);
    theLog.snprintf(loggingLevel::Debug, "[%s] contains [%s]", aFile.filename, readContents);
}

void loop() {
}
# nvs
Arduino Library to make usage of Non Volatile Storage a bit simpler

When reading or writing to a flash-based filesystem, eg. SPIFFS, you need quite some errorhandling for all things which could go wrong.
All this errorhandling is encapsulated into a wrapper class, making it easier to simply read, save, erase and dump files. If anything goes wrong, it shows up in the logging (see https://github.com/Strooom/logging)

Furthermore, this class abstracts the details from SPIFFS, so I could make a variant for LittleFS without touching the application.

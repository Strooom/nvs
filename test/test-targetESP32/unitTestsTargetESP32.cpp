#define UnitTesting

#include <unity.h>
#include "nvs.h"
#include "logging.h"

uLog theLog;

// ##########################################
// ### Tests                              ###
// ##########################################

void test_initialization() {
    TEST_ASSERT_FALSE(nvsFile::isMounted());        //
    nvsFile::mount(true);                           // Format if needed
    TEST_ASSERT_TRUE(nvsFile::isMounted());         //
    nvsFile::unMount();                             //
    TEST_ASSERT_FALSE(nvsFile::isMounted());        //
}

void test_exists() {
    char aFileName[32]    = "/test.txt";
    char someContents[32] = "Lorem ipsum dolor sit amet";
    nvsFile::format();                                    // format to erase all files
    nvsFile::mount();                                     //
    TEST_ASSERT_FALSE(nvsFile::exists(aFileName));        // should not exist before
    nvsFile aFile(aFileName);                             // create a file object
    aFile.save(someContents);
    TEST_ASSERT_TRUE(nvsFile::exists(aFileName));        // should exist after creating it
}

void test_create_size_save_size_erase() {
    char aFileName[32]    = "/test.txt";
    char someContents[64] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
    nvsFile::mount();                                             //
    nvsFile aFile(aFileName);                                     // create a file object
    aFile.save(someContents);                                     //
    TEST_ASSERT_TRUE(nvsFile::exists(aFileName));                 // should exist after (re-)creating it
    TEST_ASSERT_EQUAL(strlen(someContents), aFile.size());        // check size of the file vs ites contents
    TEST_ASSERT_TRUE(aFile.erase());                              // try to erase the file..
    TEST_ASSERT_FALSE(nvsFile::exists(aFileName));                // should no longer exist after erasing it
}

void test_edge_cases() {
    char aFileName[32]    = "/test.txt";
    char someContents[64] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
    nvsFile::mount();                                             //
    nvsFile aFile(aFileName);                                     // create a file object
    aFile.save(someContents);                                     //
    TEST_ASSERT_TRUE(nvsFile::exists(aFileName));                 // should exist after (re-)creating it
    TEST_ASSERT_EQUAL(strlen(someContents), aFile.size());        // check size of the file vs its contents
    TEST_ASSERT_TRUE(aFile.erase());                              // try to erase the file..
    TEST_ASSERT_FALSE(nvsFile::exists(aFileName));                // should no longer exist after erasing it
}

// ##########################################
// ### Test Runner                        ###
// ##########################################

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_initialization);
    RUN_TEST(test_exists);
    RUN_TEST(test_create_size_save_size_erase);
    RUN_TEST(test_edge_cases);
    UNITY_END();
}

void loop() {
}

//
// Created by akgunduz on 23.10.2015.
//

#include "Job.h"

Job::Job(const ComponentObject& host, const char* jobZipFile, const char* rootPath)
    : host(host) {

    char jobName[NAME_MAX];

    if (!extract(jobZipFile, rootPath, jobName)) {
        LOGS_E(getHost(), "Job can not extracted from Zip file!!!");
        return;
    }

    jobFile = new JobItem(host, jobName, JOB_FILE);

    if (!createDependencyMap()){
        LOGS_E(getHost(), "Dependency Loop Detected in the Job!!!");
        return;
    }
}

Job::~Job() {

    delete jobFile;
}


bool Job::extract(const char *zipFile, const char* rootPath, char *jobName) {

    mz_zip_archive zip_archive;
    mz_zip_archive_file_stat file_stat;
    char absPath[PATH_MAX];

    memset(&zip_archive, 0, sizeof(zip_archive));

    mz_bool status = mz_zip_reader_init_file(&zip_archive, zipFile, 0);
    if (!status) {
        LOGS_E(getHost(), "mz_zip_reader_init_file() failed!");
        return false;
    }

    int fileCount = mz_zip_reader_get_num_files(&zip_archive);
    if (fileCount < 1) {
        LOGS_E(getHost(), "mz_zip_reader_get_num_files() failed!");
        mz_zip_reader_end(&zip_archive);
        return false;
    }

    for (int i = 0; i < fileCount; i++) {

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue;
        }

        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            LOGS_E(getHost(), "mz_zip_reader_file_stat() failed!");
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        if (strcmp(file_stat.m_filename, "Job.json") != 0) {
            continue;
        }

        size_t unCompSize = file_stat.m_uncomp_size;
        const char *buffer = (const char *) mz_zip_reader_extract_to_heap(&zip_archive, i, &unCompSize, 0);
        if (!buffer) {
            LOGS_E(getHost(), "mz_zip_reader_extract_to_heap() failed!");
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        struct json_object *node = json_tokener_parse(buffer);
        if (node == nullptr) {
            LOGS_E(getHost(), "Invalid JSON File");
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        auto *header = (struct json_object *) json_object_get_object(node)->head->v;

        json_object_object_foreach(header, key, val) {

            if (strcmp(key, "name") == 0) {

                enum json_type type = json_object_get_type(val);
                if (type != json_type_string) {
                    LOGS_E(getHost(), "Invalid JSON Name Node");
                    mz_zip_reader_end(&zip_archive);
                    return false;
                }

                strcpy(jobName, json_object_get_string(val));
                break;
            }
        }

        break;
    }

    sprintf(absPath, "%s/%s", rootPath, jobName);

    Util::removePath(absPath);

    for (int i = 0; i < fileCount; i++) {

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue;
        }

        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            printf("mz_zip_reader_file_stat() failed!\n");
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        sprintf(absPath, "%s/%s/%s", rootPath, jobName, file_stat.m_filename);

        Util::mkPath(absPath);

        status = mz_zip_reader_extract_to_file(&zip_archive, i, absPath, 0);
        if (!status) {
            printf("mz_zip_reader_extract_file_to_file() failed!\n");
            mz_zip_reader_end(&zip_archive);
            return false;
        }
    }

    mz_zip_reader_end(&zip_archive);

    return true;
}

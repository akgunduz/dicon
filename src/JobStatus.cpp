//
// Created by z003sc8r on 8/11/2020.
//

#include "JobStatus.h"

const char* sJobStatus[] = {
        "OK",
        "PATH_INVALID",
        "ZIP_INVALID",
        "RULES_INVALID",
        "DEPENDENCY_LOOP",
        "MISSING_FILES",
};

const char* JobStatus::getDesc(JOB_STATUS status) {

    return sJobStatus[status];
}
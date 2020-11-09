//
// Created by Haluk AKGUNDUZ on 11.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_JOBSTATUS_H
#define DICON_JOBSTATUS_H

enum JOB_STATUS {

    JOBSTATUS_OK,
    JOBSTATUS_PATH_INVALID,
    JOBSTATUS_ZIP_INVALID,
    JOBSTATUS_RULES_INVALID,
    JOBSTATUS_DEPENDENCY_LOOP,
    JOBSTATUS_MISSING_FILES,
    JOBSTATUS_MAX,
};

class JobStatus {

    static constexpr const char* sJobStatus[JOBSTATUS_MAX] = {
            "OK",
            "PATH_INVALID",
            "ZIP_INVALID",
            "RULES_INVALID",
            "DEPENDENCY_LOOP",
            "MISSING_FILES",
    };

public:

    static const char* getDesc(JOB_STATUS _status) {

        return sJobStatus[_status];
    }
};

#endif //DICON_JOBSTATUS_H

//
// Created by Haluk AKGUNDUZ on 30.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_USERINTERFACECONTROLLER_H
#define BANKOR_USERINTERFACECONTROLLER_H

#include "UserInterfaceEvent.h"

typedef void (*TypeUIUpdateCB)(void*, int, void*);

enum UI_UPDATE {

    UI_UPDATE_DIST_COLL_LIST,
    UI_UPDATE_DIST_NODE_LIST,

    UI_UPDATE_COLL_ID,
    UI_UPDATE_COLL_FILE_LIST,
    UI_UPDATE_COLL_FILE_LISTITEM,
    UI_UPDATE_COLL_PROCESS_LIST,
    UI_UPDATE_COLL_PROCESS_LISTITEM,

    UI_UPDATE_NODE_ID,
    UI_UPDATE_NODE_STATE,
    UI_UPDATE_NODE_PROCESS_LIST,
    UI_UPDATE_NODE_CLEAR,

    UI_UPDATE_MAX
};

class UserInterfaceController {

private:

    void *uiContext;
    TypeUIUpdateCB uiUpdateCB;
    static UserInterfaceController *instance;

    UserInterfaceController(void *, TypeUIUpdateCB);

public:

    static UserInterfaceController* newInstance(void *, TypeUIUpdateCB);
    void updateUI(int, UserInterfaceEvent*);

    template <typename T, typename _ = void>
    struct is_vector {
        static const bool value = false;
    };
    template <typename T>
    struct is_vector< T,
            typename std::enable_if<
                    std::is_same<T, std::vector< typename T::value_type, typename T::allocator_type >
                    >::value
            >::type
    >
    {
        static const bool value = true;
    };

    template<typename T, typename... Args>
    void display(int id, T&& var, Args&&... args) {

        auto *event = new UserInterfaceEvent(id);
        if (is_vector<T>::value) {
            event->addDataList(var);
        }

        //std::tuple<Args> t = args;
    }

//    void display(int, std::vector<long>);
//    void display(int, std::vector<long>, const char* , ...);
//    void display(int, const char* , ...);
//    void display(int, int, const char* , const char*);
//    void display(int, int, long, const char* , const char*);
//    void display(int, int, const char* , long);
//    void display(int, int, void*);
//    void display(int, void*);
};


#endif //BANKOR_USERINTERFACECONTROLLER_H

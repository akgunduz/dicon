//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_PARAMETERITEM_H
#define DICON_PARAMETERITEM_H

#include "ContentItem.h"

class ParameterItem : public ContentItem {

	std::string param;

public:

	ParameterItem(const TypeHostUnit&, long, long, const std::string&);

	~ParameterItem() override{};

    const std::string& get();

	void set(const std::string&);

	virtual CONTENT_TYPES getType();

    virtual bool check();
};

typedef std::shared_ptr<ParameterItem> TypeParameterItem;
typedef std::vector<TypeParameterItem> TypeParameterList;

#endif //DICON_PARAMETERITEM_H

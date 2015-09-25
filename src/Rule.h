//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Rule_H_
#define __Rule_H_

#include "Log.h"
#include "Content.h"

#define RULE_HEADER "Rule"
#define SRULE_RUNTYPE "runtype"
#define SRULE_FILES "files"
#define SRULE_PARAMETERS "parameters"
#define SRULE_EXECUTORS "executors"

class Rule;

typedef bool (Rule::*fRuleParser)(json_object *);

enum RULE_TYPES {
	RULE_RUNTYPE,
	RULE_FILES,
	RULE_PARAMETERS,
	RULE_EXECUTORS,
	RULE_MAX
};

enum RUN_TYPE {
	RUN_SEQUENTIAL,
	RUN_PARALLEL,
	RUN_MAX
};

static std::string sRuleTypes[RULE_MAX] = {
	SRULE_RUNTYPE,
	SRULE_FILES,
	SRULE_PARAMETERS,
	SRULE_EXECUTORS
};

class Rule {

	FileContent* mRuleFile;
	std::string mRootPath;
	bool mIsValid;
	enum RUN_TYPE mRunType;

	std::map<std::string, RULE_TYPES> mRuleMap = {
			{sRuleTypes[RULE_RUNTYPE], RULE_RUNTYPE},
			{sRuleTypes[RULE_FILES], RULE_FILES},
			{sRuleTypes[RULE_PARAMETERS], RULE_PARAMETERS},
			{sRuleTypes[RULE_EXECUTORS], RULE_EXECUTORS}
	};
	std::vector<Content*> mContentList[RULE_MAX];

	fRuleParser parserNodeList[RULE_MAX] = {
			&Rule::parseRunTypeNode,
			&Rule::parseFileNode,
			&Rule::parseParamNode,
			&Rule::parseExecutorNode
	};

	bool readFile(const std::string &, char **);
	bool parseBuffer(char *buf);
	bool parseRunTypeNode(json_object *node);
	bool parseFileNode(json_object *node);
	bool parseParamNode(json_object *node);
	bool parseExecutorNode(json_object *node);

public:

	Rule(const std::string&, const std::string&);
	~Rule();
	bool isValid();
	RUN_TYPE getRunType();
	FileContent* getRuleFile();
	std::string getRootPath();
	Content* getContent(RULE_TYPES type, int index);
	ssize_t getContentCount(RULE_TYPES type);
	ssize_t getFlaggedFileCount();
	bool updateFileContent(FileContent*);
	bool addContent(RULE_TYPES, Content *);
	void reset();
	void display();

};


#endif //__Rule_H_

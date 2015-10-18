//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Rule_H_
#define __Rule_H_

#include "Log.h"
#include "FileContent.h"
#include "Unit.h"
#include "RuleTypes.h"

#define RULE_HEADER "Rule"

class Rule;

typedef bool (Rule::*fRuleParser)(json_object *);

class Rule {

	Unit unitHost;
	Unit unitNode;
	FileContent* content;
	char rootPath[PATH_MAX];
	bool valid;
	bool parallel;

	std::map<std::string, RULE_TYPES> ruleMap = {
			{RuleTypes::getName(RULE_RUNTYPE), RULE_RUNTYPE},
			{RuleTypes::getName(RULE_FILES), RULE_FILES},
			{RuleTypes::getName(RULE_PARAMETERS), RULE_PARAMETERS},
			{RuleTypes::getName(RULE_EXECUTORS), RULE_EXECUTORS}
	};
	std::vector<Content*> contentList[RULE_MAX];

	fRuleParser parserNodeList[RULE_MAX] = {
			&Rule::parseRunTypeNode,
			&Rule::parseFileNode,
			&Rule::parseParamNode,
			&Rule::parseExecutorNode
	};

	bool readFile(const char*, char **);
	bool parseBuffer(char *buf);
	bool parseRunTypeNode(json_object *node);
	bool parseFileNode(json_object *node);
	bool parseParamNode(json_object *node);
	bool parseExecutorNode(json_object *node);

public:

	Rule(Unit host, Unit node, const char*, FileContent* content = nullptr);
	~Rule();
	bool isParallel();
	FileContent* getContent();
	const char* getRootPath();
	Content* getContent(RULE_TYPES type, int index);
	int getContentCount(RULE_TYPES type);
	int getFlaggedFileCount();
	void reset();
	void display();
};


#endif //__Rule_H_

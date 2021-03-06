/***********************************************************************************
*  Copyright(c) 2015     wu.2724@osu.edu
*  All rights reserved.
*
*  File name:   lispparser
*  Description: This is the class LispParser, which mainly realizes the funtion of
*  lexical and syntatic analysis, and output the results in required format.
*
*  Date: 10/01/2015
*  Modified : 10/27/2015    description: fixed bug
*  Author: Longhua Wu
*************************************************************************************/
#ifndef LISPPARSER_H
#define LISPPARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <locale>
#include <iterator>
#include <cstdlib>
#include <stdexcept>
#include <map>

using namespace std;

enum valueType { BOOL_TYPE, INT_TYPE, NO_TYPE};

struct NodeValue
{
    int intValue;
    bool boolValue;
    valueType vType;
    NodeValue() : vType(NO_TYPE) {}
};

struct TreeNode
{
    string expr;
    TreeNode *left;
    TreeNode *right;
    bool isList;
    NodeValue nodeValue;
    TreeNode() : left(NULL),right(NULL),isList(false) {}
    TreeNode(string x) : expr(x), left(NULL), right(NULL),isList(false) {}
};

class LispParser
{
//FUNCTION
public:
    LispParser();
    ~LispParser();
    void buildBinaryTree(string textline, size_t &curIdx, TreeNode *node);
    void checkInnerNodesList(TreeNode *node);
	bool IsList(TreeNode *node);
    // Always call function checkInnerNodesList before get IsAllList
    bool getIsAllList() {return isAllList;}
    void resetStatus();
    void deleteBinaryTree(TreeNode*node);
    void printExpr(TreeNode *node);
    void updateIsList(TreeNode *node);
	void clearListMap(map<string, TreeNode*>& listmap);
    string int2str(int num);
    void copyTree(TreeNode *node, TreeNode *node_cpy);
    bool checkIsInnerNode(TreeNode *node) {return (node->left != NULL || node->right != NULL);}

    void testPrint(TreeNode *node, vector<int> orders);
    void testPrintDList();
	void testPrintAList(map<string, TreeNode*>& alistMap);
public:
    map<string,TreeNode*> dlistMap;
    //map<string,TreeNode*> alistMap;

private:
    string getNextToken(string textline, size_t &curIdx);
    void printNodeExpr(TreeNode *node);
    void printListExpr(TreeNode *node);

//CLASS MEMBER
private:
    bool hasLetter;           // determine whether an atom has letter.
    bool hasMinusSign;
    bool isAllList;           // determine whether inner nodes' isList attributes are all true
                              // if it is true, print out in list notation,otherwise dot notation
};

#endif // LISPPARSER_H

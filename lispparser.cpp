#include "lispparser.h"

LispParser::LispParser():hasLetter(false),hasMinusSign(false),isAllList(true)
{

}

void LispParser::resetStatus()
{
    hasLetter = false;
    hasMinusSign = false;
    isAllList = true;
}

string LispParser::getNextToken(string textline,size_t & curIdx)
{
    string strToken;
    size_t strLen = textline.length();
    size_t subStrLen = 0;
    size_t beginIdx = curIdx;
    hasLetter = false;
    hasMinusSign = false;
    try{
        while(curIdx < strLen)
        {
            // handle white space
            if(isspace(textline[curIdx]))
            {
                if(subStrLen > 0)  // if the content before white space has numalpha, current token is ended
                    break;
                else
                    beginIdx = ++curIdx;
            }
            else if(textline[curIdx] == '(' ||
                    textline[curIdx] == ')' ||
                    textline[curIdx] == '.' )
            {
                if(subStrLen == 0)
                {
                    strToken = string(1,textline[curIdx]);
                    ++ curIdx;
                }
                break;
            }
            else if(isalnum(textline[curIdx]))
            {
                //ERROR: Unacceptable Symbol---letter in lower case.
                if(islower(textline[curIdx]))
                    throw runtime_error("US_LL");
                if(!hasLetter && isalpha(textline[curIdx]))
                    hasLetter = true;

                ++ curIdx;
                ++ subStrLen;
            }
            else if(textline[curIdx] == '-')
            {
                if(hasMinusSign)
                {
                    throw runtime_error("IM");
                }
                else
                {
                    if(beginIdx == curIdx)
                        hasMinusSign = true;
                    else
                        throw runtime_error("IM");
                    ++ curIdx;
                    ++ subStrLen;
                }
            }
            else
            {   //ERROR: Unacceptable Symbol!
                throw runtime_error("US");
            }
        }
        if(subStrLen > 0)
        {
            strToken = textline.substr(beginIdx,subStrLen);
            // ERROR: Invalid atom which contains letter beginning with digit.
            if(hasLetter && isdigit(textline[beginIdx]))
                throw runtime_error("ID");
            if(hasMinusSign && subStrLen == 1)
                throw runtime_error("IM");
        }
    }catch(runtime_error re)
    {
        if(re.what() == string("US"))
            cout << "ERROR: Unaccecptable symbol."<< endl;
        else if(re.what() == string("US_LL"))
            cout << "ERROR: Unacceptable symbol-letter in lower case."<<endl;
        else if(re.what() == string("ID"))
            cout << "ERROR: Invalid atom which contains letter beginning with digit." <<endl;
        else if(re.what() == string("IM"))
            cout << "ERROR: Invalide atom with minus sign at wrong position.";
        exit(0);
    }
    return strToken;
}

void LispParser::buildBinaryTree(string textline, size_t &curIdx, TreeNode *node)
{
    string tk = getNextToken(textline,curIdx);
    try{
        if(tk == string(".") || tk == string(")"))
        {
            //ERROR: missing atom or openparenthesis.
            throw runtime_error("MAO");
        }
        else
        {
            if(tk == string("("))
            {
                TreeNode *lchild = new TreeNode();
                TreeNode *rchild = new TreeNode();
                node->left = lchild;
                node->right = rchild;
                lchild->rSibling = rchild;
                buildBinaryTree(textline,curIdx,lchild);
                //ERROR: missing dot symbol
                if(getNextToken(textline,curIdx) != string("."))
                    throw runtime_error("MDOT");
                buildBinaryTree(textline,curIdx,rchild);
                if(rchild->isList)
                    node->isList = true;
                //ERROR: missing closeparenthesis
                if(getNextToken(textline,curIdx) != string(")"))
                    throw runtime_error("MCLO");
            }
            else
            {
                node->expr = tk;
                if(tk == string("NIL"))
                    node->isList = true;
                return;
            }
        }
    }catch(runtime_error re)
    {
        if(re.what()==string("MDOT"))
            cout << "ERROR: missing dot symbol" <<endl;
        else if(re.what()==string("MCLO"))
            cout << "ERROR: missing closeparenthesis" <<endl;
        else if(re.what()==string("MAO"))
            cout << "ERROR: missing atom or openparenthesis"<<endl;
        exit(0);
    }
}

void LispParser::deleteBinaryTree(TreeNode *node)
{
  if(node->left != NULL)
      deleteBinaryTree(node->left);
  if(node->right != NULL)
      deleteBinaryTree(node->right);
  if(node->left == NULL && node->right == NULL)
      delete node;
}

void LispParser::checkInnerNodesList(TreeNode *node)
{
    if(checkIsInnerNode(node))
    {
        if(!node->isList)
            isAllList = false;
        else
        {
            if(isAllList)
                checkInnerNodesList(node->left);
            if(isAllList)
                checkInnerNodesList(node->right);
        }
    }
}

void LispParser::printExpr(TreeNode *node)
{
    checkInnerNodesList(node);
    if(getIsAllList())
        printListExpr(node);
    else
        printNodeExpr(node);
    evaluateExpr(node);
    cout << endl << node->nodeValue.intValue;
}

void LispParser::printNodeExpr(TreeNode *node)
{
    if(checkIsInnerNode(node))
    {
        cout << "(";
        printNodeExpr(node->left);
        cout << " . ";
        printNodeExpr(node->right);
        cout << ")";
    }
    else
    {
        cout << node->expr;
    }
}

void LispParser::printListExpr(TreeNode *node)
{
    if(checkIsInnerNode(node))
    {
        cout << "(";
        printListExpr(node->left);
        while(checkIsInnerNode(node->right))
        {
            node = node->right;
            cout << " ";
            printListExpr(node->left);
        }
        cout << ")";
    }
    else
    {
        cout << node->expr;
    }
}

NodeValue LispParser::evaluateExpr(TreeNode *node)
{
    NodeValue node_v;
    if(!checkIsInnerNode(node))  // exp is an atom (leaf node)
    {
        if(node->expr == string("T"))
        {
            node_v.vType = BOOL_TYPE;
            node_v.boolValue = true;
        }
        else if(node->expr == string("NIL"))
        {
            node_v.vType = BOOL_TYPE;
            node_v.boolValue = false;
        }
        else if(node->expr.find_first_not_of("-0123456789") == string::npos) // the atom represents a number
        {
            node_v.vType = INT_TYPE;
            node_v.intValue = stoi(node->expr);
        }
        else if(node->expr == string("PLUS"))
        {
            node_v.intValue = evaluateExpr(node->rSibling->left).intValue + evaluateExpr(node->rSibling->right).intValue;
            node_v.vType = INT_TYPE;
        }
        else if(node->expr == string("MINUS"))
        {
            node_v.intValue = evaluateExpr(node->rSibling->left).intValue - evaluateExpr(node->rSibling->right).intValue;
            node_v.vType = INT_TYPE;
        }
        else if(node->expr == string("TIMES"))
        {
            node_v.intValue = evaluateExpr(node->rSibling->left).intValue * evaluateExpr(node->rSibling->right).intValue;
            node_v.vType = INT_TYPE;
        }
        else if(node->expr == string("QUOTIENT"))
        {
            node_v.intValue = evaluateExpr(node->rSibling->left).intValue / evaluateExpr(node->rSibling->right).intValue;
            node_v.vType = INT_TYPE;
        }
        else if(node->expr == string("REMAINDER"))
        {
            node_v.intValue = evaluateExpr(node->rSibling->left).intValue % evaluateExpr(node->rSibling->right).intValue;
            node_v.vType = INT_TYPE;
        }
        else
            throw runtime_error("IAT"); // Error: Invalid Atom
    }
    else
    {
          node->nodeValue = evaluateExpr(node->left);
          node_v = node->nodeValue;
    }
    return node_v;
}

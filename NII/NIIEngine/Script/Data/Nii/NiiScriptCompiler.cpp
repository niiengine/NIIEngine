/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#include "NiiPreProcess.h"
#include "NiiScriptCompiler.h"
#include "NiiScriptCompilerManager.h"
#include "NiiScriptHelperManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiLogManager.h"
#include "NiiScriptAnalyze.h"

namespace NII
{
    ScriptError::ErrorMap ScriptError::mErrorMap;
    //-------------------------------------------------------------------
    //-------------------------------------------------------------------
    // ScriptError
    //-------------------------------------------------------------------
    //-------------------------------------------------------------------
    ScriptError::ScriptError(ExpectType type, const String & file, NIIi line, const String & msg) :
        mErrorNo(type),
        mFile(file),
        mLine(line),
        mMsg(msg)
    {
    }
    //-------------------------------------------------------------------
    const String & ScriptError::getErrorStr(ExpectType type)
    {
        N_assert(type <= ET_ObjExistError, "error");
        return mErrorMap[type];
    }
    //-------------------------------------------------------------------
    //-------------------------------------------------------------------
    // ScriptCompiler
    //-------------------------------------------------------------------
    //-------------------------------------------------------------------
    ScriptCompiler::ScriptCompiler()
    {
    }
    //-------------------------------------------------------------------
    ScriptCompiler::~ScriptCompiler()
    {
    }
    //-------------------------------------------------------------------
    bool ScriptCompiler::compile(const String & str, const String & source, GroupID gid)
    {
        clear();
        mMapValue.clear();
        mGroup = gid;

        ScriptNodeList * snlist;
        TokenNodeList * nodes;

        ScriptLexerParser parser(this, source);
        parser.parse(str, nodes);
        ScriptLexerParser::create(snlist);
        parser.analyze(*nodes, snlist);

        // Processes the imports for this script
        processExtern(snlist);
        // Process object inheritance
        processObject(snlist, snlist);
        // Process variable expansion
        processRefValue(snlist);

        ScriptNodeList::iterator i, iend = snlist->end();
        for(i = snlist->begin(); i != iend; ++i)
        {
            if((*i)->mType != SNT_Object || reinterpret_cast<ObjectScriptNode *>(*i)->mBaseObj)
                continue;

            ScriptAnalyze * analyze = N_Only(ScriptCompiler).create(this,
                N_Only(ScriptHelper).getUnit(mLanguage, static_cast<ObjectScriptNode *>(*i)->mClass), mLanguage);
            if(analyze)
            {
                analyze->analyze(*i);
                N_Only(ScriptCompiler).destroy(analyze);
            }
        }

        destroyExtern();

        ScriptLexerParser::free(nodes);
        ScriptLexerParser::destroy(snlist);

        return mErrors.empty();
    }
    //-------------------------------------------------------------------------------------
    void ScriptCompiler::error(const ScriptNode & node, ScriptError::ExpectType code, const String & msg)
    {
        ScriptError * error = N_new ScriptError(code, node.mFile, node.mLine, msg);

        String str = _T("Compiler error: ");
        str = str + ScriptError::getErrorStr(code) + _T(" in ") + node.mFile +
            _T("(") + N_conv(node.mLine) + _T(")");
        if(!msg.empty())
            str = str + _T(": ") + msg;
        N_Only(Log).log(str);

        mErrors.push_back(error);
    }
    //-------------------------------------------------------------------------------------
    void ScriptCompiler::error(const TokenNode & node, ScriptError::ExpectType code, const String & msg)
    {
        ScriptError * error = N_new ScriptError(code, node.mFile, node.mLineNo, msg);

        String str = _T("Compiler error: ");
        str = str + ScriptError::getErrorStr(code) + _T(" in ") + node.mFile +
            _T("(") + N_conv(node.mLineNo) + _T(")");
        if (!msg.empty())
            str = str + _T(": ") + msg;
        N_Only(Log).log(str);

        mErrors.push_back(error);
    }
    //------------------------------------------------------------------
    void ScriptCompiler::clear()
    {
        ErrorList::iterator i, iend = mErrors.end();
        for(i = mErrors.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mErrors.clear();
    }
    //------------------------------------------------------------------
    void ScriptCompiler::addEnvValue(const String & name, const String & value)
    {
        mMapValue.insert(Npair(name, value));
    }
    //------------------------------------------------------------------
    GroupID ScriptCompiler::getResourceGroup() const
    {
        return mGroup;
    }
    //------------------------------------------------------------------
    void ScriptCompiler::processLanguage(ScriptNodeList * nodes)
    {
    }
    //--------------------------------------------------------------------------
    ScriptNodeList * ScriptCompiler::loadExtern(const String & name)
    {
        ScriptNodeList * re = 0;
        TokenNodeList * nodes = 0;
        ScriptLexerParser parser(this, name);
        if(N_OnlyPtr(ResourceScheme))
        {
            DataStream * stream = N_Only(ResourceScheme).open(name, mGroup);
            if(stream != 0)
            {
                Nui8 * tempdata;
                NCount dsize = stream->cloneData(tempdata);

                String temp((const Ntchar*)tempdata, dsize);
                parser.parse(temp, nodes);

                N_free(tempdata);
                N_delete stream;
            }
        }

        if(nodes != 0)
        {
            ScriptLexerParser::create(re);
            parser.analyze(*nodes, re);
            parser.free(nodes);
        }

        if(!re->empty())
        {
            return re;
        }

        ScriptLexerParser::destroy(re);
        return 0;
    }
    //------------------------------------------------------------------
    void ScriptCompiler::processExtern(ScriptNodeList * nodes)
    {
        // We only need to iterate over the top-level of nodes
        ScriptNodeList::iterator i = nodes->begin();
        while(i != nodes->end())
        {
            // We move to the next node here and save the current one.
            // If any replacement happens, then we are still assured that
            // i points to the node *after* the replaced nodes, no matter
            // how many insertions and deletions may happen
            ScriptNodeList::iterator cur = i++;
            if((*cur)->mType == SNT_Extern)
            {
                ExternScriptNode * import = static_cast<ExternScriptNode *>(*cur);
                if(mExternNodeList.find(import->mFile) == mExternNodeList.end())
                {
                    // Load the script
                    ScriptNodeList * inport = loadExtern(import->mFile);
                    if(inport != 0)
                    {
                        processExtern(inport);
                        processObject(inport, inport);
                        mExternNodeList.insert(Npair(import->mFile, inport));
                    }
                }

                // Handle the target request now
                // If it is a '*' import we remove all previous requests and just use the '*'
                // Otherwise, ensure '*' isn't already registered and register our request
                if(import->mTarget == _T("*"))
                {
                    mExternList.erase(mExternList.lower_bound(import->mFile),
                        mExternList.upper_bound(import->mFile));
                    mExternList.insert(Npair(import->mFile, _T("*")));
                }
                else
                {
                    ExternList::iterator iter = mExternList.lower_bound(import->mFile),
                        end = mExternList.upper_bound(import->mFile);
                    if(iter == end || iter->second != _T("*"))
                    {
                        mExternList.insert(Npair(import->mFile, import->mTarget));
                    }
                }
                N_delete *cur;
                nodes->erase(cur);
            }
        }

        // All import nodes are removed
        // We have cached the code blocks from all the imported scripts
        // We can process all import requests now
        ExternNodeList::iterator z, zend = mExternNodeList.end();
        for(z = mExternNodeList.begin(); z != zend; ++z)
        {
            ExternList::iterator j = mExternList.lower_bound(z->first),
                end = mExternList.upper_bound(z->first);
            if(j != end)
            {
                if(j->second == _T("*"))
                {
                    // Insert the entire AST into the import table
                    mExternTable.insert(mExternTable.begin(), z->second->begin(), z->second->end());
                    continue; // Skip ahead to the next file
                }
                else
                {
                    for(; j != end; ++j)
                    {
                        // Locate this target and insert it into the import table
                        ScriptNode * node = findObj(z->second, j->second);
                        if(node != 0)
                            mExternTable.insert(mExternTable.begin(), node);
                    }
                }
            }
        }
    }
    //--------------------------------------------------------------------------------
    ScriptNode * ScriptCompiler::findObj(ScriptNodeList * nodes, const String & obj)
    {
        ScriptNodeList::iterator i, iend = nodes->end();
        for(i = nodes->begin(); i != iend; ++i)
        {
            if((*i)->mType == SNT_Object)
            {
                if(static_cast<ObjectScriptNode *>(*i)->mName == obj)
                {
                    return *i;
                }
            }
        }
        return 0;
    }
    //---------------------------------------------------------------------------------
    void ScriptCompiler::processObject(ScriptNodeList * nodes, ScriptNodeList * top)
    {
        for(ScriptNodeList::iterator i = nodes->begin(); i != nodes->end(); ++i)
        {
            if((*i)->mType == SNT_Object)
            {
                ObjectScriptNode * obj = static_cast<ObjectScriptNode *>(*i);

                std::vector<String>::const_iterator baseIt, end_it = obj->BaseObjList.end();
                for(baseIt = obj->BaseObjList.begin(); baseIt != end_it; ++baseIt)
                {
                    const String & base = *baseIt;
                    // Check the top level first, then check the import table
                    ScriptNode * newNodes = findObj(top, base);
                    if(newNodes == 0)
                    {
                        newNodes = findObj(&mExternTable, base);
                    }
                    if(newNodes != 0)
                    {
                        inheritProperty(obj, newNodes);
                    }
                }

                // Recurse into children
                processObject(&obj->mChildren, top);

                // Overrides now exist in obj's overrides list. These are non-object nodes which must now
                // Be placed in the children section of the object node such that overriding from parents
                // into children works properly.
                obj->mChildren.insert(obj->mChildren.begin(), obj->mInheritValue.begin(), obj->mInheritValue.end());
            }
        }
    }
    //-------------------------------------------------------------------------------
    void ScriptCompiler::inheritProperty(ObjectScriptNode * dest, const ScriptNode * source)
    {
        if(source->mType == SNT_Object)
        {
            const ObjectScriptNode * src = reinterpret_cast<const ObjectScriptNode *>(source);

            // Overlay the environment of one on top the other first
            map<String, String>::type::const_iterator j, jend = src->mMapValue.end();
            for(j = src->mMapValue.begin(); j != jend; ++j)
            {
                String re;
                bool var = dest->getVariable(j->first, re);
                if(!var)
                    dest->setVariable(j->first, j->second);
            }

            // Create a vector storing each pairing of override between source and destination
            vector<std::pair<ScriptNode *, ScriptNodeList::iterator> >::type overrides;
            // A list of indices for each destination node tracks the minimum
            // source node they can index-match against
            map<ObjectScriptNode *, NCount>::type indices;
            // A map storing which nodes have overridden from the destination node
            map<ObjectScriptNode *, bool>::type overridden;

            ScriptNodeList::const_iterator z, zend = src->mChildren.end();
            for(z = src->mChildren.begin(); z != zend; ++z)
            {
                if((*z)->mType == SNT_Object)
                {
                    overrides.push_back(Npair(*z, dest->mChildren.end()));
                }
                else
                {
                    ScriptNode * newNode = (*z)->clone();
                    newNode->mParent = dest;
                    dest->mInheritValue.push_back(newNode);
                }
            }

            // Track the running maximum override index in the name-matching phase
            NCount maxOverrideIndex = 0;
            // Loop through destination children searching for name-matching overrides
            for(ScriptNodeList::iterator i = dest->mChildren.begin(); i != dest->mChildren.end(); )
            {
                if((*i)->mType == SNT_Object)
                {
                    ObjectScriptNode * node = reinterpret_cast<ObjectScriptNode *>(*i);
                    indices[node] = maxOverrideIndex;
                    overridden[node] = false;

                    // special treatment for materials with * in their name
                    bool nodeHasWildcard = node->mName.find('*') != String::npos;
                    // Find the matching name node
                    for(NCount j = 0; j < overrides.size(); ++j)
                    {
                        ObjectScriptNode * inherit = reinterpret_cast<ObjectScriptNode *>(overrides[j].first);
                        // Consider a match a node that has a wildcard and matches an input name
                        bool wildcardMatch = nodeHasWildcard && (StrUtil::match(inherit->mName, node->mName, false) ||
                            (node->mName.size() == 1 && inherit->mName.empty()));
                        if(inherit->mClass == node->mClass && !node->mName.empty() && (inherit->mName == node->mName || wildcardMatch))
                        {
                            // Pair these two together unless it's already paired
                            if(overrides[j].second == dest->mChildren.end())
                            {
                                ScriptNodeList::iterator currentIterator = i;
                                ObjectScriptNode * currentNode = node;
                                if(wildcardMatch)
                                {
                                    //If wildcard is matched, make a copy of current material and put it before the iterator,
                                    //matching its name to the parent. Use same reinterpret cast as above when node is set
                                    ScriptNode * newNode = (*i)->clone();
                                    currentIterator = dest->mChildren.insert(currentIterator, newNode);
                                    currentNode = reinterpret_cast<ObjectScriptNode *>(*currentIterator);
                                    currentNode->mName = inherit->mName;//make the regex match its matcher
                                }
                                overrides[j] = Npair(overrides[j].first, currentIterator);
                                // Store the max override index for this matched pair
                                indices[currentNode] = maxOverrideIndex = std::max(j, maxOverrideIndex);;
                                overridden[currentNode] = true;
                            }
                            else
                            {
                                error(*node, ScriptError::ET_UniqueError);
                            }

                            if(!wildcardMatch)
                                break;
                        }
                    }

                    if(nodeHasWildcard)
                    {
                        //if the node has a wildcard it will be deleted since it was duplicated for every match
                        ScriptNodeList::iterator deletable = i++;
                        N_delete *i;
                        dest->mChildren.erase(deletable);
                    }
                    else
                    {
                        ++i; //Behavior in absence of regex, just increment iterator
                    }
                }
                else
                {
                    ++i; //Behavior in absence of replaceable object, just increment iterator to find another
                }
            }

            // Now make matches based on index
            // Loop through destination children searching for name-matching overrides
            for(ScriptNodeList::iterator i = dest->mChildren.begin(); i != dest->mChildren.end(); ++i)
            {
                if((*i)->mType == SNT_Object)
                {
                    ObjectScriptNode * node = reinterpret_cast<ObjectScriptNode *>(*i);
                    if(!overridden[node])
                    {
                        // Retrieve the minimum override index from the map
                        // Search for minimum matching override
                        for(NCount j = indices[node]; j < overrides.size(); ++j)
                        {
                            ObjectScriptNode * inherit = reinterpret_cast<ObjectScriptNode *>(overrides[j].first);
                            if(inherit->mName.empty() && inherit->mClass == node->mClass && overrides[j].second == dest->mChildren.end())
                            {
                                overrides[j] = Npair(overrides[j].first, i);
                                break;
                            }
                        }
                    }
                }
            }

            ScriptNodeList::iterator insertPos = dest->mChildren.begin();
            for(NCount i = 0; i < overrides.size(); ++i)
            {
                if(overrides[i].second != dest->mChildren.end())
                {
                    // Override the destination with the source (base) object
                    inheritProperty(reinterpret_cast<ObjectScriptNode *>(*overrides[i].second), overrides[i].first);
                    insertPos = overrides[i].second;
                    insertPos++;
                }
                else
                {
                    // No override was possible, so insert this node at the insert position
                    // into the destination (child) object
                    ScriptNode * newNode = overrides[i].first->clone();
                    newNode->mParent = dest;
                    if(insertPos != dest->mChildren.end())
                    {
                        dest->mChildren.insert(insertPos, newNode);
                    }
                    else
                    {
                        dest->mChildren.push_back(newNode);
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ScriptCompiler::destroyExtern()
    {
        mExternList.clear();
        mExternTable.clear();

        ExternNodeList::iterator z, zend = mExternNodeList.end();
        for(z = mExternNodeList.begin(); z != zend; ++z)
        {
            ScriptLexerParser::destroy(z->second);
        }
        mExternNodeList.clear();
    }
    //-----------------------------------------------------------------------
    void ScriptCompiler::processRefValue(ScriptNodeList * nodes)
    {
        ScriptNodeList::iterator i = nodes->begin();
        while(i != nodes->end())
        {
            ScriptNodeList::iterator cur = i;
            ++i;

            if((*cur)->mType == SNT_Object)
            {
                // Only process if this object is not mBaseObj
                ObjectScriptNode * obj = static_cast<ObjectScriptNode *>(*cur);
                if(!obj->mBaseObj)
                {
                    processRefValue(&obj->mChildren);
                    processRefValue(&obj->mParamValue);
                }
            }
            else if((*cur)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = static_cast<PropertyScriptNode *>(*cur);
                processRefValue(&prop->mValueDataList);
            }
            else if((*cur)->mType == SNT_RefValue)
            {
                RefValueScriptNode * var = static_cast<RefValueScriptNode *>(*cur);

                // Look up the enclosing scope
                ObjectScriptNode * scope = 0;
                ScriptNode * temp = var->mParent;
                while(temp)
                {
                    if(temp->mType == SNT_Object)
                    {
                        scope = (ObjectScriptNode *)temp;
                        break;
                    }
                    temp = temp->mParent;
                }

                // Look up the variable in the environment
                String varAccess;
                bool accessOk;
                if(scope)
                    accessOk = scope->getVariable(var->mRefData, varAccess);
                if(!scope || !accessOk)
                {
                    map<String, String>::type::iterator k = mMapValue.find(var->mRefData);
                    accessOk = k != mMapValue.end();
                    if(accessOk)
                        varAccess = k->second;
                }

                if(accessOk)
                {
                    // Found the variable, so process it and insert it into the tree
                    ScriptLexerParser parser(this, var->mFile);

                    TokenNodeList * cst;
                    ScriptNodeList snlist;

                    parser.parseText(varAccess, cst);
                    parser.analyze(*cst, &snlist);

                    ScriptLexerParser::free(cst);

                    // Set up ownership for these nodes
                    for(ScriptNodeList::iterator j = snlist.begin(); j != snlist.end(); ++j)
                        (*j)->mParent = var->mParent;

                    // Recursively handle variable accesses within the variable expansion
                    processRefValue(&snlist);

                    // Insert the nodes in place of the variable
                    nodes->insert(cur, snlist.begin(), snlist.end());
                }
                else
                {
                    error(*var, ScriptError::ET_UnRefError);
                }

                // Remove the variable node
                N_delete *cur;
                nodes->erase(cur);
            }
        }
    }
    //--------------------------------------------------------------------
}
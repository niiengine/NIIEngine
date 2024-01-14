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
#include "NiiGpuProgramCodeGen.h"
#include "NiiDataStream.h"
#include "NiiException.h"

namespace NII
{
    const String ShaderFiles[] = { _T("VertexShader_vs"), _T("PixelShader_ps"), _T("GeometryShader_gs"), _T("HullShader_hs"), _T("DomainShader_ds") };
    //------------------------------------------------------------------------
    const String SegmentFilePatterns[] = { _T("segment_vs"), _T("segment_ps"), _T("segment_gs"), _T("segment_hs"), _T("segment_ds") };
    //------------------------------------------------------------------------
    //Must be sorted from best to worst
    const String BestD3DShaderTargets[ST_PCnt][5] =
    {
        {
            _T("vs_5_0"), _T("vs_4_1"), _T("vs_4_0"), _T("vs_4_0_level_9_3"), _T("vs_4_0_level_9_1")
        },
        {
            _T("ps_5_0"), _T("ps_4_1"), _T("ps_4_0"), _T("ps_4_0_level_9_3"), _T("ps_4_0_level_9_1")
        },
        {
            _T("gs_5_0"), _T("gs_4_1"), _T("gs_4_0"), _T("placeholder"), _T("placeholder")
        },
        {
            _T("hs_5_0"), _T("hs_4_1"), _T("hs_4_0"), _T("placeholder"), _T("placeholder")
        },
        {
            _T("ds_5_0"), _T("ds_4_1"), _T("ds_4_0"), _T("placeholder"), _T("placeholder")
        },
    };
    //------------------------------------------------------------------------
    int setOp(int op1, int op2) { return op2; }
    int addOp(int op1, int op2) { return op1 + op2; }
    int subOp(int op1, int op2) { return op1 - op2; }
    int mulOp(int op1, int op2) { return op1 * op2; }
    int divOp(int op1, int op2) { return op1 / op2; }
    int modOp(int op1, int op2) { return op1 % op2; }
    int minOp(int op1, int op2) { return std::min( op1, op2 ); }
    int maxOp(int op1, int op2) { return std::max( op1, op2 ); }
    //-----------------------------------------------------------------------------------
    struct Operation
    {
        const Ntchar * mToken;
        NCount mSize;
        int (*mFunc)(int, int);
        Operation(const Ntchar *_name, NCount len, int (*func)(int, int)) :
            mToken(_name), mSize(len), mFunc(func) {}
    };
    //-----------------------------------------------------------------------------------
    const Operation c_operations[8] =
    {
        Operation( _T("pset"), sizeof("@pset" ), &setOp ),
        Operation( _T("padd"), sizeof("@padd" ), &addOp ),
        Operation( _T("psub"), sizeof("@psub" ), &subOp ),
        Operation( _T("pmul"), sizeof("@pmul" ), &mulOp ),
        Operation( _T("pdiv"), sizeof("@pdiv" ), &divOp ),
        Operation( _T("pmod"), sizeof("@pmod" ), &modOp ),
        Operation( _T("pmin"), sizeof("@pmin" ), &minOp ),
        Operation( _T("pmax"), sizeof("@pmax" ), &maxOp )
    };
    #define poperationsize 8
    //-----------------------------------------------------------------------------------
    const Operation c_counterOperations[10] =
    {
        Operation( _T("counter"), sizeof("@counter" ), 0 ),
        Operation( _T("value"), sizeof("@value" ),     0 ),
        Operation( _T("set"), sizeof("@set" ), &setOp ),
        Operation( _T("add"), sizeof("@add" ), &addOp ),
        Operation( _T("sub"), sizeof("@sub" ), &subOp ),
        Operation( _T("mul"), sizeof("@mul" ), &mulOp ),
        Operation( _T("div"), sizeof("@div" ), &divOp ),
        Operation( _T("mod"), sizeof("@mod" ), &modOp ),
        Operation( _T("min"), sizeof("@min" ), &minOp ),
        Operation( _T("max"), sizeof("@max" ), &maxOp )
    };
    //-----------------------------------------------------------------------------------
    void copy(ExpressionAnaly * in, String & out, NCount length)
    {
        out.append(in->begin(), in->begin() + length);
    }
    //-----------------------------------------------------------------------------------
    void repeat(ExpressionAnaly * in, String & out, NCount length, NCount passNum, const String & counterVar)
    {
        String::const_iterator itor = in->begin();
        String::const_iterator end  = in->begin() + length;

        while(itor != end)
        {
            if(*itor == _T('@') && !counterVar.empty())
            {
                ExpressionAnaly subStr(in->getData(), in->getRef(), itor + 1);
                if(subStr.startWith(counterVar))
                {
                    Ntchar tmp[16];
                    Nstprintf(tmp, _T("%lu"), passNum);
                    out += tmp;
                    itor += counterVar.size() + 1;
                }
                else
                {
                    out.push_back(*itor++);
                }
            }
            else
            {
               out.push_back(*itor++);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void hashFileConcatenate(DataStream * in, vectorL<uint8> & data)
    {
        Nui64 re[2];
        memcpy( data.begin(), re, sizeof(Nui64) * 2u );
        const size_t fileSize = in->size();
        data.resize( fileSize + sizeof(Nui64) * 2u );

        in->read( data.begin() + sizeof(Nui64) * 2u, fileSize );
        MurmurHash_128( data.begin(), data.size(), re, re );
        memcpy( data.begin(), re, sizeof(Nui64) * 2u );
    }
    //-----------------------------------------------------------------------------------
    void hashSegmentFiles(VFS * vfs, const String & fileExt, const StringList & segments, vectorL<uint8> & data) const
    {
        StringList::const_iterator itor = segments.begin();
        StringList::const_iterator end  = segments.end();

        while(itor != end)
        {
            //Only open segment files with current render system extension
            const String::size_type extPos0 = itor->find(fileExt);
            const String::size_type extPos1 = itor->find(_T(".any"));
            if(extPos0 == itor->size() - fileExt.size() || extPos1 == itor->size() - 4u)
            {
                ScopeDataStream in(vfs->open(*itor));
                hashFileConcatenate(in, data);
            }

            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    int interpretAsNumberThenAsProperty(const PropertyData * data, const String & argValue) const
    {
        int opValue; 
        StrConv::conv(argValue, opValue, -std::numeric_limits<int>::max());
        if(opValue == -std::numeric_limits<int>::max())
        {
            //Not a number, interpret as property
            opValue = data->get(argValue);
        }

        return opValue;
    }
    //-----------------------------------------------------------------------------------
    int32 evaluateExpressionRecursive(const PropertyData * data, ExpressionList & expression, bool & sError) const
    {
        bool syntaxError = sError;
        bool lastExpWasOperator = true;
        ExpressionList::iterator itor = expression.begin();
        ExpressionList::iterator end  = expression.end();

        while( itor != end )
        {
            Expression & exp = *itor;

            if( exp.mValue == _T("&&") )
                exp.mType = ET_AND;
            else if( exp.mValue == _T("||") )
                exp.mType = ET_OR;
            else if( exp.mValue == _T("<") )
                exp.mType = ET_LESS;
            else if( exp.mValue == _T("<=") )
                exp.mType = ET_LESSEQUAL;
            else if( exp.mValue == _T("==") )
                exp.mType = ET_EQUAL;
            else if( exp.mValue == _T("!=") )
                exp.mType = ET_NEQUAL;
            else if( exp.mValue == _T(">") )
                exp.mType = ET_GREATER;
            else if( exp.mValue == _T(">=") )
                exp.mType = ET_GREATEREQUAL;
            else if( !exp.mChildren.empty() )
                exp.mType = ET_OBJECT;
            else
                exp.mType = ET_VAR;

            if((exp.isOperator() && lastExpWasOperator) || (!exp.isOperator() && !lastExpWasOperator))
            {
                syntaxError = true;
                Nprintf(_T("Unrecognized token '%s'"), exp.mValue.c_str() );
            }
            else
            {
                lastExpWasOperator = exp.isOperator();
            }

            ++itor;
        }

        //If we don't check 'expression.size() > 3u' here, we can end up in infinite recursion
        //later on (because operators get turned into ET_OBJECT and thus the object
        //is evaluated recusrively, and turned again into ET_OBJECT)
        if( !syntaxError && expression.size() > 3u )
        {
            //We will now enclose "a < b" into "(a < b)" other wise statements like these:
            //a && b < c will be parsed as (a && b) < c which is completely counterintuitive.

            //We need expression.size() > 3 which is guaranteed because if back nor front
            //are neither operators and we can't have two operators in a row, then they can
            //only be in the middle, or there is no operator at all.
            itor = expression.begin() + 1;
            end  = expression.end();
            while( itor != end )
            {
                if( itor->mType >= ET_LESS && itor->mType <= ET_GREATEREQUAL )
                {
                    //We need to merge n-1, n, n+1 into:
                    // (n-1)' = ET_OBJECT with 3 mChildren:
                    //      n-1, n, n+1
                    //and then remove both n & n+1.
                    itor->mChildren.resize( 3 );

                    itor->mChildren[1].mType = itor->mType;
                    itor->mChildren[1].mValue.swap( itor->mValue );
                    itor->mChildren[0].swap( *(itor - 1) );
                    itor->mChildren[2].swap( *(itor + 1) );

                    itor->mType = ET_OBJECT;

                    (itor - 1)->swap( *itor );

                    itor = expression.erase( itor, itor + 2 );
                    end  = expression.end();
                }
                else
                {
                    ++itor;
                }
            }
        }

        //Evaluate the individual properties.
        itor = expression.begin();
        while( itor != end && !syntaxError )
        {
            Expression & exp = *itor;
            if( exp.mType == ET_VAR )
            {
                Ntchar * endPtr;
                exp.mResult = Nstol( exp.mValue.c_str(), &endPtr, 10 );
                if( exp.mValue.c_str() == endPtr )
                {
                    //This isn't a number. Let's try if it's a variable
                    data->get( exp.mValue, exp.mResult);
                }
                lastExpWasOperator = false;
            }
            else
            {
                exp.mResult = evaluateExpressionRecursive(data, exp.mChildren, syntaxError );
                lastExpWasOperator = false;
            }

            ++itor;
        }

        //Perform operations between the different properties.
        int32 retVal = 1;
        if( !syntaxError )
        {
            itor = expression.begin();

            Expression::Type nextOperation = ET_VAR;

            while( itor != end )
            {
                int32 result = itor->mNegated ? !itor->mResult : itor->mResult;

                switch( nextOperation )
                {
                case ET_OR:
                    retVal = (retVal != 0) | (result != 0); break;
                case ET_AND:
                    retVal = (retVal != 0) & (result != 0); break;
                case ET_LESS:
                    retVal =  retVal <  result; break;
                case ET_LESSEQUAL:
                    retVal =  retVal <= result; break;
                case ET_EQUAL:
                    retVal =  retVal == result; break;
                case ET_NEQUAL:
                    retVal =  retVal != result; break;
                case ET_GREATER:
                    retVal =  retVal >  result; break;
                case ET_GREATEREQUAL:
                    retVal =  retVal >= result; break;
                case ET_OBJECT:
                case ET_VAR:
                    if( !itor->isOperator() )
                        retVal = result;
                    break;
                }

                nextOperation = itor->mType;

                ++itor;
            }
        }

        sError = syntaxError;

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    bool lisSegmentFiles(VFS * vfs, StringGroupList & segments)
    {
        bool retVal = false;
        StringList stringVectorPtr;
        vfs->list(stringVectorPtr, false, false);

        StringList slLowerCase(*stringVectorPtr);

        {
            StringList::iterator itor = slLowerCase.begin();
            StringList::iterator end  = slLowerCase.end();
            while( itor != end )
            {
                std::transform(itor->begin(), itor->end(), itor->begin(), ::tolower);
                ++itor;
            }
        }

        for(size_t i = 0; i < ST_PCnt; ++i)
        {
            StringList::const_iterator itLowerCase = slLowerCase.begin();
            StringList::const_iterator itor = stringVectorPtr->begin();
            StringList::const_iterator end  = stringVectorPtr->end();

            while( itor != end )
            {
                if( itLowerCase->find(SegmentFilePatterns[i] ) != String::npos ||
                    itLowerCase->find(_T("segment_all") ) != String::npos)
                {
                    retVal = true;
                    segments[i].push_back(*itor);
                }

                ++itLowerCase;
                ++itor;
            }

            //Enumeration order depends on OS and filesystem. Ensure deterministic alphabetical order.
            std::sort(segments[i].begin(), segments[i].end());
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    ExpressionAnaly & ExpressionAnaly::operator = (const ExpressionAnaly & o)
    {
        mData = o.mData;
        mRef = o.mRef;
        mStart = o.mStart;
        mEnd = o.mEnd;
    }
    //-----------------------------------------------------------------------------------
    bool ExpressionAnaly::checkBegin(bool & sError) const
    {
        size_t expEnd = checkEnd();

        if( expEnd == String::npos )
        {
            sError = true;
            return false;
        }

        ExpressionAnaly subStr(mData, getRef(), getStart(), getStart() + expEnd);

        setStart(getStart() + expEnd + 1);
        setEnd(mRef()->size());

        bool textStarted = false;
        bool syntaxError = false;
        bool nextExpressionNegates = false;

        std::vector<Expression *> expressionParents;
        ExpressionList outExpressions;
        outExpressions.clear();
        outExpressions.resize(1);

        Expression * currentExpression = &outExpressions.back();

        String::const_iterator it = subStr.begin();
        String::const_iterator en = subStr.end();

        while(it != en && !syntaxError)
        {
            char c = *it;

            if(c == _T('('))
            {
                currentExpression->mChildren.push_back(Expression());
                expressionParents.push_back(currentExpression);

                currentExpression->mChildren.back().mNegated = nextExpressionNegates;

                textStarted = false;
                nextExpressionNegates = false;

                currentExpression = &currentExpression->mChildren.back();
            }
            else if(c == _T(')'))
            {
                if( expressionParents.empty() )
                    syntaxError = true;
                else
                {
                    currentExpression = expressionParents.back();
                    expressionParents.pop_back();
                }

                textStarted = false;
            }
            else if( c == _T(' ') || c == _T('\t') || c == _T('\n') || c == _T('\r') )
            {
                textStarted = false;
            }
            else if( c == _T('!') &&
                     //Avoid treating "!=" as a negation of variable.
                     ( (it + 1) == en || *(it + 1) != _T('=') ) )
            {
                nextExpressionNegates = true;
            }
            else
            {
                if( !textStarted )
                {
                    textStarted = true;
                    currentExpression->mChildren.push_back( Expression() );
                    currentExpression->mChildren.back().mNegated = nextExpressionNegates;
                }

                if( c == _T('&') || c == _T('|') || c == _T('=') || c == _T('<') || c == _T('>') || c == _T('!') /* can only mean "!=" */ )
                {
                    if( currentExpression->mChildren.empty() || nextExpressionNegates )
                    {
                        syntaxError = true;
                    }
                    else if( !currentExpression->mChildren.back().mValue.empty() &&
                        c != *(currentExpression->mChildren.back().mValue.end()-1) &&
                            c != _T('=') )
                    {
                        currentExpression->mChildren.push_back(Expression());
                    }
                }

                currentExpression->mChildren.back().mValue.push_back(c);
                nextExpressionNegates = false;
            }

            ++it;
        }

        bool retVal = false;

        if(!expressionParents.empty())
            syntaxError = true;

        if(!syntaxError)
            retVal = evaluateExpressionRecursive(mData, outExpressions, syntaxError) != 0;

        if(syntaxError)
            Nprintf(_T("Syntax Error at line %lu\n"), subStr.getLineCount());

        sError = syntaxError;

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    bool ExpressionAnaly::checkArgs(StringList & outArgs)
    {
        size_t expEnd = checkEnd();

        if(expEnd == String::npos)
        {
            return true;
        }

        ExpressionAnaly subStr(mData, getRef(), getStart(), getStart() + expEnd);
        setStart(getStart() + expEnd + 1);
        setEnd(getRef()->size());
        
        int expressionState = 0;
        bool syntaxError = false;

        outArgs.clear();
        outArgs.push_back(String());

        String::const_iterator it = subStr.begin();
        String::const_iterator en = subStr.end();

        while(it != en && !syntaxError)
        {
            char c = *it;

            if(c == _T('(') || c == _T(')') || c == _T('@') || c == _T('&') || c == _T('|'))
            {
                syntaxError = true;
            }
            else if(c == _T(' ') || c == _T('\t') || c == _T('\n') || c == _T('\r'))
            {
                if(expressionState == 1)
                    expressionState = 2;
            }
            else if(c == _T(','))
            {
                expressionState = 0;
                outArgs.push_back(String());
            }
            else
            {
                if(expressionState == 2)
                {
                    Nprintf(_T("Syntax Error at line %lu: ',' or ')' expected\n"), subStr.getLineCount());
                    syntaxError = true;
                }
                else
                {
                    outArgs.back().push_back(*it);
                    expressionState = 1;
                }
            }

            ++it;
        }

        if(syntaxError)
            Nprintf(_T("Syntax Error at line %lu\n"), subStr.getLineCount());

        return syntaxError;
    }
    //-----------------------------------------------------------------------------------
    NCount ExpressionAnaly::getLineCount(const String & buffer, NCount idx)
    {
        String::const_iterator itor = buffer.begin();
        String::const_iterator end  = buffer.begin() + idx;

        NCount lineCount = 0;

        while(itor != end)
        {
            if(*itor == _T('\n'))
                ++lineCount;
            ++itor;
        }

        return lineCount + 1;
    }
    //-----------------------------------------------------------------------------------
    NCount ExpressionAnaly::getLineCount()
    {
        return getLineCount(*getRef(), getStart());
    }
    //-----------------------------------------------------------------------------------
    NCount ExpressionAnaly::checkEnd()
    {
        String::const_iterator it = begin();
        String::const_iterator en = end();

        int nesting = 0;

        while( it != en && nesting >= 0 )
        {
            if( *it == _T('(') )
                ++nesting;
            else if( *it == _T(')') )
                --nesting;
            ++it;
        }

        assert( nesting >= -1 );

        size_t re = String::npos;
        if( it != en && nesting < 0 )
        {
            re = it - begin() - 1;
        }
        else
        {
            Nprintf(_T("Syntax Error at line %lu: opening parenthesis without matching closure\n"), getLineCount());
        }

        return re;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GpuProgramCodeGen
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    void GpuProgramCodeGen::Expression::swap(Expression & o)
    {
        std::swap(mResult, o.mResult);
        std::swap(mNegated, o.mNegated);
        std::swap(mType, o.mType);
        mChildren.swap(o.mChildren);
        mValue.swap(o.mValue);
    }
    //------------------------------------------------------------------------
    GpuProgramCodeGen::GpuProgramCodeGen(Nid codeid):
        CodeGen(codeid, T_Framework),
        mVFS(0),
        mRenderSys(0),
        mShaderProfile(_T("unset!")),/// "glsl", "glsles", "hlsl"
        //mShaderSyntax(_T("unset!")),
        mShaderFileExt(_T("unset!")),/// Either glsl or hlsl
        //mFastShaderBuildHack(false),
        mQualityType(false),
        mAutoDestroy(true),
    #if N_Debug
        mDebug(true),
        mDebugProperty(true),
    #else
        mDebug(false),
        mDebugProperty(false),
    #endif
    {
        mSegment.reize(ST_PCnt);
        mShaderTargets.resize(ST_PCnt);///[0] = "vs_4_0", etc. Only used by D3D
#if N_PLAT == N_PLAT_IOS
        mOutputPath = macCachePath() + _T('/');
#endif
    }
    //------------------------------------------------------------------------
    GpuProgramCodeGen::GpuProgramCodeGen(const String * name, Nid codeid):
        CodeGen(name, codeid, T_Framework),
        mVFS(0),
        mRenderSys(0),
        mShaderProfile(_T("unset!")),/// "glsl", "glsles", "hlsl"
        //mShaderSyntax(_T("unset!")),
        mShaderFileExt(_T("unset!")),/// Either glsl or hlsl
        //mFastShaderBuildHack(false),
        mQualityType(false),
        mAutoDestroy(true),
    #if N_Debug
        mDebug(true),
        mDebugProperty(true),
    #else
        mDebug(false),
        mDebugProperty(false),
    #endif
    {
        mSegment.reize(ST_PCnt);
        mShaderTargets.resize(ST_PCnt);///[0] = "vs_4_0", etc. Only used by D3D
#if N_PLAT == N_PLAT_IOS
        mOutputPath = macCachePath() + _T('/');
#endif
    }
    //------------------------------------------------------------------------
    GpuProgramCodeGen::~GpuProgramCodeGen()
    {
    }
    //------------------------------------------------------------------------
    void GpuProgramCodeGen::setQuality(QualityType set)
    {
        mQualityType = set;
    }
    //-----------------------------------------------------------------------------------
    void GpuProgramCodeGen::getHash(Nui32 & hash[4]) const
    {
        vectorL<uint8> data;
        data.resize(sizeof(Nui64) * 2u, 0);
        for(size_t i = 0; i < ST_PCnt; ++i)
        {
            const String filename = ShaderFiles[i] + mShaderFileExt;
            if(mVFS->exists(filename))
            {
                //Library segment files first
                LibraryList::const_iterator itor = mLibrary.begin();
                LibraryList::const_iterator end  = mLibrary.end();

                while( itor != end )
                {
                    hashSegmentFiles(itor->mVFS, mShaderFileExt, itor->mSegment[i], data);
                    ++itor;
                }

                //Main segment files
                hashSegmentFiles(mVFS, mShaderFileExt, mSegment[i], data);

                //The shader file
                ScopeDataStream in(mVFS->open(filename));
                hashFileConcatenate(in, data);
            }
        }

        memcpy(hash, data.begin(), sizeof(Nui32) * 4u);
    }
    //------------------------------------------------------------------------
    void GpuProgramCodeGen::setDebug(bool enable, bool property, const String & path)
    {
        mDebug = enable;
        mDebugProperty = property;
        mOutputPath = path;
    }
    //------------------------------------------------------------------------
    void GpuProgramCodeGen::debugOutput(Nofstream & outFile )
    {
        outFile.write(_T("#if 0"), sizeof( "#if 0" ) - 1u );

        Ntchar tmpBuffer[64];
        Ntchar friendlyText[32];
        LwString value( LwString::FromEmptyPointer( tmpBuffer, 64 ) );

        {
            PropertyValueList::const_iterator itor = mPropertys.begin();
            PropertyValueList::const_iterator end  = mPropertys.end();

            while( itor != end )
            {
                itor->keyName.getFriendlyText( friendlyText, 32 );
                value.clear();
                value.a( itor->mValue );

                outFile.write( _T("\n\t***\t"), sizeof( "\n\t***\t" ) - 1u );
                outFile.write( friendlyText, Nsnlen( friendlyText, 32 ) );
                outFile.write( _T("\t"), sizeof( "\t" ) - 1u );
                outFile.write( value.c_str(), value.size() );
                ++itor;
            }
        }

        outFile.write( _T("\n\tDONE DUMPING PROPERTIES"), sizeof( "\n\tDONE DUMPING PROPERTIES" ) - 1u );

        {
            SegmentList::const_iterator itor = mSegmentList.begin();
            SegmentList::const_iterator end  = mSegmentList.end();

            while( itor != end )
            {
                itor->first.getFriendlyText( friendlyText, 32 );
                outFile.write( _T("\n\t***\t"), sizeof( "\n\t***\t" ) - 1u );
                outFile.write( friendlyText, Nsnlen( friendlyText, 32 ) );
                outFile.write( _T("\t"), sizeof( "\t" ) - 1u );
                outFile.write( itor->second.c_str(), itor->second.size() );
                ++itor;
            }
        }

        outFile.write( _T("\n\tDONE DUMPING SEGMENTS\n#endif\n"), sizeof( "\n\tDONE DUMPING SEGMENTS\n#endif\n" ) - 1u );
    }
    //-----------------------------------------------------------------------------------
    bool GpuProgramCodeGen::genMath(const String & inBuffer, String & outBuffer)
    {
        outBuffer.clear();
        outBuffer.reserve(inBuffer.size());

        StringList argValues;
        ExpressionAnaly subStr(this, &inBuffer, 0);

        size_t keyword, pos;

        bool syntaxError = false;

        while(!syntaxError)
        {
            pos = subStr.find(_T("@"));
            keyword = ~0;

            while(pos != String::npos && keyword == (size_t)~0)
            {
                size_t maxSize = subStr.findFirstOf( _T(" \t("), pos + 1 );
                maxSize = maxSize == String::npos ? subStr.getSize() : maxSize;
                ExpressionAnaly keywordStr(this, &inBuffer, subStr.getStart() + pos + 1, subStr.getStart() + maxSize);

                for( size_t i = 0; i < poperationsize && keyword == (size_t)~0; ++i )
                {
                    if( keywordStr.matchEqual( c_operations[i].mToken ) )
                        keyword = i;
                }

                if( keyword == (size_t)~0 )
                    pos = subStr.find( _T("@"), pos + 1 );
            }
            if(keyword == (size_t)~0)
                break;

            //Copy what comes before the block
            copy(&subStr, outBuffer, pos );

            subStr.setStart( subStr.getStart() + pos + c_operations[keyword].mSize );
            syntaxError |= subStr.checkArgs(argValues);

            syntaxError |= argValues.size() < 2 || argValues.size() > 3;

            if( !syntaxError )
            {
                const IdString dstProperty = argValues[0];
                const size_t idx = argValues.size() == 3 ? 1 : 0;
                const int op1Value = interpretAsNumberThenAsProperty(this, argValues[idx]);
                const int op2Value = interpretAsNumberThenAsProperty(this, argValues[idx + 1]);

                int result = c_operations[keyword].mFunc( op1Value, op2Value );
                add( dstProperty, result );
            }
            else
            {
                size_t lineCount = subStr.getLineCount();
                if( keyword <= 1 )
                {
                    Nprintf(_T("Syntax Error at line %lu: @%s expects one parameter\n"), lineCount, c_operations[keyword].mToken );
                }
                else
                {
                    Nprintf(_T("Syntax Error at line %lu: @%s expects two or three parameters\n"), lineCount, c_operations[keyword].mToken );
                }
            }
        }

        copy(&subStr, outBuffer, subStr.getSize() );

        return syntaxError;
    }
    //-----------------------------------------------------------------------------------
    bool GpuProgramCodeGen::genFor(const String & inBuffer, String & outBuffer) const
    {
        outBuffer.clear();
        outBuffer.reserve( inBuffer.size() );

        StringList argValues;
        ExpressionAnaly subStr(this, &inBuffer, 0);
        size_t pos = subStr.find(_T("@foreach"));

        bool syntaxError = false;

        while( pos != String::npos && !syntaxError )
        {
            //Copy what comes before the block
            copy(&subStr, outBuffer, pos);

            subStr.setStart( subStr.getStart() + pos + sizeof( "@foreach" ) );
            syntaxError |= subStr.checkArgs(argValues);

            ExpressionAnaly blockSubString = subStr;
            blockSubString.checkBlockEnd(syntaxError);

            if( !syntaxError )
            {
                Ntchar *endPtr;
                int count = Nstol( argValues[0].c_str(), &endPtr, 10 );
                if( argValues[0].c_str() == endPtr )
                {
                    //This isn't a number. Let's try if it's a variable
                    //count = get( argValues[0], -1 );
                    count = get( argValues[0], 0 );
                }

                /*if( count < 0 )
                {
                    Nprintf(_T("Invalid parameter at line %lu (@foreach). '%s' is not a number nor a variable\n"),
                            blockSubString.getLineCount(), argValues[0].c_str() );
                    syntaxError = true;
                    count = 0;
                }*/

                String counterVar;
                if( argValues.size() > 1 )
                    counterVar = argValues[1];

                int start = 0;
                if( argValues.size() > 2 )
                {
                    start = Nstol( argValues[2].c_str(), &endPtr, 10 );
                    if( argValues[2].c_str() == endPtr )
                    {
                        //This isn't a number. Let's try if it's a variable
                        start = get( argValues[2], -1 );
                    }

                    if( start < 0 )
                    {
                        Nprintf(_T("Invalid parameter at line %lu (@foreach). '%s' is not a number nor a variable\n"),
                            blockSubString.getLineCount(), argValues[2].c_str() );
                        syntaxError = true;
                        start = 0;
                        count = 0;
                    }
                }

                for( int i = start; i < count; ++i )
                    repeat(&blockSubString, outBuffer, blockSubString.getSize(), i, counterVar );

            }

            subStr.setStart( blockSubString.getEnd() + sizeof( "@end" ) );
            pos = subStr.find(_T("@foreach"));
        }

        copy(&subStr, outBuffer, subStr.getSize());

        return syntaxError;
    }
    //-----------------------------------------------------------------------------------
    bool GpuProgramCodeGen::genProperty(String & inBuffer, String & outBuffer) const
    {
        outBuffer.clear();
        outBuffer.reserve( inBuffer.size() );

        ExpressionAnaly subStr(this, &inBuffer, 0 );
        size_t pos = subStr.find( _T("@property") );

        bool syntaxError = false;

        while( pos != String::npos && !syntaxError )
        {
            //Copy what comes before the block
            copy(&subStr, outBuffer, pos );

            subStr.setStart( subStr.getStart() + pos + sizeof("@property" ) );
            bool result = subStr.checkBegin(syntaxError );

            ExpressionAnaly blockSubString = subStr;
            bool isElse = blockSubString.checkBlockEnd(syntaxError, true );

            if( result && !syntaxError )
                copy(&blockSubString, outBuffer, blockSubString.getSize() );

            if( !isElse )
            {
                subStr.setStart( blockSubString.getEnd() + sizeof("@end") );
                pos = subStr.find( _T("@property") );
            }
            else
            {
                subStr.setStart( blockSubString.getEnd() + sizeof("@else") );
                blockSubString = subStr;
                blockSubString.checkBlockEnd(syntaxError );
                if( !syntaxError && !result )
                    copy(&blockSubString, outBuffer, blockSubString.getSize() );
                subStr.setStart( blockSubString.getEnd() + sizeof("@end") );
                pos = subStr.find( _T("@property") );
            }
        }

        copy(&subStr, outBuffer, subStr.getSize());

        while( !syntaxError && outBuffer.find( _T("@property") ) != String::npos )
        {
            inBuffer.swap( outBuffer );
            syntaxError = genProperty( inBuffer, outBuffer );
        }

        return syntaxError;
    }
    //-----------------------------------------------------------------------------------
    void GpuProgramCodeGen::genSegment(VFS * vfs, const StringList & segments)
    {
        StringList::const_iterator itor = segments.begin();
        StringList::const_iterator end  = segments.end();

        while(itor != end)
        {
            //Only open segment files with current render system extension
            const String::size_type extPos0 = itor->find( mShaderFileExt );
            const String::size_type extPos1 = itor->find( _T(".any") );
            if(extPos0 == itor->size() - mShaderFileExt.size() || extPos1 == itor->size() - 4u)
            {
                ScopeDataStream in(vfs->open(*itor));

                String inString;
                String outString;

                inString.resize(in->size());
                in->read(&inString[0], in->size());

                genMath(inString, outString);
                while( outString.find( _T("@foreach") ) != String::npos )
                {
                    genFor(outString, inString);
                    inString.swap( outString );
                }
                genProperty(outString, inString);
                genUndef(inString, outString);
                genDefine(outString, inString);
                genPropertyMath(inString, outString);
            }
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    bool GpuProgramCodeGen::genUndef( String & inBuffer, String & outBuffer )
    {
        outBuffer.clear();
        outBuffer.reserve( inBuffer.size() );

        StringList argValues;
        ExpressionAnaly subStr(this, &inBuffer, 0 );
        size_t pos = subStr.find( _T("@undefsegment") );

        bool syntaxError = false;

        while( pos != String::npos && !syntaxError )
        {
            //Copy what comes before the block
            copy(&subStr, outBuffer, pos);

            subStr.setStart( subStr.getStart() + pos + sizeof("@undefsegment") );
            syntaxError |= subStr.checkArgs(argValues);

            syntaxError |= argValues.size() != 1u;

            if( !syntaxError )
            {
                const IdString segmentName( argValues[0] );
                SegmentList::iterator it = mSegmentList.find( segmentName );
                if( it != mSegmentList.end() )
                    mSegmentList.erase( it );
            }
            else
            {
                Nprintf(_T("Syntax Error at line %lu: @undefsegment expects one parameter"), subStr.getLineCount() );
            }

            pos = subStr.find( _T("@undefsegment") );
        }

        copy(&subStr, outBuffer, subStr.getSize());

        return syntaxError;
    }
    //-----------------------------------------------------------------------------------
    bool GpuProgramCodeGen::genDefine( const String & inBuffer, String & outBuffer )
    {
        outBuffer.clear();
        outBuffer.reserve( inBuffer.size() );

        StringList argValues;
        ExpressionAnaly subStr(this, &inBuffer, 0 );
        size_t pos = subStr.find( _T("@segment") );

        bool syntaxError = false;

        while( pos != String::npos && !syntaxError )
        {
            //Copy what comes before the block
            copy(&subStr, outBuffer, pos);

            subStr.setStart( subStr.getStart() + pos + sizeof("@segment") );
            syntaxError |= subStr.checkArgs(argValues);

            syntaxError |= argValues.size() != 1;

            if( !syntaxError )
            {
                const IdString segmentName( argValues[0] );
                SegmentList::const_iterator it = mSegmentList.find( segmentName );
                if( it != mSegmentList.end() )
                {
                    syntaxError = true;
                    Nprintf(_T("Error at line %lu: @segment '%s' already defined"), subStr.getLineCount(), argValues[0].c_str() );
                }
                else
                {
                    ExpressionAnaly blockSubString = subStr;
                    blockSubString.checkBlockEnd(syntaxError );

                    String tmpBuffer;
                    copy(&blockSubString, tmpBuffer, blockSubString.getSize() );
                    mSegmentList[segmentName] = tmpBuffer;

                    subStr.setStart( blockSubString.getEnd() + sizeof("@end") );
                }
            }
            else
            {
                Nprintf(_T("Syntax Error at line %lu: @segment expects one parameter"), subStr.getLineCount() );
            }

            pos = subStr.find( _T("@segment") );
        }

        copy(&subStr, outBuffer, subStr.getSize() );

        return syntaxError;
    }
    //-----------------------------------------------------------------------------------
    bool GpuProgramCodeGen::genRef(String & inBuffer, String & outBuffer) const
    {
        outBuffer.clear();
        outBuffer.reserve( inBuffer.size() );

        StringList argValues;
        ExpressionAnaly subStr(this, &inBuffer, 0 );
        size_t pos = subStr.find( _T("@insertsegment") );

        bool syntaxError = false;

        while( pos != String::npos && !syntaxError )
        {
            //Copy what comes before the block
            copy(&subStr, outBuffer, pos);

            subStr.setStart(subStr.getStart() + pos + sizeof("@insertsegment"));
            syntaxError |= subStr.checkArgs(argValues);

            syntaxError |= argValues.size() != 1;

            if( !syntaxError )
            {
                const IdString segmentName( argValues[0] );
                SegmentList::const_iterator it = mSegmentList.find( segmentName );
                if( it != mSegmentList.end() )
                    outBuffer += it->second;
            }
            else
            {
                Nprintf(_T("Syntax Error at line %lu: @insertsegment expects one parameter"), subStr.getLineCount());
            }

            pos = subStr.find( _T("@insertsegment") );
        }

        copy(&subStr, outBuffer, subStr.getSize());

        return syntaxError;
    }
    //-----------------------------------------------------------------------------------
    bool GpuProgramCodeGen::genPropertyMath(const String & inBuffer, String & outBuffer)
    {
        outBuffer.clear();
        outBuffer.reserve(inBuffer.size());

        StringList argValues;
        ExpressionAnaly subStr(this, &inBuffer, 0);

        size_t pos, keyword;
        bool syntaxError = false;

        while(!syntaxError)
        {
            pos = subStr.find(_T("@"));
            keyword = ~0;

            if(pos != String::npos)
            {
                size_t maxSize = subStr.findFirstOf(_T(" \t("), pos + 1 );
                maxSize = maxSize == String::npos ? subStr.getSize() : maxSize;
                ExpressionAnaly keywordStr(this, &inBuffer, subStr.getStart() + pos + 1, subStr.getStart() + maxSize );

                for( size_t i = 0; i < 10 && keyword == (size_t)~0; ++i )
                {
                    if( keywordStr.matchEqual( c_counterOperations[i].mToken ) )
                        keyword = i;
                }

                if( keyword == (size_t)~0 )
                    break;
            }

            //Copy what comes before the block
            copy(&subStr, outBuffer, pos);

            subStr.setStart( subStr.getStart() + pos + c_counterOperations[keyword].mSize );
            syntaxError |= subStr.checkArgs(argValues);

            if( keyword <= 1 )
                syntaxError |= argValues.size() != 1;
            else
                syntaxError |= argValues.size() < 2 || argValues.size() > 3;

            if( syntaxError )
            {
                size_t lineCount = subStr.getLineCount();
                if( keyword <= 1 )
                {
                    Nprintf(_T("Syntax Error at line %lu: @%s expects one parameter\n"), lineCount, c_counterOperations[keyword].mToken );
                }
                else
                {
                    Nprintf(_T("Syntax Error at line %lu: @%s expects two or three parameters\n"), lineCount, c_counterOperations[keyword].mToken );
                }
            }
            else
            {
                if( argValues.size() == 1 )
                {
                    const IdString dstProperty = argValues[0];
                    const IdString srcProperty = dstProperty;
                    int op1Value = get( srcProperty );

                    //@value & @counter write, the others are invisible
                    Ntchar tmp[16];
                    Nstprintf( tmp, _T("%i"), op1Value );
                    outBuffer += tmp;

                    if( keyword == 0 )
                    {
                        ++op1Value;
                        add( dstProperty, op1Value );
                    }
                }
                else
                {
                    const IdString dstProperty = argValues[0];
                    const size_t idx = argValues.size() == 3 ? 1 : 0;
                    const int op1Value = interpretAsNumberThenAsProperty(this, argValues[idx]);
                    const int op2Value = interpretAsNumberThenAsProperty(this, argValues[idx + 1]);

                    int result = c_counterOperations[keyword].mFunc( op1Value, op2Value );
                    add( dstProperty, result );
                }
            }
        }

        copy(&subStr, outBuffer, subStr.getSize());

        return syntaxError;
    }
    //-----------------------------------------------------------------------------------
    /*bool GpuProgramCodeGen::parse(const String & inBuffer, String & outBuffer) const
    {
        outBuffer.clear();
        outBuffer.reserve( inBuffer.size() );

        return genFor( inBuffer, outBuffer );
        //return genProperty( inBuffer, outBuffer );
    }*/
    //-----------------------------------------------------------------------------------
    bool GpuProgramCodeGen::checkBlockEnd(bool & syntaxError, bool allowsElse)
    {
        bool isElse = false;

        const Ntchar * blockNames[] =
        {
            _T("foreach"),
            _T("property"),
            _T("segment"),
            _T("else")
        };

        cbitset32<2048> allowedElses;
        if( allowsElse )
            allowedElses.set(0);

        String::const_iterator it = begin();
        String::const_iterator en = end();

        int nesting = 0;

        while( it != en && nesting >= 0 )
        {
            if( *it == _T('@') )
            {
                ExpressionAnaly subStr(this, getRef(), it + 1);

                bool start = subStr.startWith( _T("end") );
                if( start )
                {
                    --nesting;
                    it += sizeof( "end" ) - 1;
                    continue;
                }
                else
                {
                    if( allowsElse )
                        start = subStr.startWith( _T("else") );
                    if( start )
                    {
                        if( !allowedElses.test( static_cast<size_t>( nesting ) ) )
                        {
                            syntaxError = true;
                            Nprintf(_T("Unexpected @else while looking for @end\nNear: '%s'\n"), &(*subStr.begin()) );
                        }
                        if( nesting == 0 )
                        {
                            //Decrement nesting so that we're out and tell caller we went from
                            //@property() through @else. Caller will later have to go from
                            //@else to @end
                            isElse = true;
                            --nesting;
                        }
                        else
                        {
                            //Do not decrease 'nesting', as we now need to look for "@end" but
                            //unset allowedElses, so that we do not allow two consecutive @else
                            allowedElses.setValue( static_cast<size_t>( nesting ), 0u );
                        }
                        it += sizeof( "else" ) - 1;
                        continue;
                    }
                    else
                    {
                        for( size_t i=0; i<sizeof( blockNames ) / sizeof( Ntchar* ); ++i )
                        {
                            bool startBlock = subStr.startWith(blockNames[i]);
                            if( startBlock )
                            {
                                it = subStr.begin() + Nstrlen(blockNames[i]);
                                if( i == 3 )
                                {
                                    //Do not increase 'nesting' for "@else"
                                    if( !allowedElses.test( static_cast<size_t>( nesting ) ) )
                                    {
                                        syntaxError = true;
                                        Nprintf(_T("Unexpected @else while looking for @end\nNear: '%s'\n"), &(*subStr.begin()) );
                                    }
                                }
                                else
                                {
                                    ++nesting;
                                }
                                allowedElses.setValue( static_cast<size_t>( nesting ), i == 1u );
                                break;
                            }
                        }
                    }
                }
            }

            ++it;
        }

        assert( nesting >= -1 );

        if( it != en && nesting < 0 )
        {
            int keywordLength = (isElse ? sizeof( "else" ) : sizeof( "end" )) - 1;
            setEnd(it - getRef()->begin() - keywordLength);
        }
        else
        {
            syntaxError = true;

            Ntchar tmpData[64];
            memset(tmpData, 0, sizeof(tmpData));
            Nstrncpy(tmpData, &(*begin()), std::min<size_t>(63u, getSize()));

            Nprintf(_T("Syntax Error at line %lu: start block (e.g. @foreach; @property) without matching @end\nNear: '%s'\n"), getLineCount(), tmpData);
        }

        return isElse;
    }
    //------------------------------------------------------------------------
    void GpuProgramCodeGen::setup(const VFSList & vfsl)
    {
        VFSList::const_iterator itor = vfsl->begin();
        VFSList::const_iterator end  = vfsl->end();
        ++itor;
        if(itor != end)
        {
            mLibrary.clear();
            while( itor != end )
            {
                mLibrary.emplace_back(Library(*itor));
                Library & library = mLibrary.back();
                library.mSegment.reize(ST_PCnt);
                ++itor;
            }
        }
        else
        {
            LibraryList::iterator itor = mLibrary.begin();
            LibraryList::iterator end  = mLibrary.end();
            while( itor != end )
            {
                for(size_t i = 0; i < ST_PCnt; ++i)
                    itor->mSegment[i].clear();
                ++itor;
            }
        }

        for(size_t i = 0; i < ST_PCnt; ++i)
            mSegment[i].clear();

        mVFS = vfsl[0];

        if( !mVFS )
            return; //Some RenderPattern implementations may not use template files at all

        bool hasValidFile = false;

        //Check this folder can at least generate one valid mType of shader.
        for(size_t i = 0; i < ST_PCnt; ++i)
        {
             //Probe both types since this may be called before we know what RS to use.
            const String filename = ShaderFiles[i];
            hasValidFile |= mVFS->isExist( filename + _T(".glsl") );
            hasValidFile |= mVFS->isExist( filename + _T(".hlsl") );
            hasValidFile |= mVFS->isExist( filename + _T(".metal") );
            hasValidFile |= mVFS->isExist( filename + _T(".any") );
        }

        if( !hasValidFile )
        {
            N_EXCEPT( Exception::ERR_FILE_NOT_FOUND,
                         "Data folder provided contains no valid template shader files. "
                         "Did you provide the right folder location? Check you have the "
                         "right read pemissions. Folder: " + mVFS->getName());
        }

        lisSegmentFiles(mVFS, mSegment);

        LibraryList::iterator itor = mLibrary.begin();
        LibraryList::iterator end  = mLibrary.end();

        while(itor != end)
        {
            if( !lisSegmentFiles(itor->mVFS, itor->mSegment))
            {
                N_log("HLMS Library path '" + itor->mVFS->getName() +
                    "' has no segment files. Are you sure you provided ");
            }

            ++itor;
        }
    }
    //------------------------------------------------------------------------
    void GpuProgramCodeGen::addLib(VFS * lib)
    {
        mLibrary.push_back(Library(lib));
        Library & library = mLibrary.back();
        library.mSegment.reize(ST_PCnt);
        lisSegmentFiles(library.mVFS, library.mSegment);
    }
    //------------------------------------------------------------------------
    void GpuProgramCodeGen::removeLib(VFS * lib)
    {
        
    }
    //------------------------------------------------------------------------
    void GpuProgramCodeGen::setRender(RenderSys * rsys)
    {
        mRenderSys = rsys;

        mShaderProfile = _T("unset!");
        mShaderFileExt = _T("unset!");
        //mShaderSyntax  = "unset!";

        if( mRenderSys )
        {
            /*{
                mFastShaderBuildHack = false;
                const SysOptionList & rsConfigOptions = rsys->getConfig();
                SysOptionList::const_iterator itor = rsConfigOptions.find( "Fast Shader Build Hack" );
                if( itor != rsConfigOptions.end() )
                    StrConv::conv( itor->second.currentValue, mFastShaderBuildHack);
            }*/

            //Prefer glsl over glsles
            const String shaderProfiles[4] = { _T("hlsl"), _T("glsles"), _T("glsl"), _T("metal") };
            const RenderFeature * feature = mRenderSys->getFeature();

            for( size_t i = 0; i < 4; ++i )
            {
                if( feature->isShaderSupport( shaderProfiles[i] ) )
                {
                    mShaderProfile = shaderProfiles[i];
                    //mShaderSyntax = shaderProfiles[i];
                }
            }

            if( mShaderProfile == _T("hlsl") )
            {
                mShaderFileExt = _T(".hlsl");

                for( size_t i = 0; i < ST_PCnt; ++i )
                {
                    for( size_t j = 0; j < ST_PCnt && mShaderTargets[i].empty(); ++j )
                    {
                        if( feature->isShaderSupport( BestD3DShaderTargets[i][j] ) )
                            mShaderTargets[i] = BestD3DShaderTargets[i][j];
                    }
                }
            }
            else if( mShaderProfile == _T("metal") )
            {
                mShaderFileExt = _T(".metal");
            }
            else
            {
                mShaderFileExt = _T(".glsl");

                if( mRenderSys->checkGpuApi( "GL_AMD_shader_trinary_minmax" ) )
                    mFeatureList.push_back( N_PropertyRender::GlAmdTrinaryMinMax );

                struct Extensions
                {
                    const char * mName;
                    uint32 minGlVersion;
                };

                Extensions extensions[] =
                {
                    { "GL_ARB_base_instance",               420 },
                    { "GL_ARB_shading_language_420pack",    420 },
                    { "GL_ARB_texture_buffer_range",        430 },
                };

                for(size_t i = 0; i < sizeof(extensions) / sizeof(extensions[0]); ++i)
                {
                    if(mRenderSys->getShaderVersion() >= extensions[i].minGlVersion ||
                        mRenderSys->checkGpuApi(extensions[i].mName))
                    {
                        mFeatureList.push_back(extensions[i].mName);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
}
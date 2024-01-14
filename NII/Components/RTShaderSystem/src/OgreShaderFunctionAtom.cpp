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

#include "OgreShaderFunctionAtom.h"
#include "OgreRoot.h"

namespace Ogre {
namespace RTShader {
//-----------------------------------------------------------------------------
Operand::Operand(ParameterPtr parameter, Operand::OpSemantic opSemantic, int opMask, ushort indirectionLevel)
{
	mParameter = parameter;
	mSemantic = opSemantic;
	mMask = opMask;
	mIndirectionLevel = indirectionLevel;
}
//-----------------------------------------------------------------------------
Operand::Operand(const Operand& other) 
{
	*this = other;
}
//-----------------------------------------------------------------------------
Operand& Operand::operator= (const Operand & other)
{
	if (this != &other) 
	{
		mParameter = other.mParameter;
		mSemantic = other.mSemantic;
		mMask = other.mMask;
		mIndirectionLevel = other.mIndirectionLevel;
	}		
	return *this;
}
//-----------------------------------------------------------------------------
Operand::~Operand()
{
	// nothing todo
}
//-----------------------------------------------------------------------------
String Operand::getMaskAsString(int mask)
{
	String retVal = "";

	if (mask & ~OPM_ALL) 
	{
		if (mask & OPM_X)
		{
			retVal += "x";
		}

		if (mask & OPM_Y)
		{
			retVal += "y";
		}

		if (mask & OPM_Z)
		{
			retVal += "z";
		}

		if (mask & OPM_W)
		{
			retVal += "w";
		}
	}

	return retVal;
}

//-----------------------------------------------------------------------------
int Operand::getFloatCount(int mask)
{
	int floatCount = 0;

	while (mask != 0)
	{
		if ((mask & Operand::OPM_X) != 0)
		{
			floatCount++;

		}			
		mask = mask >> 1;
	}

	return floatCount;
}

//-----------------------------------------------------------------------------
GpuDataType	Operand::getGpuConstantType(int mask)
{
	int floatCount = getFloatCount(mask);
	GpuDataType type;

	switch (floatCount)
	{
	case 1:
		type = GDT_Float;
		break;
	case 2:
		type = GDT_Float2X;
		break;
	case 3:
		type = GDT_Float3X;
		break;
	case 4:
		type = GDT_Float4X;
		break;
	default:
		type = GDT_Unknow;
		break;
	}

	return type;
}

//-----------------------------------------------------------------------------
String Operand::toString() const
{
	String retVal = mParameter->toString();
	if ((mMask & OPM_ALL) || ((mMask & OPM_X) && (mMask & OPM_Y) && (mMask & OPM_Z) && (mMask & OPM_W)))
	{
		return retVal;
	}

	retVal += "." + getMaskAsString(mMask);

	return retVal;
}

//-----------------------------------------------------------------------------
FunctionAtom::FunctionAtom()
{
	mGroupExecutionOrder   = -1;
	mInternalExecutionOrder = -1;
}

//-----------------------------------------------------------------------------
int FunctionAtom::getGroupExecutionOrder() const
{
	return mGroupExecutionOrder;
}

//-----------------------------------------------------------------------------
int	FunctionAtom::getInternalExecutionOrder() const
{
	return mInternalExecutionOrder;
}


String FunctionInvocation::Type = "FunctionInvocation";

//-----------------------------------------------------------------------
FunctionInvocation::FunctionInvocation(const String& functionName, 
									   int groupOrder, int internalOrder, String returnType)
{
	mFunctionName = functionName;
	mGroupExecutionOrder = groupOrder;
	mInternalExecutionOrder = internalOrder;
	mReturnType = returnType;
}

//-----------------------------------------------------------------------------
FunctionInvocation::FunctionInvocation(const FunctionInvocation& other)
{
    
	mFunctionName = other.mFunctionName;
	mGroupExecutionOrder = other.mGroupExecutionOrder;
	mInternalExecutionOrder = other.mInternalExecutionOrder;
	mReturnType = other.mReturnType;
    
    for ( OperandVector::const_iterator it = other.mOperands.begin(); it != other.mOperands.end(); ++it)
        mOperands.push_back(Operand(*it));
}

//-----------------------------------------------------------------------
void FunctionInvocation::writeSourceCode(std::ostream& os, const String& targetLanguage) const
{
	// Write function name.
	os << mFunctionName << "(";

	// Write parameters.
	ushort curIndLevel = 0;
	for (OperandVector::const_iterator it = mOperands.begin(); it != mOperands.end(); )
	{
		os << (*it).toString();
		++it;

		ushort opIndLevel = 0;
		if (it != mOperands.end())
		{
			opIndLevel = (*it).getIndirectionLevel();
		}

		if (curIndLevel < opIndLevel)
		{
			while (curIndLevel < opIndLevel)
			{
				++curIndLevel;
				os << "[";
			}
		}
		else //if (curIndLevel >= opIndLevel)
		{
			while (curIndLevel > opIndLevel)
			{
				--curIndLevel;
				os << "]";
			}
			if (opIndLevel != 0)
			{
				os << "][";
			}
			else if (it != mOperands.end())
			{
				os << ", ";
			}
		}
	}

	// Write function call closer.
	os << ");";
}

//-----------------------------------------------------------------------
void FunctionInvocation::pushOperand(ParameterPtr parameter, Operand::OpSemantic opSemantic, int opMask, int indirectionLevel)
{
	mOperands.push_back(Operand(parameter, opSemantic, opMask, indirectionLevel));
}

//-----------------------------------------------------------------------
bool FunctionInvocation::operator == ( const FunctionInvocation& rhs ) const
{
    return FunctionInvocationCompare()(*this, rhs);
}

//-----------------------------------------------------------------------
bool FunctionInvocation::operator != ( const FunctionInvocation& rhs ) const
{
    return !(*this == rhs);
}

//-----------------------------------------------------------------------
bool FunctionInvocation::operator < ( const FunctionInvocation& rhs ) const
{
    return FunctionInvocationLessThan()(*this, rhs);
}

bool FunctionInvocation::FunctionInvocationLessThan::operator ()(FunctionInvocation const& lhs, FunctionInvocation const& rhs) const
{
    // Check the function names first
    // Adding an exception to std::string sorting.  I feel that functions beginning with an underscore should be placed before
    // functions beginning with an alphanumeric character.  By default strings are sorted based on the ASCII value of each character.
    // Underscores have an ASCII value in between capital and lowercase characters.  This is why the exception is needed.
    if (lhs.getFunctionName() < rhs.getFunctionName())
    {
        if(rhs.getFunctionName().at(0) == '_')
            return false;
        else
            return true;
    }
    if (lhs.getFunctionName() > rhs.getFunctionName())
    {
        if(lhs.getFunctionName().at(0) == '_')
            return true;
        else
            return false;
    }

    // Next check the return type
    if (lhs.getReturnType() < rhs.getReturnType())
        return true;
    if (lhs.getReturnType() > rhs.getReturnType())
        return false;

    // Check the number of operands
    if (lhs.mOperands.size() < rhs.mOperands.size())
        return true;
    if (lhs.mOperands.size() > rhs.mOperands.size())
        return false;

    // Now that we've gotten past the two quick tests, iterate over operands
    // Check the semantic and type.  The operands must be in the same order as well.
    OperandVector::const_iterator itLHSOps = lhs.mOperands.begin();
    OperandVector::const_iterator itRHSOps = rhs.mOperands.begin();

    for ( ; ((itLHSOps != lhs.mOperands.end()) && (itRHSOps != rhs.mOperands.end())); ++itLHSOps, ++itRHSOps)
    {
        if (itLHSOps->getSemantic() < itRHSOps->getSemantic())
            return true;
        if (itLHSOps->getSemantic() > itRHSOps->getSemantic())
            return false;

        GpuDataType leftType    = itLHSOps->getParameter()->getType();
        GpuDataType rightType   = itRHSOps->getParameter()->getType();
        
        if (Ogre::Root::getSingletonPtr()->getRender()->getName().find("OpenGL ES 2") != String::npos)
        {
            if (leftType == GDT_Sampler1D)
                leftType = GDT_Sampler2D;

            if (rightType == GDT_Sampler1D)
                rightType = GDT_Sampler2D;
        }

        // If a swizzle mask is being applied to the parameter, generate the GpuDataType to
        // perform the parameter type comparison the way that the compiler will see it.
        if ((itLHSOps->getFloatCount(itLHSOps->getMask()) > 0) ||
           (itRHSOps->getFloatCount(itRHSOps->getMask()) > 0))
        {
            if (itLHSOps->getFloatCount(itLHSOps->getMask()) > 0)
            {
                leftType = (GpuDataType)((itLHSOps->getParameter()->getType() - itLHSOps->getParameter()->getType()) +
                                             itLHSOps->getFloatCount(itLHSOps->getMask()));
            }
            if (itRHSOps->getFloatCount(itRHSOps->getMask()) > 0)
            {
                rightType = (GpuDataType)((itRHSOps->getParameter()->getType() - itRHSOps->getParameter()->getType()) +
                                             itRHSOps->getFloatCount(itRHSOps->getMask()));
            }
        }

        if (leftType < rightType)
            return true;
        if (leftType > rightType)
            return false;
    }

    return false;
}

bool FunctionInvocation::FunctionInvocationCompare::operator ()(FunctionInvocation const& lhs, FunctionInvocation const& rhs) const
{
    // Check the function names first
    if (lhs.getFunctionName() != rhs.getFunctionName())
        return false;

    // Next check the return type
    if (lhs.getReturnType() != rhs.getReturnType())
        return false;

    // Check the number of operands
    if (lhs.mOperands.size() != rhs.mOperands.size())
        return false;

    // Now that we've gotten past the two quick tests, iterate over operands
    // Check the semantic and type.  The operands must be in the same order as well.
    OperandVector::const_iterator itLHSOps = lhs.mOperands.begin();
    OperandVector::const_iterator itRHSOps = rhs.mOperands.begin();
    for ( ; ((itLHSOps != lhs.mOperands.end()) && (itRHSOps != rhs.mOperands.end())); ++itLHSOps, ++itRHSOps)
    {
        if (itLHSOps->getSemantic() != itRHSOps->getSemantic())
            return false;

        GpuDataType leftType    = itLHSOps->getParameter()->getType();
        GpuDataType rightType   = itRHSOps->getParameter()->getType();
        
        if (Ogre::Root::getSingletonPtr()->getRender()->getName().find("OpenGL ES 2") != String::npos)
        {
            if (leftType == GDT_Sampler1D)
                leftType = GDT_Sampler2D;

            if (rightType == GDT_Sampler1D)
                rightType = GDT_Sampler2D;
        }

        // If a swizzle mask is being applied to the parameter, generate the GpuDataType to
        // perform the parameter type comparison the way that the compiler will see it.
        if ((itLHSOps->getFloatCount(itLHSOps->getMask()) > 0) ||
           (itRHSOps->getFloatCount(itRHSOps->getMask()) > 0))
        {
            if (itLHSOps->getFloatCount(itLHSOps->getMask()) > 0)
            {
                leftType = (GpuDataType)((itLHSOps->getParameter()->getType() - itLHSOps->getParameter()->getType()) +
                                             itLHSOps->getFloatCount(itLHSOps->getMask()));
            }
            if (itRHSOps->getFloatCount(itRHSOps->getMask()) > 0)
            {
                rightType = (GpuDataType)((itRHSOps->getParameter()->getType() - itRHSOps->getParameter()->getType()) +
                                             itRHSOps->getFloatCount(itRHSOps->getMask()));
            }
        }

        if (leftType != rightType)
            return false;
    }

    // Passed all tests, they are the same
    return true;
}

}
}

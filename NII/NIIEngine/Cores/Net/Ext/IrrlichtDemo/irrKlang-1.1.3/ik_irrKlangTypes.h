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

#ifndef __IRRKLANG_TYPES_H_INCLUDED__
#define __IRRKLANG_TYPES_H_INCLUDED__


namespace irrklang
{

	//! 8 bit unsigned variable.
	/** This is a typedef for unsigned char, it ensures portability of the engine. */
	typedef unsigned char ik_u8;

	//! 8 bit signed variable.
	/** This is a typedef for signed char, it ensures portability of the engine. */
	typedef signed char	ik_s8;

	//! 8 bit character variable.
	/** This is a typedef for char, it ensures portability of the engine. */
	typedef char ik_c8;



	//! 16 bit unsigned variable.
	/** This is a typedef for unsigned short, it ensures portability of the engine. */
	typedef unsigned short ik_u16;

	//! 16 bit signed variable.
	/** This is a typedef for signed short, it ensures portability of the engine. */
	typedef signed short ik_s16;



	//! 32 bit unsigned variable.
	/** This is a typedef for unsigned int, it ensures portability of the engine. */
	typedef unsigned int ik_u32;

	//! 32 bit signed variable.
	/** This is a typedef for signed int, it ensures portability of the engine. */
	typedef signed int ik_s32;



	//! 32 bit floating point variable.
	/** This is a typedef for float, it ensures portability of the engine. */
	typedef float ik_f32;

	//! 64 bit floating point variable.
	/** This is a typedef for double, it ensures portability of the engine. */
	typedef double ik_f64;



    // some constants

	const ik_f32 IK_ROUNDING_ERROR_32	= 0.000001f;
	const ik_f64 IK_PI64			    = 3.1415926535897932384626433832795028841971693993751;
	const ik_f32 IK_PI32			    = 3.14159265359f;
	const ik_f32 IK_RADTODEG            = 180.0f / IK_PI32;
	const ik_f32 IK_DEGTORAD            = IK_PI32 / 180.0f;
	const ik_f64 IK_RADTODEG64          = 180.0 / IK_PI64;
	const ik_f64 IK_DEGTORAD64          = IK_PI64 / 180.0;

	//! returns if a float equals the other one, taking floating
	//! point rounding errors into account
	inline bool equalsfloat(const ik_f32 a, const ik_f32 b, const ik_f32 tolerance = IK_ROUNDING_ERROR_32)
	{
		return (a + tolerance > b) && (a - tolerance < b);
	}


} // end namespace


#endif // __IRR_TYPES_H_INCLUDED__


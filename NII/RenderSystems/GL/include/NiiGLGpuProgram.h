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

#ifndef _NII_GLGpuProgram_H_
#define _NII_GLGpuProgram_H_

#include "NiiGLPreInclude.h"
#include "NiiGpuProgram.h"
#include "NiiGpuBuffer.h"
#include "NiiVertexIndexIndirect.h"

namespace NII
{
    /** low-level GL 着色程序 (eg ARB and NV) 
	*/
    class _EngineGLAPI GLGpuProgram : public GpuProgram
    {
    public:
        GLGpuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0);
        virtual ~GLGpuProgram();

		/// 着色程序ID
		GLuint getProgramID() const             { return mProgramID; }

		/// 应用着色程序
        virtual void bindProgram() const              {}

		/// 解除着色程序 
        virtual void unbindProgram() const            {}

        /// 绑定着色程序参数
		virtual void bindProgramParameters(const GpuProgramParam * params, Nui16 mask) const {}

		/// 绑定着色程序参数
		virtual void bindProgramPassIterationParameters(const GpuProgramParam * params) const {}

        /// @copydoc Resource::calculateSize
        virtual NCount calcSize() const;
    protected:
		/// @copydetails GpuProgram::loadCodeImpl
		void loadCodeImpl(const VString & code) {}

        /// @copydetails Resource::unloadImpl
        void unloadImpl() {}
	protected:
        GLuint mProgramID;
    };

    /**low-level GL ARB 着色程序*/
    class _EngineGLAPI GLArbGpuProgram : public GLGpuProgram
    {
    public:
        GLArbGpuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0);
        virtual ~GLArbGpuProgram();

		/// 着色程序类型
        GLuint getProgramType() const;

        /// @copydetails GpuProgram::bindProgram
        void bindProgram() const;

        /// @copydetails GpuProgram::unbindProgram
        void unbindProgram() const;

        /// @copydetails GpuProgram::bindProgramParameters
		void bindProgramParameters(const GpuProgramParam * params, Nui16 mask) const;

		/// @copydetails GpuProgram::bindProgramPassIterationParameters
		void bindProgramPassIterationParameters(const GpuProgramParam * params) const;
    protected:
		/// @copydetails GLGpuProgram::loadCodeImpl
        void loadCodeImpl(const VString & code);

        /// @copydoc Resource::unloadImpl
        void unloadImpl();
    };
    
    class _EngineGLAPI GLGpuNvparseProgram : public GLGpuProgram
    {
    public:
        GLGpuNvparseProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs);
        virtual ~GLGpuNvparseProgram();

        /// @copydetails GLGpuProgram::bindProgram
        void bindProgram() const;

        /// @copydetails GLGpuProgram::unbindProgram
        void unbindProgram() const;

        /// @copydetails GLGpuProgram::bindProgramParameters
        void bindProgramParameters(const GpuProgramParam * params, Nui16 mask) const;
    protected:
        /// @copydoc Resource::unload
        void unloadImpl();

		/// @copydoc GLGpuProgram::loadCodeImpl
        void loadCodeImpl(const VString & code);
    };
    
	/// ATI_FS
	class _EngineGLAPI ATI_FS_GLGpuProgram : public GLGpuProgram
	{
	public:
        ATI_FS_GLGpuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0);
		virtual ~ATI_FS_GLGpuProgram();

		/// @copydetails GLGpuProgram::bindProgram
		void bindProgram() const;
        
		/// @copydetails GLGpuProgram::unbindProgram
		void unbindProgram() const;
        
		/// @copydetails GLGpuProgram::bindProgramParameters
		void bindProgramParameters(const GpuProgramParam * params, Nui16 mask)const;
        
		/// @copydetails GLGpuProgram::bindProgramPassIterationParameters
        void bindProgramPassIterationParameters(const GpuProgramParam * params)const;
	protected:
		/// @copydoc Resource::unload
		void unloadImpl();

		/// @copydetails GpuProgram::loadCodeImpl
		void loadCodeImpl(const VString & code);

        GLenum mProgramType;
	};
}
#endif
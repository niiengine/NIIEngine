/*
-----------------------------------------------------------------------------
大型多媒体框架
时间: 2018-5-7
文本编码: utf-8
所属公司: 深圳闽登科技有限公司
命名风格: 概论命名法
编程风格: 统筹式
管理模式: 分布式
内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)
主要成分: c++(80%) c(20%)
用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件
偏向用途: 立体游戏软件
主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com
------------------------------------------------------------------------------
*/
/*
Copyright (c) "2018-2019", Shenzhen Mindeng Technology Co., Ltd(www.niiengine.com),
		Mindeng Base Communication Application Framework
All rights reserved.
	Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
	Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and/or other materials
provided with the distribution.
	Neither the name of the "ORGANIZATION" nor the names of its contributors may be used
to endorse or promote products derived from this software without specific prior written
permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

    #define SvdEpsilon 1e-04
    #define SvdMaxIterations 32
    //------------------------------------------------------------------------
    template <typename TN> const NIIf       Matrix3<TN>::EPSILON = 1e-06;
    template <typename TN> const Matrix3<TN> Matrix3<TN>::ZERO = Matrix3<TN>(0,0,0,0,0,0,0,0,0);
    template <typename TN> const Matrix3<TN> Matrix3<TN>::IDENTITY = Matrix3<TN>(1,0,0,0,1,0,0,0,1);
    //------------------------------------------------------------------------
    template <typename TN> inline Matrix3<TN>::Matrix3()
    {
    }
    //------------------------------------------------------------------------
    template <typename TN> inline Matrix3<TN>::Matrix3(const Matrix3<TN> & o)
    {
        memcpy(m, o.m, 9 * sizeof(TN));
    }
    //------------------------------------------------------------------------
    template <typename TN> inline Matrix3<TN>::Matrix3(const Matrix4<TN> & o)
    {
        m[0][0] = o.m[0][0]; m[0][1] = o.m[0][1]; m[0][2] = o.m[0][2];
        m[1][0] = o.m[1][0]; m[1][1] = o.m[1][1]; m[1][2] = o.m[1][2];
        m[2][0] = o.m[2][0]; m[2][1] = o.m[2][1]; m[2][2] = o.m[2][2];
    }
    //------------------------------------------------------------------------
    template <typename TN> inline Matrix3<TN>::Matrix3 (
        TN _00, TN _01, TN _02, TN _10, TN _11, TN _12, TN _20, TN _21, TN _22)
    {
        m[0][0] = _00; m[0][1] = _01; m[0][2] = _02;
        m[1][0] = _10; m[1][1] = _11; m[1][2] = _12;
        m[2][0] = _20; m[2][1] = _21; m[2][2] = _22;
    }
    //------------------------------------------------------------------------
    template <typename TN> inline Matrix3<TN>::Matrix3(
        const Vector3<TN> & x, const Vector3<TN> & y, const Vector3<TN> & z)
    {
        set(0, x);
        set(1, y);
        set(2, z);
    }
    //------------------------------------------------------------------------
    template <typename TN> inline TN * Matrix3<TN>::operator[] (NCount row) const
    {
        return (TN*)m[row];
    }
    //------------------------------------------------------------------------
    template <typename TN> inline Matrix3<TN> & Matrix3<TN>::operator = (const Matrix3<TN> & o)
    {
        memcpy(m, o.m, 9 * sizeof(TN));
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename TN> inline Matrix3<TN> & Matrix3<TN>::operator = (const Matrix4<TN> & o)
    {
        m[0][0] = o.m[0][0]; m[0][1] = o.m[0][1]; m[0][2] = o.m[0][2];
        m[1][0] = o.m[1][0]; m[1][1] = o.m[1][1]; m[1][2] = o.m[1][2];
        m[2][0] = o.m[2][0]; m[2][1] = o.m[2][1]; m[2][2] = o.m[2][2];
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename TN> inline bool Matrix3<TN>::operator != (const Matrix3<TN> & o)const
    {
        return !operator==(o);
    }
    //------------------------------------------------------------------------
    template <typename TN> bool Matrix3<TN>::isScale() const
    {
        TN t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
        if(fabs(1.0 - t) > 1e-04)
            return true;

        t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
        if(fabs(1.0 - t) > 1e-04)
            return true;

        t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
        if(fabs(1.0 - t) > 1e-04)
            return true;

        return false;
    }
    //------------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::project(const Vector3<TN> & src,
        Vector3<TN> & dst)
    {
        dst.x = src * Vector3<TN>(m[0][0], m[0][1], m[0][2]);
        dst.y = src * Vector3<TN>(m[1][0], m[1][1], m[1][2]);
        dst.z = src * Vector3<TN>(m[2][0], m[2][1], m[2][2]);
    }
    //------------------------------------------------------------------------
    template <typename TN> inline Vector3<TN> Matrix3<TN>::get(size_t col) const
    {
        assert(col < 3);
        return Vector3<TN>(m[0][col], m[1][col], m[2][col]);
    }
    //-------------------------------------------------------------------
    template <typename TN> inline void Matrix3<TN>::set(size_t col, const Vector3<TN> & v)
    {
        assert(col < 3);
        m[0][col] = v.x;
        m[1][col] = v.y;
        m[2][col] = v.z;
    }
    //-------------------------------------------------------------------
    template <typename TN> bool Matrix3<TN>::operator == (const Matrix3<TN> & o) const
    {
        if( m[0][0] != o.m[0][0] || m[0][1] != o.m[0][1] || m[0][2] != o.m[0][2] ||
            m[1][0] != o.m[1][0] || m[1][1] != o.m[1][1] || m[1][2] != o.m[1][2] ||
            m[2][0] != o.m[2][0] || m[2][1] != o.m[2][1] || m[2][2] != o.m[2][2])
            return false;
        return true;
    }
    //-------------------------------------------------------------------
    template <typename TN> Matrix3<TN> Matrix3<TN>::operator + (const Matrix3<TN> & o) const
    {
        Matrix3<TN> re;
        re.m[0][0] = m[0][0] + o.m[0][0];
        re.m[0][1] = m[0][1] + o.m[0][1];
        re.m[0][2] = m[0][2] + o.m[0][2];
        re.m[1][0] = m[1][0] + o.m[1][0];
        re.m[1][1] = m[1][1] + o.m[1][1];
        re.m[1][2] = m[1][2] + o.m[1][2];
        re.m[2][0] = m[2][0] + o.m[2][0];
        re.m[2][1] = m[2][1] + o.m[2][1];
        re.m[2][2] = m[2][2] + o.m[2][2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> Matrix3<TN> Matrix3<TN>::operator - (const Matrix3<TN> & o) const
    {
        Matrix3<TN> re;
        re.m[0][0] = m[0][0] - o.m[0][0];
        re.m[0][1] = m[0][1] - o.m[0][1];
        re.m[0][2] = m[0][2] - o.m[0][2];
        re.m[1][0] = m[1][0] - o.m[1][0];
        re.m[1][1] = m[1][1] - o.m[1][1];
        re.m[1][2] = m[1][2] - o.m[1][2];
        re.m[2][0] = m[2][0] - o.m[2][0];
        re.m[2][1] = m[2][1] - o.m[2][1];
        re.m[2][2] = m[2][2] - o.m[2][2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> Matrix3<TN> Matrix3<TN>::operator * (const Matrix3<TN> & o) const
    {
        Matrix3<TN> re;
        re.m[0][0] = m[0][0]*o.m[0][0]+m[0][1]*o.m[1][0]+m[0][2]*o.m[2][0];
        re.m[0][1] = m[0][0]*o.m[0][1]+m[0][1]*o.m[1][1]+m[0][2]*o.m[2][1];
        re.m[0][2] = m[0][0]*o.m[0][2]+m[0][1]*o.m[1][2]+m[0][2]*o.m[2][2];
        re.m[1][0] = m[1][0]*o.m[0][0]+m[1][1]*o.m[1][0]+m[1][2]*o.m[2][0];
        re.m[1][1] = m[1][0]*o.m[0][1]+m[1][1]*o.m[1][1]+m[1][2]*o.m[2][1];
        re.m[1][2] = m[1][0]*o.m[0][2]+m[1][1]*o.m[1][2]+m[1][2]*o.m[2][2];
        re.m[2][0] = m[2][0]*o.m[0][0]+m[2][1]*o.m[1][0]+m[2][2]*o.m[2][0];
        re.m[2][1] = m[2][0]*o.m[0][1]+m[2][1]*o.m[1][1]+m[2][2]*o.m[2][1];
        re.m[2][2] = m[2][0]*o.m[0][2]+m[2][1]*o.m[1][2]+m[2][2]*o.m[2][2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> Vector3<TN> Matrix3<TN>::operator * (const Vector3<TN> & o) const
    {
        Vector3<TN> re;
        re[0] = m[0][0] * o[0] + m[0][1] * o[1] + m[0][2] * o[2];
        re[1] = m[1][0] * o[0] + m[1][1] * o[1] + m[1][2] * o[2];
        re[2] = m[2][0] * o[0] + m[2][1] * o[1] + m[2][2] * o[2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> Vector3<TN> operator * (const Vector3<TN> & v, const Matrix3<TN> & m)
    {
        Vector3<TN> re;
        re[0] = v[0] * m.m[0][0] + v[1] * m.m[1][0] + v[2] * m.m[2][0];
        re[1] = v[0] * m.m[0][1] + v[1] * m.m[1][1] + v[2] * m.m[2][1];
        re[2] = v[0] * m.m[0][2] + v[1] * m.m[1][2] + v[2] * m.m[2][2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> Matrix3<TN> Matrix3<TN>::operator - () const
    {
        Matrix3<TN> re;
        re[0][0] = -m[0][0]; re[0][1] = -m[0][1]; re[0][2] = -m[0][2];
        re[1][0] = -m[1][0]; re[1][1] = -m[1][1]; re[1][2] = -m[1][2];
        re[2][0] = -m[2][0]; re[2][1] = -m[2][1]; re[2][2] = -m[2][2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> Matrix3<TN> Matrix3<TN>::operator * (TN scale) const
    {
        Matrix3<TN> re;
        re[0][0] = scale * m[0][0];
        re[0][1] = scale * m[0][1];
        re[0][2] = scale * m[0][2];
        re[1][0] = scale * m[1][0];
        re[1][1] = scale * m[1][1];
        re[1][2] = scale * m[1][2];
        re[2][0] = scale * m[2][0];
        re[2][1] = scale * m[2][1];
        re[2][2] = scale * m[2][2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> Matrix3<TN> operator * (TN scale, const Matrix3<TN>& o)
    {
        Matrix3<TN> re;
        re[0][0] = scale * o.m[0][0];
        re[0][1] = scale * o.m[0][1];
        re[0][2] = scale * o.m[0][2];
        re[1][0] = scale * o.m[1][0];
        re[1][1] = scale * o.m[1][1];
        re[1][2] = scale * o.m[1][2];
        re[2][0] = scale * o.m[2][0];
        re[2][1] = scale * o.m[2][1];
        re[2][2] = scale * o.m[2][2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> Matrix3<TN> Matrix3<TN>::T() const
    {
        Matrix3<TN> re;
        re[0][0] = m[0][0]; re[0][1] = m[1][0]; re[0][2] = m[2][0];
        re[1][0] = m[0][1]; re[1][1] = m[1][1]; re[1][2] = m[2][1];
        re[2][0] = m[0][2]; re[2][1] = m[1][2]; re[2][2] = m[2][2];
        return re;
    }
    //-------------------------------------------------------------------
    template <typename TN> bool Matrix3<TN>::inverse(Matrix3<TN> & m3, NIIf functor) const
    {
        // Invert a 3x3 using cofactors.  This is about 8 times faster than
        // the Numerical Recipes code which uses Gaussian elimination.

        m3[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];
        m3[0][1] = m[0][2]*m[2][1] - m[0][1]*m[2][2];
        m3[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];
        m3[1][0] = m[1][2]*m[2][0] - m[1][0]*m[2][2];
        m3[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];
        m3[1][2] = m[0][2]*m[1][0] - m[0][0]*m[1][2];
        m3[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0];
        m3[2][1] = m[0][1]*m[2][0] - m[0][0]*m[2][1];
        m3[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0];

        TN fDet = m[0][0] * m3[0][0] + m[0][1] * m3[1][0]+ m[0][2] * m3[2][0];

        if(Math::Abs(fDet) <= functor)
            return false;

        TN fInvDet = 1.0f/fDet;
        m3[0][0] *= fInvDet; m3[0][1] *= fInvDet; m3[0][2] *= fInvDet;
        m3[1][0] *= fInvDet; m3[1][1] *= fInvDet; m3[1][2] *= fInvDet;
        m3[2][0] *= fInvDet; m3[2][1] *= fInvDet; m3[2][2] *= fInvDet;
        return true;
    }
    //-------------------------------------------------------------------
    template <typename TN> Matrix3<TN> Matrix3<TN>::inverse(TN functor) const
    {
        Matrix3<TN> kInverse = Matrix3<TN>::ZERO;
        inverse(kInverse, functor);
        return kInverse;
    }
    //-------------------------------------------------------------------
    template <typename TN> TN Matrix3<TN>::det() const
    {
        TN _00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
        TN _10 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
        TN _20 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

        TN fDet = m[0][0] * _00 + m[0][1] * _10 + m[0][2] * _20;

        return fDet;
    }
    //-------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::Bidiagonalize(Matrix3<TN> & kA,
        Matrix3<TN> & kL, Matrix3<TN> & kR)
    {
        TN afV[3], afW[3];
        TN fLength, fSign, fT1, fInvT1, fT2;
        bool bIdentity;

        // map first column to (*,0,0)
        fLength = Math::sqrt(kA[0][0]*kA[0][0] + kA[1][0]*kA[1][0] + kA[2][0]*kA[2][0]);
        if ( fLength > 0.0 )
        {
            fSign = (kA[0][0] > 0.0f ? 1.0f : -1.0f);
            fT1 = kA[0][0] + fSign*fLength;
            fInvT1 = 1.0f/fT1;
            afV[1] = kA[1][0]*fInvT1;
            afV[2] = kA[2][0]*fInvT1;

            fT2 = -2.0f/(1.0f+afV[1]*afV[1]+afV[2]*afV[2]);
            afW[0] = fT2*(kA[0][0]+kA[1][0]*afV[1]+kA[2][0]*afV[2]);
            afW[1] = fT2*(kA[0][1]+kA[1][1]*afV[1]+kA[2][1]*afV[2]);
            afW[2] = fT2*(kA[0][2]+kA[1][2]*afV[1]+kA[2][2]*afV[2]);
            kA[0][0] += afW[0];
            kA[0][1] += afW[1];
            kA[0][2] += afW[2];
            kA[1][1] += afV[1]*afW[1];
            kA[1][2] += afV[1]*afW[2];
            kA[2][1] += afV[2]*afW[1];
            kA[2][2] += afV[2]*afW[2];

            kL[0][0] = 1.0f+fT2;
            kL[0][1] = kL[1][0] = fT2*afV[1];
            kL[0][2] = kL[2][0] = fT2*afV[2];
            kL[1][1] = 1.0f+fT2*afV[1]*afV[1];
            kL[1][2] = kL[2][1] = fT2*afV[1]*afV[2];
            kL[2][2] = 1.0f+fT2*afV[2]*afV[2];
            bIdentity = false;
        }
        else
        {
            kL = Matrix3<TN>::IDENTITY;
            bIdentity = true;
        }

        // map first row to (*,*,0)
        fLength = Math::sqrt(kA[0][1]*kA[0][1]+kA[0][2]*kA[0][2]);
        if ( fLength > 0.0 )
        {
            fSign = (kA[0][1] > 0.0f ? 1.0f : -1.0f);
            fT1 = kA[0][1] + fSign*fLength;
            afV[2] = kA[0][2]/fT1;

            fT2 = -2.0f/(1.0f+afV[2]*afV[2]);
            afW[0] = fT2*(kA[0][1]+kA[0][2]*afV[2]);
            afW[1] = fT2*(kA[1][1]+kA[1][2]*afV[2]);
            afW[2] = fT2*(kA[2][1]+kA[2][2]*afV[2]);
            kA[0][1] += afW[0];
            kA[1][1] += afW[1];
            kA[1][2] += afW[1]*afV[2];
            kA[2][1] += afW[2];
            kA[2][2] += afW[2]*afV[2];

            kR[0][0] = 1.0;
            kR[0][1] = kR[1][0] = 0.0;
            kR[0][2] = kR[2][0] = 0.0;
            kR[1][1] = 1.0f+fT2;
            kR[1][2] = kR[2][1] = fT2*afV[2];
            kR[2][2] = 1.0f+fT2*afV[2]*afV[2];
        }
        else
        {
            kR = Matrix3<TN>::IDENTITY;
        }

        // map second column to (*,*,0)
        fLength = Math::sqrt(kA[1][1]*kA[1][1]+kA[2][1]*kA[2][1]);
        if ( fLength > 0.0 )
        {
            fSign = (kA[1][1] > 0.0f ? 1.0f : -1.0f);
            fT1 = kA[1][1] + fSign*fLength;
            afV[2] = kA[2][1]/fT1;

            fT2 = -2.0f/(1.0f+afV[2]*afV[2]);
            afW[1] = fT2*(kA[1][1]+kA[2][1]*afV[2]);
            afW[2] = fT2*(kA[1][2]+kA[2][2]*afV[2]);
            kA[1][1] += afW[1];
            kA[1][2] += afW[2];
            kA[2][2] += afV[2]*afW[2];

            TN fA = 1.0f+fT2;
            TN fB = fT2*afV[2];
            TN fC = 1.0f+fB*afV[2];

            if ( bIdentity )
            {
                kL[0][0] = 1.0;
                kL[0][1] = kL[1][0] = 0.0;
                kL[0][2] = kL[2][0] = 0.0;
                kL[1][1] = fA;
                kL[1][2] = kL[2][1] = fB;
                kL[2][2] = fC;
            }
            else
            {
                for (NIIi iRow = 0; iRow < 3; iRow++)
                {
                    TN fTmp0 = kL[iRow][1];
                    TN fTmp1 = kL[iRow][2];
                    kL[iRow][1] = fA*fTmp0+fB*fTmp1;
                    kL[iRow][2] = fB*fTmp0+fC*fTmp1;
                }
            }
        }
    }
    //-------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::GolubKahanStep(Matrix3<TN> & kA,
        Matrix3<TN> & kL, Matrix3<TN> & kR)
    {
        TN fT11 = kA[0][1]*kA[0][1]+kA[1][1]*kA[1][1];
        TN fT22 = kA[1][2]*kA[1][2]+kA[2][2]*kA[2][2];
        TN fT12 = kA[1][1]*kA[1][2];
        TN fTrace = fT11+fT22;
        TN fDiff = fT11-fT22;
        TN fDiscr = Math::sqrt(fDiff*fDiff+4.0f*fT12*fT12);
        TN fRoot1 = 0.5f*(fTrace+fDiscr);
        TN fRoot2 = 0.5f*(fTrace-fDiscr);

        // adjust right
        TN fY = kA[0][0] - (Math::Abs(fRoot1-fT22) <= Math::Abs(fRoot2-fT22) ? fRoot1 : fRoot2);
        TN fZ = kA[0][1];
		TN fInvLength = Math::invSqrt(fY*fY+fZ*fZ);
        TN fSin = fZ*fInvLength;
        TN fCos = -fY*fInvLength;

        TN fTmp0 = kA[0][0];
        TN fTmp1 = kA[0][1];
        kA[0][0] = fCos*fTmp0-fSin*fTmp1;
        kA[0][1] = fSin*fTmp0+fCos*fTmp1;
        kA[1][0] = -fSin*kA[1][1];
        kA[1][1] *= fCos;

        size_t iRow;
        for (iRow = 0; iRow < 3; iRow++)
        {
            fTmp0 = kR[0][iRow];
            fTmp1 = kR[1][iRow];
            kR[0][iRow] = fCos*fTmp0-fSin*fTmp1;
            kR[1][iRow] = fSin*fTmp0+fCos*fTmp1;
        }

        // adjust left
        fY = kA[0][0];
        fZ = kA[1][0];
        fInvLength = Math::invSqrt(fY*fY+fZ*fZ);
        fSin = fZ*fInvLength;
        fCos = -fY*fInvLength;

        kA[0][0] = fCos*kA[0][0]-fSin*kA[1][0];
        fTmp0 = kA[0][1];
        fTmp1 = kA[1][1];
        kA[0][1] = fCos*fTmp0-fSin*fTmp1;
        kA[1][1] = fSin*fTmp0+fCos*fTmp1;
        kA[0][2] = -fSin*kA[1][2];
        kA[1][2] *= fCos;

        size_t iCol;
        for (iCol = 0; iCol < 3; iCol++)
        {
            fTmp0 = kL[iCol][0];
            fTmp1 = kL[iCol][1];
            kL[iCol][0] = fCos*fTmp0-fSin*fTmp1;
            kL[iCol][1] = fSin*fTmp0+fCos*fTmp1;
        }

        // adjust right
        fY = kA[0][1];
        fZ = kA[0][2];
        fInvLength = Math::invSqrt(fY*fY+fZ*fZ);
        fSin = fZ*fInvLength;
        fCos = -fY*fInvLength;

        kA[0][1] = fCos*kA[0][1]-fSin*kA[0][2];
        fTmp0 = kA[1][1];
        fTmp1 = kA[1][2];
        kA[1][1] = fCos*fTmp0-fSin*fTmp1;
        kA[1][2] = fSin*fTmp0+fCos*fTmp1;
        kA[2][1] = -fSin*kA[2][2];
        kA[2][2] *= fCos;

        for (iRow = 0; iRow < 3; iRow++)
        {
            fTmp0 = kR[1][iRow];
            fTmp1 = kR[2][iRow];
            kR[1][iRow] = fCos*fTmp0-fSin*fTmp1;
            kR[2][iRow] = fSin*fTmp0+fCos*fTmp1;
        }

        // adjust left
        fY = kA[1][1];
        fZ = kA[2][1];
        fInvLength = Math::invSqrt(fY*fY+fZ*fZ);
        fSin = fZ*fInvLength;
        fCos = -fY*fInvLength;

        kA[1][1] = fCos*kA[1][1]-fSin*kA[2][1];
        fTmp0 = kA[1][2];
        fTmp1 = kA[2][2];
        kA[1][2] = fCos*fTmp0-fSin*fTmp1;
        kA[2][2] = fSin*fTmp0+fCos*fTmp1;

        for (iCol = 0; iCol < 3; iCol++)
        {
            fTmp0 = kL[iCol][1];
            fTmp1 = kL[iCol][2];
            kL[iCol][1] = fCos*fTmp0-fSin*fTmp1;
            kL[iCol][2] = fSin*fTmp0+fCos*fTmp1;
        }
    }
    //-------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::SVD(Matrix3<TN>& kL, Vector3<TN> & kS,
        Matrix3<TN> & kR) const
    {
        // temas: currently unused
        //const NIIi iMax = 16;
		size_t iRow, iCol;

        Matrix3<TN> kA = *this;
        Bidiagonalize(kA,kL,kR);

        for(Nui32 i = 0; i < SvdMaxIterations; i++)
        {
            TN fTmp, fTmp0, fTmp1;
            TN fSin0, fCos0, fTan0;
            TN fSin1, fCos1, fTan1;

            bool bTest1 = (Math::Abs(kA[0][1]) <= SvdEpsilon * (Math::Abs(kA[0][0])+Math::Abs(kA[1][1])));
            bool bTest2 = (Math::Abs(kA[1][2]) <= SvdEpsilon * (Math::Abs(kA[1][1])+Math::Abs(kA[2][2])));
            if ( bTest1 )
            {
                if ( bTest2 )
                {
                    kS[0] = kA[0][0];
                    kS[1] = kA[1][1];
                    kS[2] = kA[2][2];
                    break;
                }
                else
                {
                    // 2x2 closed form factorization
                    fTmp =
                        (kA[1][1]*kA[1][1] - kA[2][2]*kA[2][2] +
                        kA[1][2]*kA[1][2])/(kA[1][2]*kA[2][2]);
                    fTan0 = 0.5f*(fTmp+Math::sqrt(fTmp*fTmp + 4.0f));
                    fCos0 = Math::invSqrt(1.0f+fTan0*fTan0);
                    fSin0 = fTan0*fCos0;

                    for (iCol = 0; iCol < 3; iCol++)
                    {
                        fTmp0 = kL[iCol][1];
                        fTmp1 = kL[iCol][2];
                        kL[iCol][1] = fCos0*fTmp0-fSin0*fTmp1;
                        kL[iCol][2] = fSin0*fTmp0+fCos0*fTmp1;
                    }

                    fTan1 = (kA[1][2]-kA[2][2]*fTan0)/kA[1][1];
                    fCos1 = Math::invSqrt(1.0f+fTan1*fTan1);
                    fSin1 = -fTan1*fCos1;

                    for (iRow = 0; iRow < 3; iRow++)
                    {
                        fTmp0 = kR[1][iRow];
                        fTmp1 = kR[2][iRow];
                        kR[1][iRow] = fCos1*fTmp0-fSin1*fTmp1;
                        kR[2][iRow] = fSin1*fTmp0+fCos1*fTmp1;
                    }

                    kS[0] = kA[0][0];
                    kS[1] = fCos0*fCos1*kA[1][1] - fSin1*(fCos0*kA[1][2]-fSin0*kA[2][2]);
                    kS[2] = fSin0*fSin1*kA[1][1] + fCos1*(fSin0*kA[1][2]+fCos0*kA[2][2]);
                    break;
                }
            }
            else
            {
                if ( bTest2 )
                {
                    // 2x2 closed form factorization
                    fTmp =
                        (kA[0][0]*kA[0][0] + kA[1][1]*kA[1][1] -
                        kA[0][1]*kA[0][1])/(kA[0][1]*kA[1][1]);
                    fTan0 = 0.5f*(-fTmp+Math::sqrt(fTmp*fTmp + 4.0f));
                    fCos0 = Math::invSqrt(1.0f+fTan0*fTan0);
                    fSin0 = fTan0*fCos0;

                    for (iCol = 0; iCol < 3; iCol++)
                    {
                        fTmp0 = kL[iCol][0];
                        fTmp1 = kL[iCol][1];
                        kL[iCol][0] = fCos0*fTmp0-fSin0*fTmp1;
                        kL[iCol][1] = fSin0*fTmp0+fCos0*fTmp1;
                    }

                    fTan1 = (kA[0][1]-kA[1][1]*fTan0)/kA[0][0];
                    fCos1 = Math::invSqrt(1.0f+fTan1*fTan1);
                    fSin1 = -fTan1*fCos1;

                    for (iRow = 0; iRow < 3; iRow++)
                    {
                        fTmp0 = kR[0][iRow];
                        fTmp1 = kR[1][iRow];
                        kR[0][iRow] = fCos1*fTmp0-fSin1*fTmp1;
                        kR[1][iRow] = fSin1*fTmp0+fCos1*fTmp1;
                    }

                    kS[0] = fCos0 * fCos1 * kA[0][0] -
                        fSin1 * (fCos0 * kA[0][1] - fSin0 * kA[1][1]);
                    kS[1] = fSin0 * fSin1 * kA[0][0] +
                        fCos1 * (fSin0 * kA[0][1] + fCos0 * kA[1][1]);
                    kS[2] = kA[2][2];
                    break;
                }
                else
                {
                    GolubKahanStep(kA, kL, kR);
                }
            }
        }

        // positize diagonal
        for (iRow = 0; iRow < 3; iRow++)
        {
            if ( kS[iRow] < 0.0 )
            {
                kS[iRow] = -kS[iRow];
                for (iCol = 0; iCol < 3; iCol++)
                    kR[iRow][iCol] = -kR[iRow][iCol];
            }
        }
    }
    //-------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::SVC(const Matrix3<TN> & kL,
        const Vector3<TN> & kS, const Matrix3<TN> & kR)
    {
        NCount row, col;
        Matrix3<TN> temp;

        // product S * R
        for(row = 0; row < 3; ++row)
        {
            for(col = 0; col < 3; ++col)
                temp[row][col] = kS[row] * kR[row][col];
        }

        // product L * S * R
        for(row = 0; row < 3; ++row)
        {
            for(col = 0; col < 3; ++col)
            {
                m[row][col] = 0.0;
                for(NCount mid = 0; mid < 3; ++mid)
                    m[row][col] += kL[row][mid] * temp[mid][col];
            }
        }
    }
    //-------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::Orthonormalize()
    {
        // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
        // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
        //
        //   q0 = m0/|m0|
        //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
        //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
        //
        // where |V| indicates length of vector V and A*B indicates dot
        // product of vectors A and B.

        // compute q0
        TN fInvLength = Math::invSqrt(m[0][0]*m[0][0] + m[1][0]*m[1][0] + m[2][0]*m[2][0]);

        m[0][0] *= fInvLength;
        m[1][0] *= fInvLength;
        m[2][0] *= fInvLength;

        // compute q1
        TN fDot0 = m[0][0]*m[0][1] + m[1][0]*m[1][1] + m[2][0]*m[2][1];

        m[0][1] -= fDot0*m[0][0];
        m[1][1] -= fDot0*m[1][0];
        m[2][1] -= fDot0*m[2][0];

        fInvLength = Math::invSqrt(m[0][1]*m[0][1] + m[1][1]*m[1][1] + m[2][1]*m[2][1]);

        m[0][1] *= fInvLength;
        m[1][1] *= fInvLength;
        m[2][1] *= fInvLength;

        // compute q2
        TN fDot1 = m[0][1]*m[0][2] + m[1][1]*m[1][2] + m[2][1]*m[2][2];

        fDot0 = m[0][0]*m[0][2] + m[1][0]*m[1][2] + m[2][0]*m[2][2];

        m[0][2] -= fDot0*m[0][0] + fDot1*m[0][1];
        m[1][2] -= fDot0*m[1][0] + fDot1*m[1][1];
        m[2][2] -= fDot0*m[2][0] + fDot1*m[2][1];

        fInvLength = Math::invSqrt(m[0][2]*m[0][2] + m[1][2]*m[1][2] + m[2][2]*m[2][2]);

        m[0][2] *= fInvLength;
        m[1][2] *= fInvLength;
        m[2][2] *= fInvLength;
    }
    //-------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::getQDU(Matrix3<TN> & kQ, Vector3<TN> & kD,
        Vector3<TN> & kU) const
    {
        // Factor M = QR = QDU where Q is orthogonal, D is diagonal,
        // and U is upper triangular with ones on its diagonal.  Algorithm uses
        // Gram-Schmidt orthogonalization (the QR algorithm).
        //
        // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
        //
        //   q0 = m0/|m0|
        //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
        //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
        //
        // where |V| indicates length of vector V and A*B indicates dot
        // product of vectors A and B.  The matrix R has entries
        //
        //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
        //   r10 = 0      r11 = q1*m1  r12 = q1*m2
        //   r20 = 0      r21 = 0      r22 = q2*m2
        //
        // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
        // u02 = r02/r00, and u12 = r12/r11.

        // Q = rotation
        // D = scaling
        // U = shear

        // D stores the three diagonal entries r00, r11, r22
        // U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

        // build orthogonal matrix Q
        NIIf fInvLength = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
        if(fabs(0 - fInvLength) > 1e-06)
            fInvLength = Math::invSqrt(fInvLength);

        kQ[0][0] = m[0][0] * fInvLength;
        kQ[1][0] = m[1][0] * fInvLength;
        kQ[2][0] = m[2][0] * fInvLength;

        TN fDot = kQ[0][0] * m[0][1] + kQ[1][0] * m[1][1] + kQ[2][0] * m[2][1];
        kQ[0][1] = m[0][1]-fDot * kQ[0][0];
        kQ[1][1] = m[1][1]-fDot * kQ[1][0];
        kQ[2][1] = m[2][1]-fDot * kQ[2][0];
        fInvLength = kQ[0][1] * kQ[0][1] + kQ[1][1] * kQ[1][1] + kQ[2][1] * kQ[2][1];
        if(fabs(0 - fInvLength) > 1e-06)
            fInvLength = Math::invSqrt(fInvLength);

        kQ[0][1] *= fInvLength;
        kQ[1][1] *= fInvLength;
        kQ[2][1] *= fInvLength;

        fDot = kQ[0][0] * m[0][2] + kQ[1][0] * m[1][2] + kQ[2][0] * m[2][2];
        kQ[0][2] = m[0][2]-fDot * kQ[0][0];
        kQ[1][2] = m[1][2]-fDot * kQ[1][0];
        kQ[2][2] = m[2][2]-fDot * kQ[2][0];
        fDot = kQ[0][1] * m[0][2] + kQ[1][1] * m[1][2] + kQ[2][1] * m[2][2];
        kQ[0][2] -= fDot * kQ[0][1];
        kQ[1][2] -= fDot * kQ[1][1];
        kQ[2][2] -= fDot * kQ[2][1];
        fInvLength = kQ[0][2] * kQ[0][2] + kQ[1][2] * kQ[1][2] + kQ[2][2] * kQ[2][2];
        if(fabs(0 - fInvLength) > 1e-06)
            fInvLength = Math::invSqrt(fInvLength);

        kQ[0][2] *= fInvLength;
        kQ[1][2] *= fInvLength;
        kQ[2][2] *= fInvLength;

        // guarantee that orthogonal matrix has determinant 1 (no reflections)
        TN fDet = kQ[0][0]*kQ[1][1]*kQ[2][2] + kQ[0][1]*kQ[1][2]*kQ[2][0] +
            kQ[0][2]*kQ[1][0]*kQ[2][1] - kQ[0][2]*kQ[1][1]*kQ[2][0] -
            kQ[0][1]*kQ[1][0]*kQ[2][2] - kQ[0][0]*kQ[1][2]*kQ[2][1];

        if ( fDet < 0.0 )
        {
            for (size_t iRow = 0; iRow < 3; iRow++)
                for (size_t iCol = 0; iCol < 3; iCol++)
                    kQ[iRow][iCol] = -kQ[iRow][iCol];
        }

        // build "right" matrix R
        Matrix3<TN> kR;
        kR[0][0] = kQ[0][0]*m[0][0] + kQ[1][0]*m[1][0] + kQ[2][0]*m[2][0];
        kR[0][1] = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] + kQ[2][0]*m[2][1];
        kR[1][1] = kQ[0][1]*m[0][1] + kQ[1][1]*m[1][1] + kQ[2][1]*m[2][1];
        kR[0][2] = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] + kQ[2][0]*m[2][2];
        kR[1][2] = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] + kQ[2][1]*m[2][2];
        kR[2][2] = kQ[0][2]*m[0][2] + kQ[1][2]*m[1][2] + kQ[2][2]*m[2][2];

        // the scaling component
        kD[0] = kR[0][0];
        kD[1] = kR[1][1];
        kD[2] = kR[2][2];

        // the shear component
        TN fInvD0 = 1.0f/kD[0];
        kU[0] = kR[0][1]*fInvD0;
        kU[1] = kR[0][2]*fInvD0;
        kU[2] = kR[1][2]/kD[1];
    }
    //-------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::to(Vector3<TN> & axis, Radian & radian) const
    {
        // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
        // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
        // I is the identity and
        //
        //       +-        -+
        //   P = |  0 -z +y |
        //       | +z  0 -x |
        //       | -y +x  0 |
        //       +-        -+
        //
        // If A > 0, R represents a counterclockwise rotation about the axis in
        // the sense of looking from the tip of the axis vector towards the
        // origin.  Some algebra will show that
        //
        //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
        //
        // In the event that A = pi, R-R^t = 0 which prevents us from extracting
        // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
        // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
        // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
        // it does not matter which sign you choose on the square roots.

        TN fTrace = m[0][0] + m[1][1] + m[2][2];
        TN fCos = 0.5f * (fTrace-1.0f);
        radian = Math::ACos(fCos);  // in [0,PI]

        if(radian > Radian(0.0))
        {
            if(radian < Radian(Math::PI))
            {
                axis.x = m[2][1]-m[1][2];
                axis.y = m[0][2]-m[2][0];
                axis.z = m[1][0]-m[0][1];
                axis.normalise();
            }
            else
            {
                // angle is PI
                NIIf fHalfInverse;
                if(m[0][0] >= m[1][1])
                {
                    // r00 >= r11
                    if(m[0][0] >= m[2][2])
                    {
                        // r00 is maximum diagonal term
                        axis.x = 0.5f * Math::sqrt(m[0][0] - m[1][1] - m[2][2] + 1.0f);
                        fHalfInverse = 0.5f/axis.x;
                        axis.y = fHalfInverse*m[0][1];
                        axis.z = fHalfInverse*m[0][2];
                    }
                    else
                    {
                        // r22 is maximum diagonal term
                        axis.z = 0.5f * Math::sqrt(m[2][2] - m[0][0] - m[1][1] + 1.0f);
                        fHalfInverse = 0.5f/axis.z;
                        axis.x = fHalfInverse*m[0][2];
                        axis.y = fHalfInverse*m[1][2];
                    }
                }
                else
                {
                    // r11 > r00
                    if ( m[1][1] >= m[2][2] )
                    {
                        // r11 is maximum diagonal term
                        axis.y = 0.5f * Math::sqrt(m[1][1] - m[0][0] - m[2][2] + 1.0f);
                        fHalfInverse  = 0.5f / axis.y;
                        axis.x = fHalfInverse * m[0][1];
                        axis.z = fHalfInverse * m[1][2];
                    }
                    else
                    {
                        // r22 is maximum diagonal term
                        axis.z = 0.5f * Math::sqrt(m[2][2] - m[0][0] - m[1][1] + 1.0f);
                        fHalfInverse = 0.5f / axis.z;
                        axis.x = fHalfInverse * m[0][2];
                        axis.y = fHalfInverse * m[1][2];
                    }
                }
            }
        }
        else
        {
            // The angle is 0 and the matrix is the identity.  Any axis will
            // work, so just use the x-axis.
            axis.x = 1.0;
            axis.y = 0.0;
            axis.z = 0.0;
        }
    }
    //-------------------------------------------------------------------
    template <typename TN> void Matrix3<TN>::from(const Vector3<TN> & axis, const Radian & radian)
    {
        TN fCos = Math::Cos(radian);
        TN fSin = Math::Sin(radian);
        TN fOneMinusCos = 1.0f-fCos;
        TN fX2 = axis.x * axis.x;
        TN fY2 = axis.y * axis.y;
        TN fZ2 = axis.z * axis.z;
        TN fXYM = axis.x * axis.y * fOneMinusCos;
        TN fXZM = axis.x * axis.z * fOneMinusCos;
        TN fYZM = axis.y * axis.z * fOneMinusCos;
        TN fXSin = axis.x * fSin;
        TN fYSin = axis.y * fSin;
        TN fZSin = axis.z * fSin;

        m[0][0] = fX2 * fOneMinusCos+fCos;
        m[0][1] = fXYM - fZSin;
        m[0][2] = fXZM + fYSin;
        m[1][0] = fXYM + fZSin;
        m[1][1] = fY2 * fOneMinusCos+fCos;
        m[1][2] = fYZM - fXSin;
        m[2][0] = fXZM - fYSin;
        m[2][1] = fYZM + fXSin;
        m[2][2] = fZ2 * fOneMinusCos+fCos;
    }
    //-------------------------------------------------------------------
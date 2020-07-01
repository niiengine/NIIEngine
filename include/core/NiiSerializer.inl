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
//内联
    //------------------------------------------------------------------------
    inline void Serializer::write(Ni8 in)
    {
        write(&in, sizeof(Ni8), 1, false);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Ni8 * in, NCount count)
    {
        write(in, sizeof(Ni8), count, false);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Nui8 in)
    {
        write(&in, sizeof(Nui8), 1, false);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Nui8 * in, NCount count)
    {
        write(in, sizeof(Nui8), count, false);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Ni16 in)
    {
        write(&in, sizeof(Ni16), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Ni16 * in, NCount count)
    {
        write(in, sizeof(Ni16), count);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Nui16 in)
    {
        write(&in, sizeof(Nui16), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Nui16 * in, NCount count)
    {
        write(in, sizeof(Nui16), count);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Ni32 in)
    {
        write(&in, sizeof(Ni32), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Ni32 * in, NCount count)
    {
        write(in, sizeof(Ni32), count);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Nui32 in)
    {
        write(&in, sizeof(Nui32), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Nui32 * in, NCount count)
    {
        write(in, sizeof(Nui32), count);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(NIIf in)
    {
        write(&in, sizeof(NIIf), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const NIIf * in, NCount count)
    {
        write(in, sizeof(NIIf), count);
    }
    //-----------------------------------------------------------------------
    inline void Serializer::write(const Ray & in)
    {
        write(&in.mOrigin);
        write(&in.mDirection);
    }
    //-----------------------------------------------------------------------
    inline void Serializer::write(const Sphere & in)
    {
        write(&in.mCenter);
        write(&in.mRadius);
    }
    //-----------------------------------------------------------------------
    inline void Serializer::write(const Quaternion & in)
    {
        write(&in.w, sizeof(NIIf), 4);
    }
    //-----------------------------------------------------------------------
    inline void Serializer::write(const Quaternion * in, NCount count)
    {
        write(&in->w, sizeof(NIIf), 4 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector2<t> & in)
    {
        write(&in.x, sizeof(t), 2);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector2<t> * in, NCount count)
    {
        write(&in->x, sizeof(t), 2 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector3<t> & in)
    {
        write(&in.x, sizeof(t), 3);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector3<t> * in, NCount count)
    {
        write(&in->x, sizeof(t), 3 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector4<t> & in)
    {
        write(&in.x, sizeof(t), 4);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector4<t> * in, NCount count)
    {
        write(&in->x, sizeof(t), 4 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Matrix3<t> & in)
    {
        write(in._m, sizeof(t), 9);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Matrix3<t> * in, NCount count)
    {
        write(in->_m, sizeof(t), 9 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Matrix4<t> & in)
    {
        write(in._m, sizeof(t), 16);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Matrix4<t> * in, NCount count)
    {
        write(in->_m, sizeof(t), 16 * count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Ni8 * out, NCount count) const
    {
        return read(out, sizeof(Ni8), count, false);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Nui8 * out, NCount count) const
    {
        return read(out, sizeof(Nui8), count, false);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Ni16 * out, NCount count) const
    {
        return read(out, sizeof(Ni16), count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Nui16 * out, NCount count) const
    {
        return read(out, sizeof(Nui16), count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Ni32 * out, NCount count) const
    {
        return read(out, sizeof(Ni32), count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Nui32 * out, NCount count) const
    {
        return read(out, sizeof(Nui32), count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(NIIf * out, NCount count) const
    {
        return read(out, sizeof(NIIf), count);
    }
    //-----------------------------------------------------------------------
    inline NCount Serializer::read(Ray & out) const
    {
        return read(&out.mOrigin) + read(&out.mDirection);
    }
    //-----------------------------------------------------------------------
    inline NCount Serializer::read(Sphere & out) const
    {
        return read(&out.mCenter) + read(&out.mRadius);
    }
    //-----------------------------------------------------------------------
    inline NCount Serializer::read(Quaternion * out, NCount count) const
    {
        return read(&out->w, sizeof(NIIf), 4 * count);
    }
    //-----------------------------------------------------------------------
    inline NCount Serializer::read(AABox & out) const
    {
        return read(&out.mMinimum) + read(&out.mMaximum);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Vector2<t> * out, NCount count) const
    {
        return read(&out->x, sizeof(t), 2 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Vector3<t> * out, NCount count) const
    {
        return read(&out->x, sizeof(t), 3 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Vector4<t> * out, NCount count) const
    {
        return read(&out->x, sizeof(t), 4 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Matrix3<t> * out, NCount count) const
    {
        return read(out->_m, sizeof(t), 9 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Matrix4<t> * out, NCount count) const
    {
        return read(out->_m, sizeof(t), 16 * count);
    }
    //-----------------------------------------------------------------------
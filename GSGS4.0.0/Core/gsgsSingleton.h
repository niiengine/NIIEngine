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
#ifndef SINGLETON_H
#define SINGLETON_H

namespace gsgs
{
    template <class T> class Singleton
    {
        static T* gInstance;
    public:
        /**
         * Static method to access the only pointer of this instance.
         * \return a pointer to the only instance of this 
         */
        static T* Get()
        {
            if (!gInstance)
                gInstance = new T();
            return gInstance;
        }

        /**
         * Release resources.
         */
        static void Free()
        {
            if (gInstance)
            {
                delete gInstance;
                gInstance = 0;
            }
        }

    protected:
        Singleton() {}
        virtual ~Singleton(){}
    };
    template <class T> T* Singleton<T>::gInstance = 0;
}
#endif // SINGLETON_H

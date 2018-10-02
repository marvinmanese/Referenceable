/**
 * Referenceable
 * 2017 Marvin Manese
 *
 * CC0 1.0 Universal (CC0 1.0) Public Domain Dedication
 * https://creativecommons.org/publicdomain/zero/1.0/
 **/
#ifndef GUARD_mmfutils_referenceable_H
#define GUARD_mmfutils_referenceable_H

// standard headers
#include <utility>

#ifndef MMF_UTIL_NO_EXCEPT
#if __cplusplus >= 201103L
#define MMF_UTIL_NO_EXCEPT noexcept
#else
#define MMF_UTIL_NO_EXCEPT
#endif // __cplusplus >= 201103L
#endif // MMF_UTIL_NO_EXCEPT

namespace mmfutils
{
    template<class T> class referenceable;
namespace detail
{
    template<class T>
    class reference_counter
    {
    private:
        typedef mmfutils::referenceable<T>* referenceable_ptr_t;

    public:
        typedef T                           value_type;
        typedef value_type&                 reference_type;
        typedef const value_type&           const_reference_type;
        typedef value_type*                 pointer_type;
        typedef const value_type*           const_pointer_type;
        typedef unsigned long               count_type;

#if __cplusplus >= 201103L
        reference_counter() = delete;
        reference_counter(const reference_counter&) = delete;
        reference_counter& operator=(const reference_counter&) = delete;
        reference_counter(reference_counter&&) = delete;
        reference_counter& operator=(reference_counter&&) = delete;
#else
    private:
        reference_counter();
        reference_counter(const reference_counter&);
        reference_counter& operator=(const reference_counter&);

    public:
#endif // __cplusplus >= 201103L
        ~reference_counter() { remove_reference(); }
        reference_counter(referenceable_ptr_t value_ptr) MMF_UTIL_NO_EXCEPT : m_valueptr(value_ptr), m_usecount(0) {}

        void kill_original();
        void add_reference();
        void remove_reference();
        void set_value_ptr(referenceable_ptr_t value_ptr);

        referenceable_ptr_t get_value_ptr() MMF_UTIL_NO_EXCEPT { return m_valueptr; }
        count_type get_use_count() const MMF_UTIL_NO_EXCEPT { return m_usecount; }

    private:
        void try_destroy();

        referenceable_ptr_t m_valueptr;
        count_type          m_usecount;
    };

    template<class T>
    void reference_counter<T>::kill_original()
    {
        m_valueptr = 0;
        try_destroy();
    }

    template<class T>
    void reference_counter<T>::add_reference()
    {
        ++m_usecount;
    }

    template<class T>
    void reference_counter<T>::remove_reference()
    {
        --m_usecount;
        if(!m_valueptr)
            try_destroy();
    }

    template<class T>
    void reference_counter<T>::set_value_ptr(referenceable_ptr_t value_ptr)
    {
        m_valueptr = value_ptr;
    }

    template<class T>
    void reference_counter<T>::try_destroy()
    {
        if(!m_usecount)
            delete this;
    }
}
    template<class T> class reference;
    template<class T>
    class referenceable
    {
    public:
        typedef typename detail::reference_counter<T>::value_type               value_type;
        typedef typename detail::reference_counter<T>::reference_type           reference_type;
        typedef typename detail::reference_counter<T>::const_reference_type     const_reference_type;
        typedef typename detail::reference_counter<T>::pointer_type             pointer_type;
        typedef typename detail::reference_counter<T>::const_pointer_type       const_pointer_type;

#if __cplusplus >= 201103L
        template<class... Args> referenceable(Args&&... args);
#else
        referenceable(const T& t);
#endif // __cplusplus >= 201103L
        referenceable(const referenceable& rhs);
#if __cplusplus >= 201103L
        referenceable& operator=(const referenceable& rhs) = delete;
        referenceable(referenceable&& rhs) noexcept(std::is_move_constructible<T>());
        referenceable& operator=(referenceable&& rhs) = delete;
#else
    private:
        referenceable& operator=(const referenceable& rhs);

    public:
#endif // __cplusplus >= 201103L
        referenceable& operator=(const T& t);
#if __cplusplus >= 201103L
        referenceable& operator=(T&& t);
#endif // __cplusplus >= 201103L
        ~referenceable();

        const_reference_type get() const MMF_UTIL_NO_EXCEPT { return m_value; }
        pointer_type get_ptr() MMF_UTIL_NO_EXCEPT { return &m_value; }
        typename detail::reference_counter<T>::count_type get_use_count() const MMF_UTIL_NO_EXCEPT { return m_referencecount ? m_referencecount->get_use_count() : 0; }

    private:
        template<class U> friend class reference;

        value_type                              m_value;
        mutable detail::reference_counter<T>*   m_referencecount;
    };

#if __cplusplus >= 201103L
    template<class T>
    template<class... Args>
    referenceable<T>::referenceable(Args&&... args) :
        m_value(std::forward<Args>(args)...),
        m_referencecount(new detail::reference_counter<T>(this))
    {
    }
#else
    template<class T>
    referenceable<T>::referenceable(const T& t) :
        m_value(t),
        m_referencecount(new detail::reference_counter<T>(this))
    {
    }
#endif // __cplusplus >= 201103L
    template<class T>
    referenceable<T>::referenceable(const referenceable& rhs) :
        m_value(rhs.m_value),
        m_referencecount(rhs.m_referencecount)
    {
        if(m_referencecount) {
            m_referencecount->set_value_ptr(this);
            rhs.m_referencecount = 0;
        }
    }
#if __cplusplus >= 201103L
    template<class T>
    referenceable<T>::referenceable(referenceable&& rhs) noexcept(std::is_move_constructible<T>()) :
        m_value(std::move(rhs.m_value)),
        m_referencecount(std::move(rhs.m_referencecount))
    {
        if(m_referencecount) {
            m_referencecount->set_value_ptr(this);
            rhs.m_referencecount = 0;
        }
    }
#endif // __cplusplus >= 201103L
    template<class T>
    referenceable<T>& referenceable<T>::operator=(const T& t)
    {
        m_value = t;
        return *this;
    }
#if __cplusplus >= 201103L
    template<class T>
    referenceable<T>& referenceable<T>::operator=(T&& t)
    {
        m_value = std::move(t);
        return *this;
    }
#endif // __cplusplus >= 201103L
    template<class T>
    referenceable<T>::~referenceable()
    {
        if(m_referencecount)
            m_referencecount->kill_original();
    }

    template<class T>
    class reference
    {
    public:
        typedef typename detail::reference_counter<T>::value_type   value_type;
        typedef typename detail::reference_counter<T>::pointer_type pointer_type;

        reference() : m_referencecount(0) {}
        reference(const reference<T>& rhs);
        reference& operator=(const reference<T>& rhs);
#if __cplusplus >= 201103L
        reference(reference<T>&& rhs);
        reference& operator=(reference<T>&& rhs);
#endif // __cplusplus >= 201103L
        ~reference();
        reference(const referenceable<T>& rhs);
        reference& operator=(const referenceable<T>& rhs);

        pointer_type get() MMF_UTIL_NO_EXCEPT { return m_referencecount ? (m_referencecount->get_value_ptr() ? m_referencecount->get_value_ptr()->get_ptr() : 0) : 0; }
        bool valid() const { return !expired(); }
        bool expired() const { return m_referencecount ? !m_referencecount->get_value_ptr() : true; }
        typename detail::reference_counter<T>::count_type get_use_count() const { return m_referencecount ? m_referencecount->get_use_count() : 0; }
        void release();

        void swap(reference<T>& other) noexcept;

    private:
        void try_add_ref();
        void try_rem_ref();

        detail::reference_counter<T>* m_referencecount;
    };

    template<class T>
    reference<T>::reference(const reference<T>& rhs) :
        m_referencecount(rhs.m_referencecount)
    {
        try_add_ref();
    }

    template<class T>
    reference<T>& reference<T>::operator=(const reference<T>& rhs)
    {
        reference<T> temp(rhs);
        swap(temp);

        return *this;
    }
#if __cplusplus >= 201103L
    template<class T>
    reference<T>::reference(reference<T>&& rhs) :
        m_referencecount(std::move(rhs.m_referencecount))
    {
        try_add_ref();
    }

    template<class T>
    reference<T>& reference<T>::operator=(reference<T>&& rhs)
    {
//        reference<T> temp(std::move(rhs));
        swap(rhs);

        return *this;
    }
#endif // __cplusplus >= 201103L
    template<class T>
    reference<T>::~reference()
    {
        try_rem_ref();
    }

    template<class T>
    reference<T>::reference(const referenceable<T>& rhs) :
        m_referencecount(rhs.m_referencecount)
    {
        try_add_ref();
    }

    template<class T>
    reference<T>& reference<T>::operator=(const referenceable<T>& rhs)
    {
        reference<T> temp(rhs);
        swap(temp);

        return *this;
    }

    template<class T>
    void reference<T>::release()
    {
        try_rem_ref();
        m_referencecount = 0;
    }

    template<class T>
    void reference<T>::swap(reference<T>& other) noexcept
    {
        std::swap(m_referencecount, other.m_referencecount);
    }

    template<class T>
    void reference<T>::try_add_ref()
    {
        if(m_referencecount)
            m_referencecount->add_reference();
    }

    template<class T>
    void reference<T>::try_rem_ref()
    {
        if(m_referencecount)
            m_referencecount->remove_reference();
    }
}

#endif // GUARD_mmfutils_referenceable_H

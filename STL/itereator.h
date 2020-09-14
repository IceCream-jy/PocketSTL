#ifndef _POCKET_ITERATOR_H_
#define _POCKET_ITERATOR_H_

#include <cstddef>

namespace pocket_stl{
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag {};
    struct bidrectional_iterator_tag {};
    struct random_access_iterator_tag {};

    //避免遗漏，迭代器可继承自 pocket_stl:iterator
    template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
    struct iterator{
        typedef Category        iterator_category;
        typedef T               value_type;
        typedef Distance        difference_type;
        typedef Pointer         pointer;
        typedef Reference       reference;
    };

    //泛化版 traits
    template <class Iterator>
    struct iterator_traits{
        typedef typename Iterator::iterator_category    iterator_category;
        typedef typename Iterator::value_type           value_type;
        typedef typename Iterator::difference_type      difference_type;
        typedef typename Iterator::pointer              pointer;
        typedef typename Iterator::reference            reference;
    };

    //原生指针的偏特化版 traits
    template <class T>
    struct iterator_traits<T*>{
        typedef random_access_iterator_tag              iterator_category;
        typedef T                                       value_type;
        typedef ptrdiff_t                               difference_type;
        typedef T*                                      pointer;
        typedef T&                                      reference;
    };

    //pointer-to-const 的偏特化版 traits
    template <class T>
    struct iterator_traits<const T*>{
        typedef random_access_iterator_tag              iterator_category;
        typedef T                                       value_type;
        typedef ptrdiff_t                               difference_type;
        typedef const T*                                pointer;
        typedef const T&                                reference;
    };
}

#endif
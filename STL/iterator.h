#ifndef _POCKET_ITERATOR_H_
#define _POCKET_ITERATOR_H_

#include <cstddef>

namespace pocket_stl{

    // 5 种迭代器类型
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidrectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidrectional_iterator_tag {};

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

    //方便决定某迭代器的类型（category）, 可直接作为函数参数传入，方便重载函数的选择
    template <class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&){
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }

    //方便决定迭代器的 value_type
    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&){
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    //方便决定迭代器的 difference_type
    template <class Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&){
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }



    /*
     * 补充 reverse iterator
     * inserter iterator
     * 
     *    
    */
}

#endif
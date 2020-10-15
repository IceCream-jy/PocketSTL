#ifndef _POCKET_VECTOR_H_
#define _POCKET_VECTOR_H_

/*
** vector
** vector<bool> 需特化， 在 std 中不是容器
*/

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include "allocator.h"
#include "uninitialized.h"

namespace pocket_stl{
    template <class T, class Alloc = allocator<T>>
    class vector{
    public:
        typedef Alloc   allocator_type;
        typedef Alloc   data_allocator;

        using value_type        = typename allocator_type::value_type;
        using size_type         = typename allocator_type::size_type;
        using difference_type   = typename allocator_type::difference_type;
        using pointer           = typename allocator_type::pointer;
        using const_pointer     = typename allocator_type::const_pointer;
        using reference         = typename allocator_type::reference;
        using const_reference   = typename allocator_type::const_reference;

        using iterator          = value_type*;
        using const_iterator    = const value_type*;
        using reverse_iterator          = std::reverse_iterator<iterator>;          // 暂时使用 std::reverse_iterator
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

    private:
        iterator __start;
        iterator __end;
        iterator __end_of_storage;

    public:*
        /***************ctor 、 copy_ctor 、 move_ctor 、 dtor 、 operator=*****************/
        // **** default ctor
        vector() noexcept{
            try{
                __start = data_allocator::allocate(16);
                __end = __start;
                __end_of_storage = __start + 16;
            }
            catch(...){
                __start = nullptr;
                __end = nullptr;
                __end_of_storage = nullptr;
            }
        }

        // **** fill ctor
        explicit vector(size_type n){
            allocate_and_fill(n, value_type());
        }

        explicit vector(size_type n, const value_type& val){
            allocate_and_fill(n, val);
        }

        // **** range ctor
        // 判断 InputIterator 是否为整型
        // 整型则调用 uninitialized_fill
        // 非整形则调用 uninitialized_copy
        template <class InputIterator>
        vector(InputIterator first, InputIterator last){
            range_initialize(first, last);
        }
        
        // **** copy ctor
        vector(const vector& x){
            allocate_and_copy(x.begin(), x.end());
        }

        // **** move ctor
        vector(vector&& x) noexcept
            : __start(x.__start), __end(x.__end), __end_of_storage(x.__end_of_storage){
            x.__start = nullptr;
            x.__end = nullptr;
            x.__end_of_storage = nullptr;
        }

        // **** initializer list
        vector(std::initializer_list<value_type> il){
            allocate_and_copy(il.begin(), il.end());
        }

        // **** dtor
        ~vector(){
            destroy_and_deallocate_all();
        }

        // **** operator=
        // copy
        vector& operator=(const vector& x);
        // move
        vector& operator=(vector&& x);
        //initializer_list
        vector& operator=(std::initializer_list<value_type> il);

    public:
        /********************** Iterator 函数 *****************************/
        iterator                begin() noexcept { return __start; }
        const_iterator          begin() const noexcept { return __start; }
        iterator                end() noexcept { return __end; }
        const_iterator          end() const noexcept { return __end; }
        reverse_iterator        rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin() const noexcept { return reverse_iterator(end()); }
        reverse_iterator        rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator  rend() const noexcept { return reverse_iterator(begin()); }
        const_iterator          cbegin() const noexcept { return begin(); }
        const_iterator          cend() const noexcept { return end(); }
        const_reverse_iterator  crbegin() { return reverse_iterator(end()); }
        const_reverse_iterator  crend() { return reverse_iterator(begin()); }
        /********************** Capacity 函数 *****************************/
        size_type size() const noexcept { return static_cast<size_type>(__end - __start); }
        size_type max_size() const noexcept { return size_type(-1) / sizeof(value_type); }
        void resize(size_type n) { return resize(n, value_type()); }
        void resize(size_type n, const value_type& val);

        size_type capacity() const noexcept{ return static_cast<size_type>(__end_of_storage - __start); }
        

        /********************** Modifiers 函数 ****************************/
        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);
        void swap(vector& x);

    private:
        /***********************内存分配构造工具*****************************/
        void allocate_and_fill(size_type n, const value_type& val);
        template <class InputIterator>
        void allocate_and_copy(InputIterator first, InputIterator last);
        template <class InputIterator>
        void range_initialize(InputIterator first, InputIterator last);
        template <class InputIterator>
        void range_initialize_aux(InputIterator first, InputIterator last, std::true_type);
        template <class Integer>
        void range_initialize_aux(Integer n, Integer val, std::false_type);
        void destroy_and_deallocate_all();
    };



    /*-------------------------------部分函数定义------------------------------------*/
    //--------------------- operator=
    //copy
    template <class T, class Alloc>
    vector<T, Alloc>& 
    vector<T, Alloc>::operator=(const vector<T, Alloc>& x){
        if(this != &x){
            const size_type len = x.size();
            if(len > capacity()){
                vector tmp(x.begin(), x.end());
                swap(tmp);
            }
            else if(len >= size()){
                std::copy(x.begin(), x.begin() + size(), __start);
                uninitialized_copy(x.begin() + size(), x.end(), __end);
                __end = __start + len;
            }
            else if(len < size()){
                std::copy(x.begin(), x.end(), __start);
                data_allocator::destroy(__start + len, __end);
                __end = __start + len;
            }
        }
    }

    //move
    template <class T, class Alloc>
    vector<T, Alloc>& 
    vector<T, Alloc>::operator=(vector<T, Alloc>&& x){
        destroy_and_deallocate_all();
        __start = x.__start;
        __end = x.__end;
        __end_of_storage = x.__end_of_storage;
        x.__start = x.__end = x.__end_of_storage = nullptr;
        return *this;
    }

    //initializer_list
    template <class T, class Alloc>
    vector<T, Alloc>& 
    vector<T, Alloc>::operator=(std::initializer_list<value_type> il){
        destroy_and_deallocate_all();
        allocate_and_copy(il.begin(), il.end());
    }

    //--------------------- capacity 函数
    template <class T, class Alloc>
    void 
    vector<T, Alloc>::resize(size_type n, const value_type& val){
        if(n < size()){
            erase(__start + n, __end);
            __end = __start + n;
        }
        else if(n > capacity()){
            
        }
    }

    //--------------------- Modifiers 函数
    template <class T, class Alloc>
    void 
    vector<T, Alloc>::swap(vector<T, Alloc>& x){
        if(this != &x){             // 暂时调用 std::swap
            std::swap(__start, x.__start);
            std::swap(__end, x.__end);
            std::swap(__end_of_storage, x.__end_of_storage);
        }
    }
    
    // -------------------- 内存分配工具
    template <class T, class Alloc>
    void 
    vector<T, Alloc>::allocate_and_fill(size_type n, const value_type& val) {
        __start = allocator_type::allocate(n);
        __end = __start + n;
        uninitialized_fill_n(__start, n, val);
        __end_of_storage = __end;
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void 
    vector<T, Alloc>::allocate_and_copy(InputIterator first, InputIterator last){
        __start = data_allocator::allocate(last - first);
        __end = uninitialized_copy(first, last, __start);
        __end_of_storage = __end;
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void 
    vector<T, Alloc>::range_initialize(InputIterator first, InputIterator last){
        range_initialize_aux(first, last, std::is_integral<InputIterator>::type());
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void 
    vector<T, Alloc>::range_initialize_aux(InputIterator first, InputIterator last, std::true_type){
        allocate_and_copy(first, last);
    }

    template <class T, class Alloc>
    template <class Integer>
    void 
    vector<T, Alloc>::range_initialize_aux(Integer n, Integer val, std::false_type){
        __start = allocate(static_cast<size_type>(n));
        uninitialized_fill_n(__start, n, val);
        __end = __start + n;
        __end_of_storage = __end;
    }

    template <class T, class Alloc>
    void 
    vector<T, Alloc>::destroy_and_deallocate_all (){
        data_allocator::destroy(__start, __end);
        data_allocator::deallocate(__start, __end_of_storage - __start);
    }
    
} // namespace

#endif
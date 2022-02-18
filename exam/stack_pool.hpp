#include <iostream>
#include <vector>
#include <iterator>

template <typename T, typename N, typename P>
class _iterator {

    private: 
        using stack_type = N;  
        using stack_pool = P;  
        stack_type head;      
        stack_pool * pool;    

    public:
        using value_type = T;             
        using reference = value_type&;      
        using pointer = value_type*;        
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        _iterator(stack_pool& pool, stack_type x):
            head{x}, pool{&pool} {} 

        _iterator(const _iterator& i) = default;

        _iterator& operator=(const _iterator& a) noexcept {
            head = a.head;
            return *this;
        }

        reference operator*() const{
            return pool->value(head);
        }

        stack_type operator&() const noexcept{
            return head;
        }

        _iterator& operator++(){  
            head = pool->next(head);
            return *this;
        }

        _iterator operator++(int){  
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        stack_type ptr_to_stack() const noexcept {return head;}

        friend bool operator==(const _iterator& a, const _iterator& b) noexcept {
            return a.head == b.head;
        }
        friend bool operator!=(const _iterator& a, const _iterator& b) noexcept {
            return !(a == b);
        }      
};

template<typename T, typename N = std::size_t>
class stack_pool{

    private:
        struct node_t{
            T value;
            N next;
        };
        
        std::vector<node_t> pool;
        using stack_type = N;
        using value_type = T;
        using size_type = typename std::vector<node_t>::size_type;

        stack_type free_nodes;

        node_t& node(stack_type x) { return pool[x - 1]; }
        const node_t& node(stack_type x) const { return pool[x - 1]; }

        template<typename X>
        stack_type _push(X&& val, stack_type head) noexcept{
            if(free_nodes == end()){
                node_t n = node_t();
                n.next = end();
                pool.push_back(std::move(n));
                free_nodes = pool.size();
            }
            stack_type new_head = free_nodes;
            free_nodes = next(free_nodes);
            next(new_head) = head;
            value(new_head) = std::forward<X>(val);

            return new_head;
        }

    public:

        /**
         * @brief Construct a new stack pool object having initial capacity 0
         * 
         */
        stack_pool() noexcept: free_nodes{end()} {}; 
        /**
         * @brief Construct a new stack pool object having a given initial capacity
         * 
         * @param n The initial capacity of the pool
         */
        explicit stack_pool(size_type n): free_nodes{end()} {reserve(n);} 
        
        stack_pool& operator=(const stack_pool&) = default;
        stack_pool& operator=(stack_pool&&) = default;

        using iterator = _iterator<value_type, stack_type, stack_pool>;
        using const_iterator = _iterator<const value_type, stack_type, const stack_pool>;

        iterator begin(stack_type x) {return iterator{*this, x};}
        iterator end(stack_type) noexcept {return iterator{*this, end()};}

        const_iterator begin(stack_type x) const {return const_iterator{*this, x};}
        const_iterator end(stack_type) const noexcept {return const_iterator{*this, end()};}

        const_iterator cbegin(stack_type x) const {return const_iterator{*this, x};}
        const_iterator cend(stack_type) const noexcept {return const_iterator{*this, end()};}

        /**
         * @brief "Allocate" a new stack in this pool. Returns the head of the new stack
         * 
         * @return stack_type 
         */
        stack_type new_stack() noexcept {return end();}
        /**
         * @brief Allocate some more space on the pool.
         *  
         * This method can improve the performance when multiple elements are added at
         * the same time to the pool
         * 
         * @param n the advised new size of the stack
         */
        void reserve(size_type n) {pool.reserve(n);}
        /**
         * @brief return the capacity of the pool 
         * 
         * @return size_type 
         */
        size_type capacity() const noexcept {return pool.capacity();}
        /**
         * @brief Check whether the given stack is empty
         * 
         * @param x 
         * @return true 
         * @return false 
         */
        bool empty(stack_type x) const noexcept {return x == end();}
        /**
         * @brief Return end of the pool
         * 
         * @return stack_type 
         */
        stack_type end() const noexcept {return stack_type(0);}
        /**
         * @brief Return the front value in the stack.
         * 
         * @param x 
         * @return T& 
         */
        T& value(stack_type x){return node(x).value;}
        /**
         * @brief Return the front value in the stack
         * 
         * @param x 
         * @return const T& 
         */
        const T& value(stack_type x) const {return node(x).value;}
        /**
         * @brief Return the next node in the stack
         * 
         * @param x 
         * @return stack_type& 
         */
        stack_type& next(stack_type x) {return node(x).next;}
        /**
         * @brief Return the next node in the stack
         * 
         * @param x 
         * @return const stack_type& 
         */
        const stack_type& next(stack_type x) const {return node(x).next;}
        /**
         * @brief Push an element to the front of the stack. Returns the new head of the stack.
         * 
         * @param val value to be pushed
         * @param head head of the stack
         * @return stack_type 
         */
        stack_type push(const T& val, stack_type head) noexcept {return _push(val, head);}
        /**
         * @brief Push an element to the front of the stack. Returns the new head of the stack.
         * 
         * @param val value to be pushed
         * @param head head of the stack
         * @return stack_type 
         */
        stack_type push(T&& val, stack_type head) noexcept {return _push(std::forward<T>(val), head);}
        /**
         * @brief Pop the head of the stack
         * 
         * @param head 
         * @return stack_type 
         */
        stack_type pop(stack_type head){
            stack_type new_stack_head = next(head);
            next(head) = free_nodes;
            free_nodes = head;
            return new_stack_head;
        }
        /**
         * @brief Empty the stack
         * 
         * @param head 
         * @return stack_type 
         */
        stack_type free_stack(stack_type head) noexcept {
            if(empty(head)) return head;
            iterator current_node = begin(head);
            iterator next_node = current_node;
            ++next_node;
            while(next_node != end(head)) current_node = next_node++;

            next(current_node.ptr_to_stack()) = free_nodes;
            free_nodes = head;
            return end();
        }

};
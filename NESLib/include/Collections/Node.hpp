
template<class T>
    class DoubleLinkedList;

template<typename T>
    class Node
    {
        friend class DoubleLinkedList<T>;

    private:
        T _val;

    protected:
        Node *_previous;
        Node *_next;

    public:

        Node(T value);

        Node(T &value);

        Node(Node &&);

        Node(Node &);

        ~Node();

        void operator>>(Node &next);

        void operator<<(Node &previous);

        T &Value() const;

        T &operator*() const
        {
            return Value();
        }

        Node<T> *Next() const
        {
            return _next;
        }

        Node<T> *Previous() const
        {
            return _previous;
        }
    };


template<typename T>
    inline Node<T>::Node(T value) : _val{value}, _next{nullptr}, _previous{nullptr}
    {
    }

template<typename T>
    inline Node<T>::Node(T &value)
    {
        _val = value;
    }

template<typename T>
    inline Node<T>::Node(Node &&n) : _val{n._val}
    {
        n.~Node();
    }

template<typename T>
    inline Node<T>::Node(Node &n) : _val{n._val}
    {
    }

template<typename T>
    inline Node<T>::~Node()
    {
        if (_val != nullptr)
        {
            delete _val;
        }
        _next = nullptr;
        _previous = nullptr;
    }



template<typename T>
    inline void Node<T>::operator>>(Node &next)
    {
        _next = &next;
        next._previous = this;
    }

template<typename T>
    inline void Node<T>::operator<<(Node &previous)
    {
        _previous = &previous;
        previous._next = this;
    }

template<typename T>
    inline T &Node<T>::Value() const
    {
        return _val;
    }

template<typename T>
    class NodeIterator
    {
    private:
        Node<T> *_currentNode;

    public:

        NodeIterator &operator++(int)
        {
            _currentNode = _currentNode->Next();
            return *this;
        }

        NodeIterator &operator--(int)
        {
            _currentNode = _currentNode->Previous();
            return *this;
        }

        NodeIterator &operator+(int offset)
        {
            for (int i = 0; i < offset; i++)
                _currentNode = _currentNode->Next();
            return *this;
        }

        NodeIterator &operator-(int offset)
        {
            for (int i = 0; i < offset; i++)
                _currentNode = _currentNode->Previous();
            return *this;
        }

        T &operator*()
        {
            return *_currentNode->Value();
        }

        Node<T> &operator()()
        {
            return *_currentNode;
        }

        NodeIterator(Node<T> *reference) : _currentNode{reference}
        {
        }

        inline bool operator==(NodeIterator &itr)
        {
            return _currentNode == &itr();
        }

        inline bool operator!=(NodeIterator &itr)
        {
            return _currentNode != &itr();
        }
    };

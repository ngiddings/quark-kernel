#ifndef MAP_H
#define MAP_H

template<class Key, class Value>
class Map
{
public:

    Map();

    bool contains(const Key& key) const
    {
        if(m_tree == nullptr)
            return false;
        else if(m_tree->search(key) == nullptr)
            return false;
        return true;
    }

    Value& get(const Key& key)
    {
        if(m_tree = nullptr)
            return (Value&) *nullptr;
        Node* node = m_tree->search(key);
        if(node == nullptr)
            return (Value&) *nullptr;
        else
            return node->m_value;
    }

    void insert(const Value& value);

    void remove(const Key& key);

    unsigned int size() const;

private:

    class Node
    {
    public:

        enum class Color
        {
            Black,
            Red
        }

        Key m_key;

        Value& m_value;

        Node *left, *right, *parent;

        Color color;

        Node();

        Node* uncle()
        {
            if(parent != nullptr && parent->parent != nullptr)
            {
                if(parent == parent->parent->left)
                    return parent->parent->right;
                return parent->parent->left;
            }
            return nullptr;
        }

        Node* search(const Key& key)
        {
            if(key == m_key)
                return this;
            if(left != nullptr)
            {
                Node* lsearch = left->search(key);
                if(lsearch != nullptr)
                    return lsearch;
            }
            if(right != nullptr)
            {
                Node* rsearch = right->search(key);
                if(rsearch != nullptr)
                    return rsearch;
            }
            return nullptr;
        }

    };

    Node *m_tree;

};

#endif
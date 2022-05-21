#ifndef ID_BIMAP_H
#define ID_BIMAP_H

#include <iostream>
#include <map>
#include <type_traits>
#include <optional>

template<class T, class K = long long int>
class id_bimap
{
    private:

        std::map<K, std::optional<T>> mp;
        K key;

    public:

        typedef T mapped_type;
        typedef K key_type;

        typedef typename std::map<K,std::optional<T>>::const_iterator iterator;
        typedef typename std::map<K,std::optional<T>>::iterator nciterator;

        explicit id_bimap()
        {
            static_assert(!std::is_same<K, T>::value, "Key and value cannot be the same type.");
            static_assert(std::is_integral<K>::value, "Key must be an integral type.");
            key = static_cast<K>(mp.size());
        }

        id_bimap(std::initializer_list<T> list)
        {
            static_assert(!std::is_same<K, T>::value, "Key and value cannot be the same type.");
            K tempKey = static_cast<K>(mp.size());
            std::map<K, std::optional<T>> temp;
            for(auto& v : list)
                temp[tempKey++] = std::optional<T>(v);
            mp = std::move(temp);
            key = tempKey; 
        }

        id_bimap(const id_bimap<T,K>& other)
        {
            static_assert(std::is_trivially_copyable<T>::value, "Value type is not-copyable.");
            K tempKey = other.key;
            std::map<K, std::optional<T>> temp = other.mp;

            key = tempKey;
            mp = std::move(temp);
        }

        id_bimap(id_bimap<T,K>&& other)
        {
            key = other.key;
            mp = std::move(other.mp);
        }

        id_bimap<T, K>& operator=(const id_bimap<T,K>& other)
        {
            if(this != &other)
            {
                static_assert(std::is_trivially_copyable<T>::value, "Value type is not-copyable.");
                K tempKey = other.key;
                std::map<K, std::optional<T>> temp = other.mp;

                key = tempKey;
                mp = std::move(temp);
            }

            return *this;
        }

        id_bimap<T, K>& operator=(id_bimap<T,K>&& other)
        {
            key = other.key;
            mp = std::move(other.mp);

            return *this;
        }

        ~id_bimap<T,K>() {}

        std::pair<iterator, bool> insert(const T& value)
        {
            iterator itr = find(value);
            if(itr != mp.cend())
            {
                std::cout << "The value is already included in the map." << std::endl;
                std::pair<iterator, bool> pair = {itr, false};
                return pair;
            }

            itr = mp.begin();
            while(itr != mp.end() && itr->second) ++itr;

            if(itr == mp.end())
            {
                mp[key++] = std::optional<T>(value);
                itr = mp.find(key-1);
            } 
            else mp[itr->first] = std::optional<T>(value);
            
            std::pair<iterator, bool> pair = {itr, true};
            return pair;
        }

        template <class... Args>
        std::pair<iterator, bool> emplace(Args&&... arg)
        {
            std::optional<T> opt;
            if(is_contiguous())
                return mp.emplace(key++, opt.emplace(arg...));
            else
                return mp.emplace(next_index(), opt.emplace(arg...));
        }

        std::pair<iterator, bool> emplace(T&& v)
        {
            if(is_contiguous())
                return mp.emplace(key++, std::optional<T>(std::move(v)));
            else
                return mp.emplace(next_index(), std::optional<T>(std::move(v)));
        }

        const T& operator[](const K& k) const
        {
            if(mp.find(k) == mp.end() || !mp.find(k)->second) throw std::out_of_range("Key cannot be found.");
            return *mp.at(k);
        }

        const K& operator[](const T& v) const
        {
            for(auto& itr : mp)
                if(itr.second && *itr.second == v)
                    return itr.first;
            throw std::domain_error("Value cannot be found.");
        }

        int size() const
        {
            int size = 0;
            for(auto& itr : mp)
            {
                if(itr.second) size++;
            }
            return size;
        }

        int capacity() const
        {
            return mp.size();
        }

        bool empty() const
        {
            return size() == 0;
        }
        
        void clear()
        {
            for(auto& itr : mp) itr.second.reset();
            key = static_cast<K>(size());
        }

        void erase(const K& k)
        {
            nciterator itr = mp.find(k);
            if(itr != mp.end())
            {
                if(!itr->second) throw std::out_of_range("Key cannot be found.");
                else itr->second.reset();
            } 
        }

        void erase(const T& v)
        {
            for(auto itr = mp.begin(); itr != mp.end(); ++itr)
                if(itr->second && *itr->second == v)
                {
                    itr->second.reset();
                    return;
                }
        }

        iterator find(const T& v) const
        {
            for(iterator itr = mp.begin(); itr != mp.end(); ++itr)
                if(itr->second && *itr->second == v)
                    return itr;
            return mp.cend();
        }

        iterator begin() const
        {
            return mp.begin();
        }

        iterator end() const
        {
            return mp.end();
        }

        const K next_index() const
        {
            for(iterator itr = mp.begin(); itr != mp.end(); ++itr)
                if(!itr->second)
                    return itr->first;
            return key;
        }

        bool is_contiguous() const
        {
            for(iterator itr = mp.begin(); itr != mp.end(); ++itr)
                if(!itr->second)
                    return false;
            return true;
        }

        template<class UnaryPredicate>
        iterator find_if(UnaryPredicate p) const
        {
            for(auto& itr : mp)
                if(itr.second && p(*itr.second))
                    return find(*itr.second);
            return mp.cend();
        }

        template<class UnaryPredicate>
        void delete_all(UnaryPredicate p)
        {
            for(auto it = mp.cbegin(); it != mp.cend(); ++it)
                if(it->second && p(*it->second))
                    erase(*it->second);
        }

        void reserve(int new_cap)
        {
            if(new_cap > size())
            {
                if(new_cap > capacity())
                {
                    for(int i = capacity(); i < new_cap; ++i)
                        mp[key++] = std::optional<T>();
                }
                else if(new_cap < capacity())
                {
                    nciterator itr = mp.begin();
                    int c = 0;
                    K k = 0;
                    while(c < size())
                    {
                        if(itr->second)
                        {
                            c++;
                            k = find(*itr->second)->first;
                        }   
                        itr++;
                    }
                    mp.erase(mp.find(k+1), mp.end());
                }
            }
        }

        void print() const 
        {
            for(iterator itr = mp.begin(); itr != mp.end(); ++itr)
                {
                    std::cout << itr->first << " " << itr->second.value_or("empty") << std::endl;
                }
        }

        const K& getKey() const
        {
            return key;
        }
};

using string_id_bimap = id_bimap<std::string>;

template<class T>
using kchar_id_bimap = id_bimap<T, char>;

#endif
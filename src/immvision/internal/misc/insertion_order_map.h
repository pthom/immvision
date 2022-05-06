#pragma once

#include <unordered_map>
#include <vector>
#include <utility>
#include <algorithm>


namespace ImmVision
{
    template<typename Key, typename Value>
    class insertion_order_map
    {
    public:

        bool contains(const Key& k) const { return _map.find(k) != _map.end(); }

        bool empty() const { return _keys.empty(); }

        void clear() { _keys.clear(); _map.clear(); }

        void insert(const Key& k, const Value& v)
        {
            assert(!contains(k));
            _keys.push_back(k);
            _map[k] = v;
        }

        void insert(const Key& k, Value&& v)
        {
            assert(!contains(k));
            _keys.push_back(k);
            _map[k] = std::move(v);
        }


        Value& get(const Key& k)
        {
            assert(contains(k));
            return _map.at(k);
        }

        const Value& get(const Key& k) const
        {
            assert(contains(k));
            return _map.at(k);
        }

        void erase(const Key& k)
        {
            assert(contains(k));
            _map.erase(_map.find(k));
            _keys.erase(std::remove(_keys.begin(), _keys.end(), 5), _keys.end());
        }

        const std::vector<Key>& insertion_order_keys() const
        {
            return _keys;
        }

        const std::vector<std::pair<const Key&, const Value&>> items() const
        {
            std::vector<std::pair<const Key&, const Value&>> r;
            for (const auto& k : insertion_order_keys())
                r.push_back({k, get(k)});
            return r;
        }

        std::vector<std::pair<const Key&, Value&>> items()
        {
            std::vector<std::pair<const Key&, Value&>> r;
            for (const auto& k : insertion_order_keys())
                r.push_back({k, get(k)});
            return r;
        }

    private:
        std::unordered_map<Key, Value> _map;
        std::vector<Key> _keys;
    };



} // namespace ImmVision

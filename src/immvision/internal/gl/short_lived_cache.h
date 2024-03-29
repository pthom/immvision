#pragma once

#include <map>
#include <vector>
#include <cassert>

#include "immvision/internal/gl/short_lived_cache.h"

namespace ImmVision
{
    namespace internal
    {
        double TimerSeconds();

        template<typename Key, typename Value>
        class Cache
        {
        public:
            bool Contains(const Key& k) const
            {
                return mDict.find(k) != mDict.end();
            }

            Value& Get(const Key& k)
            {
                // If you encounter this assert during debugging, it is perhaps due to ShortLiveCache (below)
                // which periodically removes elements that were unused during a given period (5 seconds)
                // You can temporarily disable this feature by adding a return statement at the very beginning of
                // ShortLivedCache::ClearOldEntries()
                assert(mDict.find(k) != mDict.end());
                return mDict.at(k);
            }

            void AddKey(const Key& k)
            {
                assert(mDict.find(k) == mDict.end());
                mDict.insert({k, Value()});
            }

            void RemoveKey(const Key& k)
            {
                mDict.erase(k);
            }

            void Clear()
            {
                mDict.clear();
            }

            std::vector<Key> Keys() const
            {
                std::vector<Key> r;
                for (const auto& kv: mDict)
                    r.push_back(kv.first);
                return r;
            }
        private:
            std::map<Key, Value> mDict;
        };


        template<typename Value>
        class ShortLivedValue
        {
        public:
            ShortLivedValue()
            {
                mLastAccessTime = TimerSeconds();
            }
            Value& Get()
            {
                mLastAccessTime = TimerSeconds();
                return mValue;
            }
            const Value& Get() const
            {
                mLastAccessTime = TimerSeconds();
                return mValue;
            }
            double LastAccessTime() const
            {
                return mLastAccessTime;
            }
        private:
            mutable double mLastAccessTime = TimerSeconds();
            Value mValue;
        };


        template<typename Key, typename Value>
        class ShortLivedCache
        {
        public:
            ShortLivedCache(double timeToLive) : mTimeToLiveSeconds(timeToLive) {}
            bool Contains(const Key& k)
            {
                return mCache.Contains(k);
            }
            Value& Get(const Key& k)
            {
                auto& cached = mCache.Get(k);
                // double now = TimerSeconds();
                // printf("now=%.1f Get %p Last Access Time=%.1f Age=%.1f\n", now, k, cached.LastAccessTime(), now - cached.LastAccessTime());
                return cached.Get();
            }
            const Value& Get(const Key& k) const
            {
                return Get(k);
            }
            void AddKey(const Key& k)
            {
                mCache.AddKey(k);
            }
            void RemoveKey(const Key& k)
            {
                mCache.RemoveKey(k);
            }
            std::vector<Key> Keys() const
            {
                return mCache.Keys();
            }

            void ClearOldEntries()
            {
                return;
                double now = TimerSeconds();
                std::vector<Key> oldEntries;
                for (const auto& key: Keys())
                    if ((now - mCache.Get(key).LastAccessTime()) > mTimeToLiveSeconds)
                        oldEntries.push_back(key);

                for (auto& key: oldEntries)
                    mCache.RemoveKey(key);
            }

            void Clear()
            {
                mCache.Clear();
            }
        private:
            double mTimeToLiveSeconds = 1.;
            Cache< Key, ShortLivedValue<Value> > mCache;
        };
    } // namespace internal
} // namespace ImmVision

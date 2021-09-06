#ifndef RWGUARD_H
#define RWGUARD_H

#include <shared_mutex>
#include <mutex>
#include <map>
#include <condition_variable>
#include "types.h"


namespace binom {

typedef ui64 f_virtual_index;

struct RWSyncMap {
  class RWGuard;
private:
  class RWGuardAutoDelete;
  std::map<f_virtual_index, std::weak_ptr<RWGuardAutoDelete>> mtx_map;
  std::mutex general_mtx;

  //! RWGuard autodestroyer (Removes RWGuard-element from RWSyncMap)
  class RWGuardAutoDelete {
    RWSyncMap* map;
    std::shared_mutex mtx;
    f_virtual_index v_index;

    friend class RWGuard;
  public:
    RWGuardAutoDelete(RWSyncMap* map, f_virtual_index v_index)
      : map(map), mtx(), v_index(v_index) {}
    RWGuardAutoDelete(const RWGuardAutoDelete&) = delete;
    ~RWGuardAutoDelete() {
      std::scoped_lock lock(map->general_mtx);
      auto it = map->mtx_map.find(v_index);
      if(it != map->mtx_map.cend())
        if(auto shr_ptr = it->second.lock(); !shr_ptr)
          map->mtx_map.erase(it);
    }
  };

public:
  enum class LockType : ui8 {
    shared_lock,
    unique_lock,
    unlocked
  };

  //! Read/Write guard (shared mutex for BinOM nodes)
  class RWGuard {
    std::shared_ptr<RWGuardAutoDelete> shr_ptr;
    LockType lock_type = LockType::unlocked;
    ui64 lock_count = 0;

    RWGuard(f_virtual_index v_index, RWSyncMap* map) : shr_ptr(std::make_shared<RWGuardAutoDelete>(map, v_index)) {
      map->mtx_map.emplace(v_index, shr_ptr);
    }

    RWGuard(std::weak_ptr<RWGuardAutoDelete>& weak, f_virtual_index v_index, RWSyncMap* map) : shr_ptr(weak.lock()) {
      if(!shr_ptr) {
        new(this) RWGuard(v_index, map);
        weak = shr_ptr;
      }
    }

    void forceUnlock() {
      if(!shr_ptr) return;
      lock_count = 0;
      switch (lock_type) {
        case LockType::shared_lock:
          shr_ptr->mtx.unlock_shared();
          lock_type = LockType::unlocked;
        return;
        case LockType::unique_lock:
          shr_ptr->mtx.unlock();
          lock_type = LockType::unlocked;
        return;
        case LockType::unlocked: return;
      }
    }

    friend struct RWSyncMap;
  public:
    RWGuard() = default;
    RWGuard(RWGuard&& other)
      : shr_ptr(std::move(other.shr_ptr)), lock_type(other.lock_type), lock_count(other.lock_count) {
      other.shr_ptr.reset();
    }
    RWGuard(RWGuard&) = delete;
    ~RWGuard() {forceUnlock();}

    f_virtual_index getLockedIndex() {return shr_ptr->v_index;}

    void clear() {
      forceUnlock();
      shr_ptr.reset();
    }

    RWGuard& operator=(RWGuard&& other) {
      this->~RWGuard();
      return *new(this) RWGuard(std::move(other));
    }

    LockType getLockType() {return lock_type;}

    void lock() {
      if(!shr_ptr) return;
      if(lock_type == LockType::unique_lock) {
        ++lock_count;
        return;
      }
      forceUnlock();
      shr_ptr->mtx.lock();
      lock_type = LockType::unique_lock;
    }


    void lockShared() {
      if(!shr_ptr) return;
      if(lock_type != LockType::unlocked) {
        ++lock_count;
        return;
      }
      shr_ptr->mtx.lock_shared();
      lock_type = LockType::shared_lock;
    }

    void unlock() {
      if(!shr_ptr) return;
      if(lock_count) {
        --lock_count;
        return;
      }
      forceUnlock();
    }

//    bool tryLock() {
//      if(!shr_ptr) return;
//      unlock();
//      if(shr_ptr->mtx.try_lock()) {
//        lock_type = LockType::unique_lock;
//        return true;
//      }
//      return false;
//    }

//    bool tryLockShared() {
//      if(!shr_ptr) return;
//      unlock();
//      if(shr_ptr->mtx.try_lock_shared()) {
//        lock_type = LockType::shared_lock;
//        return true;
//      }
//      return false;
//    }
  };

  //! RAII RWGuard locker
  class ScopedRWGuard {
    RWSyncMap::RWGuard* rwg = nullptr;
    LockType lock_type = LockType::unlocked;
  public:
    ScopedRWGuard(RWGuard& rwg,
                  LockType lock_type = LockType::unlocked)
      : rwg(&rwg), lock_type(lock_type) {
      switch (lock_type) {
        case binom::RWSyncMap::LockType::shared_lock:
          rwg.lockShared();
        return;
        case binom::RWSyncMap::LockType::unique_lock:
          rwg.lock();
        return;
        case binom::RWSyncMap::LockType::unlocked:return;
      }
    }

    ScopedRWGuard(ScopedRWGuard& other,
                  LockType lock_type = LockType::unlocked)
        : rwg(other.rwg), lock_type(lock_type) {
        if(rwg)
            switch (lock_type) {
                case binom::RWSyncMap::LockType::shared_lock:
                    rwg->lockShared();
                return;
                case binom::RWSyncMap::LockType::unique_lock:
                    rwg->lock();
                return;
                case binom::RWSyncMap::LockType::unlocked:return;
            }
    }

    ScopedRWGuard(ScopedRWGuard&& other,
                  LockType lock_type = LockType::unlocked)
        : rwg(other.rwg), lock_type(lock_type) {
        if(rwg && lock_type != other.lock_type) {
            rwg->unlock();
            switch (lock_type) {
                case binom::RWSyncMap::LockType::shared_lock:
                    rwg->lockShared();
                return;
                case binom::RWSyncMap::LockType::unique_lock:
                    rwg->lock();
                return;
                case binom::RWSyncMap::LockType::unlocked:return;
            }
        }
        other.rwg = nullptr;
    }

    ~ScopedRWGuard() {
        if(rwg && lock_type != LockType::unlocked)
            rwg->unlock();
    }

    inline LockType getScopeLockType() {return lock_type;}
    inline LockType getGuardLockType() {if(rwg)return rwg->getLockType(); return LockType::unlocked;}
    inline f_virtual_index getLockIndex() {if(rwg)return rwg->getLockedIndex(); return 0xFFFFFFFFFFFFFFFF_ui64;}
  };

  RWSyncMap() = default;
  ~RWSyncMap() {}

  RWGuard get(f_virtual_index node_index) {
    std::scoped_lock lock(general_mtx);
    if(auto it = mtx_map.find(node_index); it != mtx_map.cend()) {
      RWGuard guard(it->second, node_index, this);
      return guard;
    } else {
      RWGuard guard(node_index, this);
      return guard;
    }
  }
};


}


#endif // RWGUARD_H

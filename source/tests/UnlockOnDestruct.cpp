#include <catch2/catch_test_macros.hpp>

#include <common/UnlockOnDestruct.hpp>

class DummyLockable
{
private:
    bool m_isLocked = false;

public:
    void lock() { m_isLocked = true; }
    void unlock() { m_isLocked = false; }

    bool isLocked() const noexcept { return m_isLocked; }
};

class DummyNotLockable { };

TEST_CASE( "UnlockOnDestruct", "[UnlockOnDestruct]" ) {

    REQUIRE( com::concepts::Lockable<DummyLockable> == true );
    REQUIRE( com::concepts::Lockable<DummyNotLockable> == false );
    DummyLockable lockable;
    {
        com::UnlockOnDestruct<DummyLockable> unlocker(lockable);
        REQUIRE(unlocker->isLocked() == true);
    }
    REQUIRE(lockable.isLocked() == false);
    {
        lockable.lock();
        com::UnlockOnDestruct<DummyLockable> unlocker(lockable, /* acquireLock = */ false);
    }
    REQUIRE(lockable.isLocked() == false);
}


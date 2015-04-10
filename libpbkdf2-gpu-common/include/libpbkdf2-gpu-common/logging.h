#ifndef PBKDF2_GPU_COMMON_LOGGING_H
#define PBKDF2_GPU_COMMON_LOGGING_H

#include <ostream>
#include <mutex>

namespace pbkdf2_gpu {
namespace common {

class RootLogger;
class LockedLogger;

class Logger
{
public:
    virtual LockedLogger open() = 0;

    template<class T>
    LockedLogger operator<<(T& value);
};

class RootLogger : public Logger
{
private:
    std::ostream *out;
    std::recursive_mutex mutex;

public:
    inline RootLogger(std::ostream *out)
        : out(out), mutex()
    {
    }

    inline LockedLogger open() override;

    friend class LockedLogger;
};

class LockedLogger
{
private:
    RootLogger *parent;

public:
    inline LockedLogger() : parent(nullptr) { }
    inline LockedLogger(RootLogger *parent)
        : parent(parent)
    {
        parent->mutex.lock();
    }
    inline LockedLogger(const LockedLogger &other)
        : parent(other.parent)
    {
        parent->mutex.lock();
    }
    inline ~LockedLogger()
    {
        if (parent != nullptr) {
            parent->mutex.unlock();
        }
    }

    LockedLogger operator <<(std::ostream & f(std::ostream &)) const
    {
        *parent->out << f;
        return *this;
    }

    template<class T>
    LockedLogger operator <<(T &value) const
    {
        *parent->out << value;
        return *this;
    }
};

template<class T>
LockedLogger Logger::operator<<(T& value)
{
    return open() << value;
}

inline LockedLogger RootLogger::open()
{
    return LockedLogger(this);
}

class SubLogger : public Logger
{
private:
    Logger *parent;
    std::string prefix;

public:
    inline SubLogger() { }

    inline SubLogger(Logger *parent, const std::string &prefix)
        : parent(parent), prefix(prefix)
    {
    }

    inline LockedLogger open() override
    {
        return *parent << prefix;
    }
};

} // namespace common
} // namespace pbkdf2_gpu

#endif // PBKDF2_GPU_COMMON_LOGGING_H

/*
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    void lock()
    {
        if (out != nullptr) {
            mutex.lock();
        }
    }
    void unlock()
    {
        if (out != nullptr) {
            mutex.unlock();
        }
    }

public:
    RootLogger(std::ostream *out)
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
    LockedLogger() : parent(nullptr) { }
    LockedLogger(RootLogger *parent)
        : parent(parent)
    {
        parent->lock();
    }
    LockedLogger(const LockedLogger &other)
        : parent(other.parent)
    {
        if (parent != nullptr) {
            parent->lock();
        }
    }
    LockedLogger(LockedLogger &&other)
        : parent(other.parent)
    {
        other.parent = nullptr;
    }
    ~LockedLogger()
    {
        if (parent != nullptr) {
            parent->unlock();
        }
    }
    LockedLogger &operator=(const LockedLogger &other)
    {
        if (parent != nullptr) {
            parent->unlock();
        }
        parent = other.parent;
        if (parent != nullptr) {
            parent->lock();
        }
        return *this;
    }
    LockedLogger &operator=(LockedLogger &&other)
    {
        if (parent != nullptr) {
            parent->unlock();
        }
        parent = other.parent;
        other.parent = nullptr;
        return *this;
    }

    LockedLogger operator <<(std::ostream &(&f)(std::ostream &)) const
    {
        if (parent->out != nullptr) {
            *parent->out << f;
        }
        return *this;
    }

    template<class T>
    LockedLogger operator <<(T &value) const
    {
        if (parent->out != nullptr) {
            *parent->out << value;
        }
        return *this;
    }
};

template<class T>
LockedLogger Logger::operator<<(T& value)
{
    return open() << value;
}

LockedLogger RootLogger::open()
{
    return LockedLogger(this);
}

class SubLogger : public Logger
{
private:
    Logger *parent;
    std::string prefix;

public:
    SubLogger() { }

    SubLogger(Logger *parent, const std::string &prefix)
        : parent(parent), prefix(prefix)
    {
    }

    LockedLogger open() override
    {
        return *parent << prefix;
    }
};

} // namespace common
} // namespace pbkdf2_gpu

#endif // PBKDF2_GPU_COMMON_LOGGING_H

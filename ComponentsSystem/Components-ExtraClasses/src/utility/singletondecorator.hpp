#pragma once

namespace ExtraClasses {

/**
 * @brief The SingletonDecorator class Equ of boost::noncopyable
 */
class SingletonDecorator
{
    SingletonDecorator(const SingletonDecorator&) = delete;
    SingletonDecorator(SingletonDecorator&&) = delete;
    SingletonDecorator operator=(const SingletonDecorator&) = delete;
    SingletonDecorator operator=(SingletonDecorator&&) = delete;
public:
    SingletonDecorator() = default;
    virtual ~SingletonDecorator() = default;
};

} // namespace ExtraClasses

#ifndef ORDER_H
#define ORDER_H

#include <vector>

template <typename T> class Order
{
public:
    Order()
    {
        mCurrentIndex = 0;
    }

    Order(std::initializer_list<T> l) : mValues(l)
    {
        mCurrentIndex = 0;
    }

    void set(std::vector<T> values)
    {
        mValues = values;
        reset();
    }

    std::vector<T> get() const
    {
        return mValues;
    }

    void append(const T &value)
    {
        mValues.push_back(value);
    }

    void clear()
    {
        mValues.clear();
        reset();
    }

    void advance()
    {
        mCurrentIndex++;

        if (mCurrentIndex >= mValues.size())
            mCurrentIndex = 0;
    }

    bool contains(const T &value)
    {
        for(const auto &v : mValues)
        {
            if(v == value)
                return true;
        }

        return false;
    }

    size_t size()
    {
        return mValues.size();
    }

    void reset()
    {
        mCurrentIndex = 0;
    }

    T current() const
    {
        return mValues.at(mCurrentIndex);
    }

private:
    std::vector<T> mValues;
    size_t mCurrentIndex;
};

#endif // ORDER_H

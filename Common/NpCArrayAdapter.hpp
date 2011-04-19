#ifndef _NpCArrayAdapter_hpp_
#define _NpCArrayAdapter_hpp_

// A simple template to ease handling of c-style arrays

template <typename elemType, int size>
class NpCArrayAdapter
{
public:
    NpCArrayAdapter()
    { }

    operator const elemType*() const
    { return mArray; }

    operator elemType*()
    { return mArray; }
    
private:
    elemType mArray[size];
};


template <typename elemType, int rows, int cols>
class NpCDArrayAdapter
{
public:
    NpCDArrayAdapter()
    {
        for (unsigned int i = 0; i < rows; ++i)  {
            mArray[i] = new elemType[cols];
        }
    }

    NpCDArrayAdapter(const NpCDArrayAdapter& arr)
    {
        for (unsigned int i = 0; i < rows; ++i)  {
            mArray[i] = new elemType[cols];
            for (unsigned int j = 0; j < cols; ++j)  {
                mArray[i][j] = arr.mArray[i][j];
            }
        }
    }

    NpCDArrayAdapter& operator=(const NpCDArrayAdapter& arr)
    {
        if (this != &arr)  {
            for (unsigned int i = 0; i < rows; ++i)  {
                delete mArray[i];

                mArray[i] = new elemType[cols];
                for (unsigned int j = 0; j < cols; ++j)  {
                    mArray[i][j] = arr.mArray[i][j];
                }
            }
        }
    }

    ~NpCDArrayAdapter()
    {
        for (unsigned int i = 0; i < rows; ++i)  {
            delete[] mArray[i];
        }
    }

    operator const elemType**() const
    { return mArray; }

    operator elemType**()
    { return mArray; }
    
private:
    elemType* mArray[rows];
};


#endif   // _NpCArrayAdapter_hpp_

#include <iostream>
#include <vector>
#include <cstdarg>

using namespace std;

template<class T>
class array_ND  // N维数组仿真器
{
public:
    array_ND() :_data{ nullptr }, _dims{}, _transfer{}, _if_internal_memory{false}
    {
        _dims.reserve(10);
        _transfer.reserve(10);
    }
    array_ND(T* ptr, std::initializer_list<int> dims) :_if_internal_memory{false}
    {
        _data = ptr;

        _dims.reserve(10);
        _transfer.reserve(10);
        _dims.assign(dims.begin(), dims.end());

        _transfer.assign(_dims.size(), 1);

        for (int i = _dims.size() - 2; i >= 0; --i)
        {
            _transfer[i] = _transfer[i + 1] * _dims[i + 1];
        }
    }
    ~array_ND()
    {
        try_delete();
    }
    void clear()
    {
        try_delete();
    }
public:
    // 固定首参：T* 数据指针，后面全部是可变维度 int
    void attach(T* ptr, std::initializer_list<int> dims)
    {
        try_delete();

        _data = ptr;
        _dims.assign(dims.begin(), dims.end());

        _transfer.assign(_dims.size(), 1);

        for (int i = _dims.size() - 2; i >= 0; --i)
        {
            _transfer[i] = _transfer[i + 1] * _dims[i + 1];
        }
    }
public:
    // 如果已经attach了，就只对原来数组在维度上变形，但是总数据量不能变
    // 如果原来没有数据，则会自动分配内存，在动态内存上处理
    bool resize(std::initializer_list<int> dims)
    {
        int dim_total1 = 1;
        for (auto data : dims)
            dim_total1 *= data;

        int dim_total2 = 1;
        for (auto data1 : _dims)
            dim_total2 *= data1;

        // 1.如果使用内部内存，或者 已经有对应内存
        if ( ((_dims.size() != 0) && (_data != nullptr)))
        {
            if (dim_total1 != dim_total2)
            {
                cout << "ERROE: bad parameter on calling array_ND::resize()" << endl;
                return false;
            }
        }
        else 
        {
            _if_internal_memory = true;
            _data = new T[dim_total1]{};
        }

        //-------------------------------------------------------------------------------
        _dims.assign(dims.begin(), dims.end());
        _transfer.assign(_dims.size(), 1);

        for (int i = _dims.size() - 2; i >= 0; --i)
        {
            _transfer[i] = _transfer[i + 1] * _dims[i + 1];
        }



    }
public:
    // 直接传下标，依靠 _dims.size() 确定参数数量
    T& operator()(int first, ...)
    {
        va_list ap;
        va_start(ap, first);

        int idx = first * _transfer[0];
        int count = 1;

        try
        {
            for (int i = 1; i < _dims.size(); ++i)
            {
                int sub = va_arg(ap, int);
                idx += sub * _transfer[i];
                count++;
            }

            va_end(ap);
        }
        catch (...)
        {
            string error = "invalid parameters when calling array_ND::operator()";
            cout << error << endl;
            exit(0);
        }

        return _data[idx];
    }

    const T& operator()(int first, ...) const
    {
        va_list ap;
        va_start(ap, first);

        int idx = first * _transfer[0];
        int count = 1;

        try
        {
            for (int i = 1; i < _dims.size(); ++i)
            {
                int sub = va_arg(ap, int);
                idx += sub * _transfer[i];
                count++;
            }

            va_end(ap);
        }
        catch(...)
        {
            string error = "invalid parameters when calling array_ND::operator()";
            cout << error << endl;
            exit(0);            
        }

        return _data[idx];
    }


    // 友元声明：全局流输出重载
    template<typename U>
    friend ostream& operator<<(ostream& os, const array_ND<U>& arr);


 protected:
     void Print() const
     {
         if (!_data || _dims.empty())
         {
             cout << "Empty array!" << endl;
             return;
         }
         vector<int> pos(_dims.size(), 0);
         printRecursive(0, pos, cout);
         cout << endl;
     }

protected:
    void printRecursive(int curDim, vector<int>& pos, ostream& os) const
    {
        if (curDim == _dims.size())
        {
            int offset = 0;
            for (int i = 0; i < pos.size(); ++i)
                offset += pos[i] * _transfer[i];
            os << _data[offset] << " ";
            return;
        }

        int maxLen = _dims[curDim];
        for (int i = 0; i < maxLen; ++i)
        {
            pos[curDim] = i;
            printRecursive(curDim + 1, pos, os);
        }
        if (curDim != 0)
            os << endl;
    }
protected:
    void try_delete()
    {
        if (_if_internal_memory && _data)
        {
             delete[]_data;
        }
        _data = nullptr;
        _dims.clear();
        _transfer.clear();
        _if_internal_memory = false;
    }

protected:
    T* _data = nullptr;
    vector<int>      _dims;                 // 不同维上的数据容量
    vector<int>      _transfer;             // 辅助计算不同维的偏移

    bool             _if_internal_memory;   //如果是内部动态分配的内存！
};

// 全局重载 << 运算符
template<typename T>
ostream& operator<<(ostream& os, const array_ND<T>& arr)
{
    if (!arr._data || arr._dims.empty())
    {
        os << "[Empty N-D Array]";
        return os;
    }
    vector<int> pos(arr._dims.size(), 0);
    arr.printRecursive(0, pos, os);
    return os;
}

// 测试
int test1()
{
    int mat2[6] = { 1,2,3,4,5,6 };
    array_ND<int> arr2;
    arr2.attach(mat2, { 2, 3 });
    cout << "二维数组：\n" << arr2 << endl;
    cout << arr2(1, 2) << endl;

    int mat3[8] = { 11,22,33,44,55,66,77,88 };
    array_ND<int> arr3;
    arr3.attach(mat3, {2, 2, 2});
    cout << "三维数组：\n" << arr3 << endl;

    // 下标读写测试
    arr2(1, 0) = 99;
    cout << "修改后：\n" << arr2 << endl;


    double data[3][3][3][3] = { {1},{2},{3} };
    array_ND<double> arr4;
    arr4.attach(&(data[0][0][0][0]), { 3, 3, 3, 3 });
    cout << arr4 << endl;
    cout << arr4(1, 0, 0, 0) << endl;

    arr4(0, 0, 0, 0) = 10;
    cout << arr4 << endl;
    cout << arr4(0, 0, 0, 0) << endl;
    try {
        cout << arr4(1, 2) << endl;
    }
    catch (string error)
    {
        cout << error << endl;
    }
    return 0;
}

int test2()
{
    array_ND<double> arr1;
    double data[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    arr1.attach(data, { 2,8 });
    cout << arr1 << endl;
    cout << "---------------------------" << endl;
    
    arr1.resize({4,4});
    cout << arr1 << endl;
    cout << "---------------------------" << endl;


    arr1.resize({ 2,2,4 });
    cout << arr1 << endl;
    cout << "---------------------------" << endl;
    arr1.resize({ 4,2,2 });
    cout << arr1 << endl;
    cout << "---------------------------" << endl;
    arr1(0, 0, 0, 0) = 101;
    cout << arr1(0,0,0,0)<<endl;

    cout << "---------------------------" << endl;
    array_ND<double> arr2;
    arr2.resize({3,3});
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            arr2(i, j) = i * 3 + j;
        }
    }
    cout << arr2;
    cout << "---------------------------" << endl;
    arr2.resize({ 9,1 });
    cout << arr2 << endl;
    cout << "---------------------------" << endl;
    arr2.resize({ 1,9 });
    cout << arr2 << endl;

    cout << "---------------------------" << endl;
    arr2.resize({3,1,1,1,1,1,3});
    cout << arr2 << endl;

    return 0;
}

int main()
{
    test2();
}

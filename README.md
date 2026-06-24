# array_ND

可变形动态数组，采用一维数据模拟高维数组，且高维数组支持动态变形，包括维度修改。
by zxg, 
SEU.

# 例子程序
## 例子程序1
```cpp
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
```
## 例子程序2
```cpp
int test2()
{
    array_ND<double> arr1;
    double data[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    arr1.attach(data, { 2,8 });   // double    [2][8]
    cout << arr1 << endl;
    cout << "---------------------------" << endl;
    
    arr1.resize({4,4});          // double    [4][4]
    cout << arr1 << endl;
    cout << "---------------------------" << endl;


    arr1.resize({ 2,2,4 });      // double    [2][2][4]
    cout << arr1 << endl;
    cout << "---------------------------" << endl;
    arr1.resize({ 4,2,2 });      // double    [4][2][2]
    cout << arr1 << endl;
    cout << "---------------------------" << endl;
    arr1(0, 0, 0, 0) = 101;
    cout << arr1(0,0,0,0)<<endl;

    cout << "---------------------------" << endl;
    array_ND<double> arr2;
    arr2.resize({3,3});          // double    [3][3]
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            arr2(i, j) = i * 3 + j;
        }
    }
    cout << arr2;
    cout << "---------------------------" << endl;
    arr2.resize({ 9,1 });         // double    [9][1]
    cout << arr2 << endl;
    cout << "---------------------------" << endl;
    arr2.resize({ 1,9 });         // double    [1][9]
    cout << arr2 << endl;

    cout << "---------------------------" << endl;
    arr2.resize({3,1,1,1,1,1,3}); // double    [3][1][1][1][1][1][3]
    cout << arr2 << endl;

    return 0;
}

```



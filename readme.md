# *v24.03.13-2234*

1. 看到了:

   [C++ 并发编程(6) 利用条件变量实现线程安全队列_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1934y1N7Nb/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3) 
   对应`test_61.cpp`; 13:09

2. 文件结构改了下, 跟视频课对应;

# *v24.03.12-2238*

1. 看到了:
   [C++ 并发编程(4) unique_lock,共享锁以及递归锁_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1wz4y1T7fN/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3) 37:08
   对应`test_36.cpp`;

# *v24.03.12-0905*
1. `test_34.cpp`是比较重要的知识点;
2. `test_34.cpp`最后`4.层级锁`太抽象没看完, 以后用到再回顾;

# *v24.03.11-2247*

1. 看到了C++17的`std::scoped_lock`

   [C++ 并发编程(3) 互斥和死锁_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1AN4y1o78q/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3) 1:43:20

# *v24.03.10-2242*

1. 看到:

   [C++ 并发编程(3) 互斥和死锁_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1AN4y1o78q/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3) 1:10:31

# *v24.03.08-2246*

1. 看到了:

   [C++ 并发编程(3) 互斥和死锁_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1AN4y1o78q/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3) 0:26:00

# *v24.03.08-1643*

1. 看完了第二节,开始第三节.

# *v24.03.07-2236*
1. 看到了 *joining_thread*

   [C++ 并发编程(2) 线程所属权管理_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1v8411R7hD/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3) 0:24:09

# *v24.03.07-1937*
1. 将`main.cpp`中的知识点拆分出来了.
2. 第一节看完.
3. `test_4`, `test_6`, `test_7` 都有看不懂的; ???

# *v24.03.06-2237*
1. 不使用`join`而使用智能指针来防止函数`oops_sp`内的局部变量`someLocalState`被释放,但是好像没有效果,不知道该怎么改.

   ```cpp
   struct func_sp {  // smart pointer
     std::shared_ptr<int> _sp;
     func_sp(std::shared_ptr<int> sp) : _sp(sp) {}
     void operator()() {
       for (int i = 0; i < 3; i++) {
         *_sp = i;
         std::cout << "--- _sp is " << *_sp << std::endl;
         std::this_thread::sleep_for(std::chrono::seconds(1));
       }
     }
   };
   
   void oops_sp() {
     auto someLocalState = std::make_shared<int>(0);
     func_sp myfunc(someLocalState);
     std::thread functhread(myfunc);
     functhread.detach();
   }
   
   oops_sp();
   ```

2. 看到了 *慎用隐式转换*

​	[C++ 并发编程(1) 线程基础，为什么线程参数默认传参方式是值拷贝？_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1FP411x73X/?spm_id_from=333.999.0.0&vd_source=a8a38358873e306ffdd6017aaab418e3) 0:42:54



# *v24.03.05-2238*
看到了 *detach的隐患*

[C++ 并发编程(1) 线程基础，为什么线程参数默认传参方式是值拷贝？_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1FP411x73X/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3) 0:25:08

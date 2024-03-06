# *v24.03.05-2238*

看到了 *detach的隐患*

[C++ 并发编程(1) 线程基础，为什么线程参数默认传参方式是值拷贝？_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1FP411x73X/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3) 0:25:08

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


#include <iostream>
#include <string>
#include <thread>

void thread_work_1(std::string str) {
  std::cout << "--- str is: " << str << std::endl;
}

class backgroundTask {
 public:
  void operator()() { std::cout << "--- backgroundTask called" << std::endl; }
};

struct func {
  int& _i;
  func(int& i) : _i(i) {}
  void operator()() {
    for (int i = 0; i < 3; i++) {
      _i = i;
      std::cout << "---[func] _i is " << _i << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
};

struct func_sp {  // smart pointer
  std::shared_ptr<int> _sp;
  func_sp(std::shared_ptr<int> sp) : _sp(sp) {}
  void operator()() {
    for (int i = 0; i < 3; i++) {
      *_sp = i;
      std::cout << "---[func_sp] _sp is " << *_sp << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
};

void oops() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread functhread(myfunc);
  /*
  这里将线程`functhread`分离(独立于主线程运行),这里有一个隐患!
  线程函数`myfunc`(即其仿函数)内部的`_i`是`someLocalState`的引用,且`_i=1`会修改,所以
  线程函数一直会用到`someLocalState`,但是当`oops`执行完后局部变量`someLocalState`会
  被回收,导致意外发生.
  */
  functhread.detach();
}

void oops_sp() {
  auto someLocalState = std::make_shared<int>(0);
  func_sp myfunc(someLocalState);
  std::thread functhread(myfunc);
  functhread.detach();
}

void oops_usejoin() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread functhread(myfunc);
  /*
  这里将原本detach的线程改为join,让主线程等待子线程结束.
  */
  functhread.join();
}

/*
异常捕获(try/catch),来防止主线程崩溃导致子线程也崩溃.
*/
void catch_exception() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread functhread{myfunc};
  try {
    // 本线程做一些事情,可能引发崩溃
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } catch (std::exception& e) {
    functhread.join();
    throw;
  }
}

/*
线程守卫.避免用try/catch的臃肿方式.
使用RAII技术,用于管理`std::thread`对象生命周期,
保证线程对象析构的时候等待线程运行结束再回收资源.
*/
class thread_guard {
 private:
  std::thread& _t;

 public:
  explicit thread_guard(std::thread& t) : _t(t) {}
  ~thread_guard() {
    if (_t.joinable()) {
      _t.join();
    }
  }
};

void auto_guard() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread t1(myfunc);
  /*
  将t1线程作为参数传递给类对象`tg`
  当`auto_guard`函数执行完后会回收局部对象`tg`,此时触发`thread_guard`的析构,析构内会判断`t1`线程是否
  可`join`,如果可以则等待其执行完毕.以起到防止资源泄露的作用.
  */
  thread_guard tg(t1);
  //...本线程做一些事情
  /*
  主线程会先执行下面这段输出,随后依然有子线程的输出`_i is
  ..`输出,代表成功守卫了子线程,让主线程 等待(join)子线程结束后才回收资源.
  */
  /*
  因为这里对象是创建在堆区上的,所以"先创建后析构",所以这里`t1`不会出现比`tg`先执行析构的场景,所以
  不用担心`t1`线程先执行完后执行析构.
  */
  std::cout << "=== auto_guard function has finished " << std::endl;
}

/* 慎用隐式转换 */
void print_str(int i, const std::string& s) {
  std::cout << "--- i is " << i << " str is " << std::endl;
}

void oops_danger(int som_param) {
  char buffer[1024];
  sprintf(buffer, "%i", som_param);
  // 在线程内部将char const* 转化为std::string
  std::thread t(print_str, 3, buffer);  // 3和buffer是`print_str`的实参
  t.detach();
  std::cout << "danger oops finished " << std::endl;
}

/* 绑定引用 */
void change_param(int& param) { param++; }

void oops_ref(int someParam) {
  std::cout << "--- before change, param is: " << someParam << std::endl;
  /*
  需要显式引用转换:`std::ref()`
  因为`change_param`接收一个"左值引用"但传入的是一个"右值".
  `std::thread`的构造函数在处理参数时,会进行参数的复制,而不是引用.(具体看源码,目前看不懂
  ???)
  */
  // std::thread t2(change_param, someParam);  //错误
  std::thread t2(change_param, std::ref(someParam));
  t2.join();
  std::cout << "--- after change, param is: " << someParam << std::endl;
}

/* 绑定类成员函数 */
class X {
 public:
  void do_lengthy_work() { std::cout << "--- do lengthy work" << std::endl; }
};

void oops_bind_classfunc() {
  X myX;
  std::thread t(&X::do_lengthy_work, &myX);
  // 如果`do_lengthy_work`有参数:
  // std::thread t(&X::do_lengthy_work, &myX, [param1],...);
  t.join();
}

/* 线程函数参数是unique类型 */
void deal_unique(std::unique_ptr<int> p) {
  std::cout << "--- unique ptr is: " << *p << std::endl;
  (*p)++;
  std::cout << "--- after unique ptr is: " << *p << std::endl;
}

void oops_move() {
  auto p = std::make_unique<int>(100);
  /*
  `std::thread`会对传入的参数进行"拷贝"或"移动",所以当传入`std::unique_ptr
  p`时,`p`会被拷贝到 内部线程中,又因`unique_ptr`不可拷贝,所以会出错.
  这时必须要用`std::move`标记进行移动操作.
  */
  std::thread t(deal_unique, std::move(p));
  t.join();
  //`move`后不能再使用`p`了,已被`move`废弃(无法输出了p了)
  std::cout << "after unique data is " << *p << std::endl;
}

int main() {
  std::string helloStr = "hello world!";
  /* 1.通过()创建一个线程`t1`,并初始化线程函数 */
  std::thread t10(thread_work_1, helloStr);
  std::thread t11(
      &thread_work_1,
      helloStr);  // 普通函数前面加不加`&`都行,因为函数名会退化为地址
  /*
  试图让主线程睡眠来等待是无效的,因为线程有默认优化:
  当主线程结束时,就会调用子线程的析构函数,析构函数内部会调用`terminate`函数,这会触发`assert`断言导致崩溃.
  */
  // std::this_thread::sleep_for(std::chrono::seconds(1));  //睡眠1s

  /* 2.使用`join()`让主线程等待子线程结束 */
  t10.join();
  t11.join();

  /* 3.仿函数作为线程函数 */
  /*
  这里传入仿函数对象是错误的.
  会被认为`t21`是函数对象,`std::thread`被解释为返回值,`backgroundTask()`看做函数指针,`backgroundTask`被解释为
  返回值,参数void, 如下:
  std::thread (*)(backgroundTask (*)())
  */
  // std::thread t21(backgroundTask());
  // t21.join();

  // 方法一:多加一层()
  std::thread t22((backgroundTask()));
  t22.join();

  // 方法二:改用{},代表初始化
  std::thread t23{backgroundTask()};
  t23.join();

  // 方法三:因为类下重载了`()`定义了仿函数,所以可以直接传入其对象
  backgroundTask bgT;
  std::thread t24(bgT);
  t24.join();

  /* 4.lambda表达式作为线程函数 */
  std::thread t3(
      [](std::string str) { std::cout << "--- str is " << str << std::endl; },
      helloStr);
  t3.join();

  /* 5.detach的注意事项 */
  // oops();
  // std::this_thread::sleep_for(std::chrono::seconds(1));

  // 方法一:使用智能指针包装变量(没生效还是有问题,先搁置 ???)
  // oops_sp();

  /* 6.join用法 */
  oops_usejoin();

  /* 7.try/catch 捕获异常 */
  // catch_exception();

  /* 8.自动守卫 */
  auto_guard();

  /* 9.慎用隐式转换 (没听懂,先搁置 ???) */
  // oops_danger();

  /* 10.绑定引用 */
  oops_ref(10);

  /* 11.线程绑定类的成员函数 */
  oops_bind_classfunc();

  /* 12.线程函数参数是unique类型 */
  oops_move();

  return 0;
}
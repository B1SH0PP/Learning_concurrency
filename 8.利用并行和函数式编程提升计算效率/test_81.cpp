#include <iostream>

/* 这里只介绍了普通串行的快排,"函数式编程"和"利用并行进行函数式编程"先搁置,感觉没啥用 */

// 打印数组
void printArr(const int arr[]) {
  for (int i = 0; i < 6; i++) {
    std::cout << arr[i] << "\t";
  }
  std::cout << std::endl;
}

/* 快速排序(分治) */
template <typename T>
//`start`:0;`end`:len(arr)-1
void quick_sort_recursive(T arr[], int start, int end) {
  if (start > end) return;
  T pivot = arr[start];  // 选取第一个元素作为基准(第一个开始)
  int left = start, right = end;
  // 两指针相遇则停止
  while (left < right) {
    // 右边元素小于key则停止,左右指针相遇也停止
    while (arr[right] >= pivot && left < right)
      right--;
    // 左边元素大于key则停止,左右指针相遇也停止
    while (arr[left] <= pivot && left < right)
      left++;
    std::swap(arr[left], arr[right]);
    printArr(arr);
  }

  if (arr[left] < pivot) {
    // 此时就找到了基准`pivot`的准确位置
    std::swap(arr[left], arr[start]);
    printArr(arr);
  }

  quick_sort_recursive(arr, start, left - 1);
  quick_sort_recursive(arr, left + 1, end);
}

template <typename T>
void quick_sort(T arr[], int len) {
  quick_sort_recursive(arr, 0, len - 1);
}

void test_quick_sort() {
  int arr[6] = {2, 1, 9, 1, 5, 7};
  printArr(arr);
  quick_sort(arr, 6);
  printArr(arr);
}

int main() {
  test_quick_sort();
}
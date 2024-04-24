#include <stdio.h>

// 函数声明
int multiply(int x, int y);

int main() {
    int a = 10;
    int b = 5;
    int result;
    
    if (a > b) {
        result = a - b;
    } else {
        result = a + b;
    }
    
    for (int i = 0; i < result; i++) {
        printf("%d ", i);
    }
    printf("\n");
    
    switch(result) {
        case 5:
            printf("Result is 5\n");
            break;
        case 10:
            printf("Result is 10\n");
            break;
        default:
            printf("Result is neither 5 nor 10\n");
    }
    
    // 使用位移运算符将result左移1位
    result = result << 1;
    printf("Result after left shift by 1: %d\n", result);
    
    // 使用位移运算符将result右移1位
    result = result >> 1;
    printf("Result after right shift by 1: %d\n", result);
    
    // 调用函数计算两个数的乘积
    int product = multiply(a, b);
    printf("Product of %d and %d is: %d\n", a, b, product);
    
    return 0;
}

// 函数定义
int multiply(int x, int y) {
    return x * y;
}


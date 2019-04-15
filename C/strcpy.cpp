#include <stdio.h>
#include <string.h>

int main()
{
	char arr1[2];         // 栈上分配了2个空间
	char *arr2 = "abcd";  // 栈上分配了4个空间

	if (strcpy(arr1, arr2)) {
		printf("success\n");
	}
	else {
		printf("failed\n");
	}

	// 打印arr1 string：（因为字符串读到结尾\0，所以此处打印为abcd）
	printf("arr1(string)is : %s\n",arr1);
	// 打印arr1 越界字符：（strcpy函数不安全的原因就是越界赋值）
	printf("arr1[3] :%c\n", arr1[3]);

	return(0);
}
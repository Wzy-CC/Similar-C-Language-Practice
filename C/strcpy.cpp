#include <stdio.h>
#include <string.h>

int main()
{
	char arr1[2];         // ջ�Ϸ�����2���ռ�
	char *arr2 = "abcd";  // ջ�Ϸ�����4���ռ�

	if (strcpy(arr1, arr2)) {
		printf("success\n");
	}
	else {
		printf("failed\n");
	}

	// ��ӡarr1 string������Ϊ�ַ���������β\0�����Դ˴���ӡΪabcd��
	printf("arr1(string)is : %s\n",arr1);
	// ��ӡarr1 Խ���ַ�����strcpy��������ȫ��ԭ�����Խ�縳ֵ��
	printf("arr1[3] :%c\n", arr1[3]);

	return(0);
}
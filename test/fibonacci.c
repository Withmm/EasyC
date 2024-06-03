int fibonacci(int n) {
	int left = 1;
	int right = 1;
	for (int i = 0; i < n - 1; i = i + 1) {
		int tmp = left + right;
		left = right;
		right = tmp;
	}

	return left;
}


int main() {
	int x = fibonacci(5);
	return x;	
}

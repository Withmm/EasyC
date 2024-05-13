void func1(int x, int y)
{

}
int func2(int x, int y,int z)
{
	x = (y + 5) / 2;
	int y = (func1(y, z) + 1) / 2;
	return x + y + z;
}

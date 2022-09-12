#pragma once
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

struct Target
{
	float x = 0.0;
	float y = 0.0;
	bool available = true;
};

class TargetGenerator
{
	Target targets[20];
	char message[256];

public:
	TargetGenerator();
	~TargetGenerator();
	char* Generate(int count, int minX, int maxX, int minY, int maxY);
	char* GetUpdateMessage();
	bool Hit(int index);

private:
	float randRange(int low, int high);
};
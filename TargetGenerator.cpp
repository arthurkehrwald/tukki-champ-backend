#include "TargetGenerator.h"

TargetGenerator::TargetGenerator()
{
	srand(time(NULL));
}

TargetGenerator::~TargetGenerator()
{

}

char* TargetGenerator::Generate(int count, int minX, int maxX, int minY, int maxY)
{
	if (count > 20)
	{
		return NULL;
	}
	strcpy_s(message, sizeof(message), "targets:");

	for (int i = 0; i < count; i++)
	{

		Target* pTarget = new Target;
		pTarget->x = randRange(minX, maxX);
		pTarget->y = randRange(minY, maxY);
		pTarget->available = false;
		targets[i] = *pTarget;

		char targetInfo[13];
		sprintf_s(targetInfo, 13, "%.2f,%.2f:", pTarget->x, pTarget->y);
		strcat_s(message, 256, targetInfo);
	}
	message[8 + count * 10] = '\0';
	return message;
}

char* TargetGenerator::GetUpdateMessage()
{
	char message[22];
	message[0] = '\0';
	strcpy_s(message, sizeof(message), "t:");
	for (int i = 0; i < 20; i++)
	{
		strcat_s(message, sizeof(message), targets[i].available ? "1" : "0");
	}
	return message;
}

bool TargetGenerator::Hit(int index)
{
	bool available = targets[index].available;
	targets[index].available = false;
	return available;
}

float TargetGenerator::randRange(int low, int high)
{
	int r = rand() % (high - low + 1) + low;
	return r + (rand() % 100) / 100.0f;
}
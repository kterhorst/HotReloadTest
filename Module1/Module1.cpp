// Module1.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Module1.h"

// Not really a calculator ;)
class MyCalculator : public ICalculator
{
public:
	void AnotherFunction(int& x)
	{
		m_NewField1++;
		x = GetSomeLambda()() + m_NewField1;
	}

	int DoSomeStuff(int x, int y) override
	{
		int z;
		AnotherFunction(z);
		return x + y + z;
	}

	std::function<int()> GetSomeLambda() override
	{
		return [this]() {
			// Unfortunately, references to this lambda don't get patched.
			// GetSomeLambda has to be called again after reload to get the updated
			// function.
			return 100;
		};
	}

private:
	int m_SomeValue = 1337;
	int m_NewField1 = 0;

	// Adding new fields at runtime doesn't work for obvious reasons.
	// This can be circumvented by adding a placeholder memory buffer.
	// Thus new fields will then overlap with it.
	char _placeholder_[128] = {};
};

extern "C" MODULE1_API ICalculator* NewCalculator(void)
{
	return new MyCalculator;
}

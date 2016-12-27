// Host.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Module1/Module1.h"

#define DEBUG_OUT 1

namespace HotReload {
namespace Detail {
void UnprotectPage(uintptr_t addr)
{
	DWORD prev;
	if (!VirtualProtect(reinterpret_cast<void*>(addr),
						4096, PAGE_EXECUTE_READWRITE, &prev))
	{
		printf("VirtualProtect failed\n");
		throw;
	}
}

bool FileExists(const std::string& path)
{
	std::ifstream fs(path);
	return fs.good();
}
}

struct Symbol
{
	std::string name;
	std::string objectFileName;
	uint64_t rva = 0;
	bool isFunction = false;
};

struct SymbolMap
{
	std::string path;
	std::vector<std::unique_ptr<Symbol>> symbols;

	auto begin()
	{
		return symbols.begin();
	}

	auto end()
	{
		return symbols.end();
	}

	Symbol* FindSymbol(const std::string& name)
	{
		for (auto& sym : symbols)
		{
			if (sym->name == name)
				return sym.get();
		}
		return nullptr;
	}

	Symbol* FindSymbolByRVA(uint64_t rva)
	{
		for (auto& sym : symbols)
		{
			if (sym->rva == rva)
				return sym.get();
		}
		return nullptr;
	}
};

std::ostream& operator << (std::ostream& s, const Symbol& sym)
{
	std::stringstream ss;
	ss << "Symbol: '" << sym.name << "' at: 0x" << std::hex
		<< sym.rva << "  in " << sym.objectFileName;
	s << ss.str();
	return s;
}

std::ostream& operator << (std::ostream& s, const SymbolMap& mapFile)
{
	for (auto& sym : mapFile.symbols)
	{
		s << *sym << std::endl;
	}
	return s;
}

bool ReadMapFile(SymbolMap& outMapFile, const std::string& mapFilePath)
{
	outMapFile.path = mapFilePath;
	std::ifstream fs(mapFilePath);
	if (!fs.good())
		return false;
	std::string line;
	const auto npos = std::string::npos;
	bool found = false;
	while (!fs.eof())
	{
		std::getline(fs, line);
		if (line.find("Preferred load address is", 0) != npos)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		printf("Preferred load address not found\n");
		return false;
	}

	uint64_t baseAddress;
	{
		std::stringstream ss;
		std::string part;
		ss << line;
		ss >> part >> part >> part >> part >> std::hex >> baseAddress;
	}

	found = false;
	while (!fs.eof())
	{
		std::getline(fs, line);
		if (line.find("Publics by Value", 0) != npos)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		printf("Publics by Value not found\n");
		return false;
	}

	std::getline(fs, line);
	std::getline(fs, line);

	while (!fs.eof())
	{
		std::getline(fs, line);
		std::stringstream ss;
		std::string part;
		ss << line;

		// Hack, hack
		if (line.find("entry point at") != npos ||
			line.find("Static symbols") != npos)
			break;

		// Parse address
		auto sym = std::make_unique<Symbol>();
		ss >> part >> sym->name;
		ss >> std::hex >> sym->rva;
		sym->rva = sym->rva - baseAddress;

		ss >> part;
		if (part == "f")
		{
			sym->isFunction = true;
			ss >> part;
		}
		if (part == "i")
		{
			ss >> part;
		}
		if (part.length() == 1)
		{
			printf("%s\n", line.data());
			// Unknown flag?
			throw;
		}

		if (part.find(':') != npos)
			continue;
		if (part.find(".obj") == npos)
			continue;

		sym->objectFileName = part;
		outMapFile.symbols.push_back(std::move(sym));
	}

#if DEBUG_OUT
	std::cout << outMapFile << std::endl;
#endif

	return true;
}

struct LoadedModule
{
	std::string name;
	std::string path;
	HMODULE moduleHandle = nullptr;
	std::unique_ptr<SymbolMap> symbolMap;

	uint64_t GetBaseAddress() const
	{
		return reinterpret_cast<uint64_t>(moduleHandle);
	}
};

struct Environment
{
	std::unique_ptr<LoadedModule> loadedModule;
};

Environment& GetEnvironment()
{
	static Environment env;
	return env;
}

namespace Detail {
bool LoadModuleLibrary(LoadedModule& module)
{
	module.moduleHandle = LoadLibraryA(module.path.data());
	return module.moduleHandle != nullptr;
}
}

bool LoadModule(Environment& env,
				const std::string& name,
				const std::string& modulePath,
				const std::string& mapFilePath)
{
	auto moduleToLoad = std::make_unique<LoadedModule>();
	moduleToLoad->name = name;
	moduleToLoad->path = modulePath;
	moduleToLoad->symbolMap = std::make_unique<SymbolMap>();

	if (!Detail::FileExists(modulePath))
	{
		printf("Module '%s' not found\n", modulePath.data());
		return false;
	}
	if (!Detail::FileExists(mapFilePath))
	{
		printf("Map file '%s' not found\n", mapFilePath.data());
		return false;
	}
	if (!ReadMapFile(*moduleToLoad->symbolMap, mapFilePath))
	{
		printf("ReadMapFile failed\n");
		return false;
	}

	// Ensure that the hotreload directory exists.
	CreateDirectoryA("hotreload", nullptr);

	// We don't load the module directly, instead we create a temporary copy of it.
	auto now = std::chrono::system_clock::now().time_since_epoch().count();
	std::string tmpModulePath = "hotreload\\" + modulePath + "." + std::to_string(now);
	moduleToLoad->path = tmpModulePath;
	if (!CopyFileA(modulePath.data(), tmpModulePath.data(), FALSE))
	{
		printf("CopyFile failed\n");
		return false;
	}

	Detail::LoadModuleLibrary(*moduleToLoad);

	if (!env.loadedModule)
	{
		// If no previous module was loaded we are done.
		env.loadedModule = std::move(moduleToLoad);
		printf("Module %s loaded!\n", name.data());
		return true;
	}

	// Another instance of the module was already loaded and needs to be patched.
	auto moduleToReplace = env.loadedModule.get();

	// Fix up the symbols
	for (auto& symToPatch : *moduleToReplace->symbolMap)
	{
		auto symToLoad = moduleToLoad->symbolMap->FindSymbol(symToPatch->name);
		if (!symToLoad)
			continue;

		uint64_t currentSymAbsAddr =
			moduleToReplace->GetBaseAddress() + symToPatch->rva;
		uint64_t nextSymAbsAddr =
			moduleToLoad->GetBaseAddress() + symToLoad->rva;

		if (symToPatch->isFunction)
		{
			// Patch by using http://www.ragestorm.net/blogs/?p=107

			uint32_t nextSymAbsAddrLow = nextSymAbsAddr & UINT32_MAX;
			uint32_t nextSymAbsAddrHigh = (nextSymAbsAddr >> 32ULL) & UINT32_MAX;

			// The actual code takes 14 bytes but since we use 128bit CAS
			// we make it 16 bytes and fill the rest with two NOPs.
			uint8_t buf[16] = {};
			buf[0] = 0x68; // push nextSymAbsAddrLow
			*reinterpret_cast<uint32_t*>(&buf[1]) = nextSymAbsAddrLow;
			buf[5] = 0xc7; // mov dword [rsp+4], nextSymAbsAddrHigh
			buf[6] = 0x44;
			buf[7] = 0x24;
			buf[8] = 0x04;
			*reinterpret_cast<uint32_t*>(&buf[9]) = nextSymAbsAddrHigh;
			buf[13] = 0xc3;
			buf[14] = 0x90; // nop
			buf[15] = 0x90; // nop

			volatile LONG64* current = reinterpret_cast<volatile LONG64*>(currentSymAbsAddr);
			LONG64 p1 = *reinterpret_cast<LONG64*>(&buf[0]);
			LONG64 p2 = *reinterpret_cast<LONG64*>(&buf[8]);

			Detail::UnprotectPage(currentSymAbsAddr);

			while (true)
			{
				LONG64 captured[2] = { current[0], current[1] };
				if (_InterlockedCompareExchange128(
					reinterpret_cast<volatile LONG64*>(currentSymAbsAddr),
					p2, p1, captured))
					break;
			}

#if 0
			// Patch by using 5 bytes relative jmp.
			int64_t diff = static_cast<int64_t>(
				static_cast<int64_t>(nextSymAbsAddr) -
				(static_cast<int64_t>(currentSymAbsAddr) + 5)
				);

			if (diff > INT32_MAX || diff < INT32_MIN)
			{
				printf("Trampoline needed!\n");
				throw;
			}

			Detail::UnprotectPage(currentSymAbsAddr);
			uint8_t* p = reinterpret_cast<uint8_t*>(currentSymAbsAddr);
			p[0] = 0xe9;
			*reinterpret_cast<int32_t*>(&p[1]) = static_cast<int32_t>(diff);
#endif

#if DEBUG_OUT
			printf("Patching function %s  -- %llx\n", symToPatch->name.data(), currentSymAbsAddr);
#endif
		}
	}

	env.loadedModule = std::move(moduleToLoad);
	printf("Module %s reloaded!\n", name.data());
	return true;
}
}

// Export as _TestMain to make it easier to find in debug tools.
extern "C" void __declspec(dllexport) _TestMain()
{
	HotReload::Environment& env = HotReload::GetEnvironment();
	if (!HotReload::LoadModule(env, "Module1.dll", "Module1.dll", "Module1.map"))
	{
		printf("HotReload2::LoadModule failed\n");
		return;
	}
	auto fn = reinterpret_cast<TNewCalculator>(
		GetProcAddress(env.loadedModule->moduleHandle, "NewCalculator"));
	ICalculator* calc = fn();

	while (true)
	{
 		int x = calc->DoSomeStuff(1, 2);
		printf("Result: %d\n", x);
		system("pause");

		if (!HotReload::LoadModule(env, "Module1.dll", "Module1.dll", "Module1.map"))
		{
			printf("HotReload2::LoadModule failed\n");
			return;
		}
	}
}

int main()
{
	_TestMain();
    return 0;
}


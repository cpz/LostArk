#pragma once

// Lost Ark (1.61.63.0) SDK

#include <unordered_set>
#include <string>
#include <locale>

namespace SDK
{
template<typename Fn>
inline Fn GetVFunction(const void *instance, std::size_t index)
{
	auto vtable = *reinterpret_cast<const void***>(const_cast<void*>(instance));
	return reinterpret_cast<Fn>(vtable[index]);
}

template<class T>
class TArray
{
	friend class FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline size_t Num() const
	{
		return Count;
	};

	inline T& operator[](size_t i)
	{
		return Data[i];
	};

	inline const T& operator[](size_t i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(size_t i) const
	{
		return i < Num();
	}

	inline T& GetByIndex(size_t i)
	{
		return Data[i];
	}

	inline const T& GetByIndex(size_t i) const
	{
		return Data[i];
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

class FString : public TArray<wchar_t>
{
public:
	inline FString()
	{
	}

	FString(const wchar_t* wsz_string)
	{
		Max = Count = static_cast<std::int32_t>(*wsz_string ? std::wcslen(wsz_string) + 1 : 0);

		if (Count)
			Data = const_cast<wchar_t*>(wsz_string);
	};

	inline FString& operator = (const wchar_t* wsz_string)
	{
		Max = Count = static_cast<std::int32_t>(*wsz_string ? std::wcslen(wsz_string) + 1 : 0);

		if (Count)
			Data = const_cast<wchar_t*>(wsz_string);
		
		return *this;
	}
	

	/*inline FString& operator = (FString&& other) noexcept
	{
		*this = std::move(other);
		
		return *this;
	}*/
	
	
	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		const auto size = std::wcslen(Data);
		auto string = std::string(size, '\0');

		std::use_facet< std::ctype< wchar_t > >(std::locale()).narrow(Data, Data + size, '?', &string[0]);
		return string;
	}
};

template<class TEnum>
class TEnumAsByte
{
public:
	inline TEnumAsByte()
	{
	}

	inline TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	inline operator TEnum() const
	{
		return (TEnum)value;
	}

	inline TEnum GetValue() const
	{
		return (TEnum)value;
	}

private:
	uint8_t value;
};

class FNameEntry
{
public:
	char UnknownData00[0x14]; //0x000A
	char name[0x14]; //0x0014 

	std::string GetName() const
	{
		return name;
	}
};

struct FName
{
	int32_t Index;
	int32_t Number;

	inline FName()
		: Index(0),
		  Number(0)
	{
	};

	inline FName(int32_t i)
		: Index(i),
		  Number(0)
	{
	};

	FName(const char* nameToFind)
		: Index(0),
		  Number(0)
	{
		static std::unordered_set<size_t> cache;

		for (auto i : cache)
		{
			if (GetGlobalNames()[i]->GetName() == nameToFind)
			{
				Index = i;
				
				return;
			}
		}

		for (auto i = 0u; i < GetGlobalNames().Num(); ++i)
		{
			if (GetGlobalNames()[i] != nullptr)
			{
				if (GetGlobalNames()[i]->GetName() == nameToFind)
				{
					cache.insert(i);

					Index = i;

					return;
				}
			}
		}
	};

	static TArray<FNameEntry*>* GNames;
	static inline TArray<FNameEntry*>& GetGlobalNames()
	{
		return *GNames;
	};

	inline std::string GetName() const
	{
		return GetGlobalNames()[Index]->GetName();
	};

	inline bool operator==(const FName& other) const
	{
		return Index == other.Index;
	};
};

class UObject;

class FScriptInterface
{
private:
	UObject* ObjectPointer;
	void* InterfacePointer;

public:
	inline UObject* GetObject() const
	{
		return ObjectPointer;
	}

	inline UObject*& GetObjectRef()
	{
		return ObjectPointer;
	}

	inline void* GetInterface() const
	{
		return ObjectPointer != nullptr ? InterfacePointer : nullptr;
	}
};

template<class InterfaceType>
class TScriptInterface : public FScriptInterface
{
public:
	inline InterfaceType* operator->() const
	{
		return (InterfaceType*)GetInterface();
	}

	inline InterfaceType& operator*() const
	{
		return *((InterfaceType*)GetInterface());
	}

	inline operator bool() const
	{
		return GetInterface() != nullptr;
	}
};

struct FScriptDelegate
{
	class UObject*		Object;
	struct FName		FunctionName;
};

bool ObjectInitialize();
bool NamesInitialize();
bool WorldInitialize();
bool EngineInitialize();
}

#ifdef _MSC_VER
	#pragma pack(pop)
#endif

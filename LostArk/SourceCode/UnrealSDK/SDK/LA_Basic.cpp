// Lost Ark (1.61.63.0) SDK

#ifdef _MSC_VER
#pragma pack(push, 0x4)
#endif

#include "../SDK.hpp"
#include "../../Utils/FindPattern.h"

namespace SDK
{
	TArray<FNameEntry*>*    FName::GNames = nullptr;
	TArray<UObject*>*       UObject::GObjects = nullptr;
	UWorld**                g_world = nullptr;
	UEFGameEngine**         g_engine = nullptr;

	/// UWorld and GameEngine can be found in UGameEngine::CommitMapChange
	/// UObject can be found with string "Dependencies of %s:"
	/// FName can be found with string "ByteProperty" (above none)
	bool ObjectInitialize()
	{
		BYTE* g_objects = nullptr;

		auto __m_objects = static_cast<BYTE*> ( PatternScan ("EFEngine.dll", 
			                                                 "48 8B ? ? ? ? ? 48 8B 34 C8 49 8B D6 48 8B CE" ) );
		if (__m_objects == nullptr)
		{
			return false;
		}

		g_objects = reinterpret_cast<BYTE*>( *reinterpret_cast<DWORD*>(__m_objects + 0x03 ) );
		g_objects += reinterpret_cast<unsigned __int64>(__m_objects) + 0x03 + sizeof(DWORD);

		UObject::GObjects = reinterpret_cast<decltype(UObject::GObjects)>(g_objects);
		if (UObject::GObjects == nullptr)
			return false;

		return true;
	}

	bool NamesInitialize()
	{
		BYTE* g_names = nullptr;

		auto __m_names = static_cast<BYTE*>( PatternScan( "EFEngine.dll",
		                                                 "48 89 ? ? ? ? ? 48 89 ? ? ? ? ? 48 8B ? ? ? ? ? 48 2B C1 48 83 F8 19" ) );
		if (__m_names == nullptr)
		{
			return false;
		}

		g_names = reinterpret_cast<BYTE*>( *reinterpret_cast<DWORD*> (__m_names + 0x03 ) );
		g_names += reinterpret_cast<unsigned __int64>( __m_names ) + 0x03 + sizeof( DWORD );

		FName::GNames = reinterpret_cast<decltype(FName::GNames)>(g_names);
		if (FName::GNames == nullptr)
			return false;

		return true;
	}

	bool WorldInitialize()
	{
		BYTE* m_world = nullptr;

		//Offset = 0x00007FF9E26AF7F8
		auto __m_world = static_cast<BYTE*>( PatternScan("EFEngine.dll", 
			                                            "48 8B ? ? ? ? ? 8B 78 68 FF CF" ) );
		if (__m_world == nullptr)
		{
			return false;
		}

		m_world = reinterpret_cast<BYTE*>( *reinterpret_cast<DWORD*> (__m_world + 0x03 ) );
		m_world += reinterpret_cast<unsigned __int64>( __m_world ) + 0x03 + sizeof( DWORD );


		g_world = reinterpret_cast<UWorld**>(m_world);
		if (g_world == nullptr)
			return false;

		return true;
	}

	bool EngineInitialize()
	{
		BYTE* m_engine = nullptr;

		//Offset = 0x00007FF9E26AB4F8
		auto __m_engine = static_cast<BYTE*>( PatternScan("EFEngine.dll", 
		                                                  "48 8B ? ? ? ? ? 48 8B 88 2C 07 00 00 48 8B 01" ) );

		if (__m_engine == nullptr)
			return false;

		m_engine = reinterpret_cast<BYTE*>( *reinterpret_cast<DWORD*>( __m_engine + 0x03 ) );
		m_engine += reinterpret_cast<unsigned __int64>( __m_engine ) + 0x03 + sizeof( DWORD );

		g_engine = reinterpret_cast<UEFGameEngine**>(m_engine);

		if (g_engine == nullptr)
			return false;

		return true;
	}
}

#ifdef _MSC_VER
#pragma pack(pop)
#endif

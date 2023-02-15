#ifndef VMETHOD_H
#define VMETHOD_H

inline void**& GetVTable(void* instance)
{
	return *reinterpret_cast<void***>((size_t)instance);
}

inline const void** GetVTable(const void* instance)
{
	return *reinterpret_cast<const void***>((size_t)instance);
}

template<typename T>
inline T GetMethod(const void* instance, size_t index)
{
	return reinterpret_cast<T> (GetVTable(instance)[index]);
}

inline uintptr_t get_function(uintptr_t vtable, int index)
{
	return *(uintptr_t*)(*(uintptr_t*)(vtable)+index * 4);
}
#endif
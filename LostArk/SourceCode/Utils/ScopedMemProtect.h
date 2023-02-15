#pragma once
#include <cstdint>
#include <memory>

namespace Memory
{
	class ScopedMemProtect
	{
	public:
		template< typename T >
		ScopedMemProtect( T tAddress, std::size_t nSize, DWORD dwProtect )
			:	m_pAddress( ( void* )tAddress ),
			m_nSize( nSize ),
			m_dwProtect( NULL )
		{
			VirtualProtect( m_pAddress, m_nSize, dwProtect, &m_dwProtect );
		}

		~ScopedMemProtect()
		{
			VirtualProtect( m_pAddress, m_nSize, m_dwProtect, &m_dwProtect );
		}

	private:
		void*		m_pAddress;
		std::size_t	m_nSize;
		DWORD		m_dwProtect;
	};
}
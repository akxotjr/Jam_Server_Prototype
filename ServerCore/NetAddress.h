#pragma once

namespace core::network
{
	/*--------------
		NetAddress
	---------------*/

	class NetAddress
	{
	public:
		NetAddress() = default;
		NetAddress(SOCKADDR_IN sockAddr);
		NetAddress(wstring ip, uint16 port);

		SOCKADDR_IN&		GetSockAddr() { return _sockAddr; }
		const SOCKADDR_IN&	GetSockAddr() const { return _sockAddr; }
		wstring				GetIpAddress() const;
		uint16				GetPort() const { return ::ntohs(_sockAddr.sin_port); }

		bool				IsValid() const;

	public:
		static IN_ADDR		Ip2Address(const WCHAR* ip);

		bool				operator==(const NetAddress& other) const
		{
			return _sockAddr.sin_addr.S_un.S_addr == other._sockAddr.sin_addr.S_un.S_addr &&
				_sockAddr.sin_port == other._sockAddr.sin_port;
		}

	public:
		SOCKADDR_IN			_sockAddr = {};
	};


}

namespace std
{
	template <>
	struct hash<core::network::NetAddress>
	{
		size_t operator()(const core::network::NetAddress& addr) const
		{
			return hash<uint32_t>()(addr.GetSockAddr().sin_addr.S_un.S_addr) ^
				(hash<uint16_t>()(addr.GetSockAddr().sin_port) << 1);
		}
	};
}
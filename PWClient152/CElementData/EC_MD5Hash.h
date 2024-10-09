#pragma once

namespace PATCH
{
	class MD5Hash{
		unsigned int state[4];
		unsigned int count[2];
		unsigned char buffer[64];
		unsigned char digest[16];
	
		void transform (const unsigned char block[64]);
		void init();
	public:
		MD5Hash();
		void Update(const char *input, unsigned int inputlen);
		const unsigned char* Final();
		bool Equal(const char* asciidigest);
		bool Equal(MD5Hash& hash);
		
		const unsigned char* GetDigest() const { return digest; }
		char* GetString(char* buffer, unsigned int& length);
		int Length() { return 16; }
	};

}
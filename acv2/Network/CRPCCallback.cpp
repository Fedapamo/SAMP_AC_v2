#include "CRPCCallback.h"
#include "../CLog.h"
#include "../CLoader.h"
#include "../Misc.h"
#include "Network.h"
#include "../Misc.h"

#include <Boost\thread.hpp>


void CRPCCallback::Initialize()
{
	CRPC::Add(MD5_MEMORY_REGION, MD5_Memory_Region);
	CRPC::Add(MD5_FILE, MD5_File);

	boost::thread ResendFilesThread(&ResendFileInformation);
}

void CRPCCallback::ResendFileInformation()
{
	// Send the server the processes and modules that were loaded while we weren't connected.
	CLoader::Processes.ResendFiles();
	CLoader::Modules.ResendFiles();
}

void CRPCCallback::MD5_Memory_Region(RakNet::BitStream &bsData, int iExtra)
{
	// Create variables to store the info the server sent.
	int address, size;

	if (bsData.Read(address) && bsData.Read(size))
	{
		// Calculate the result of the MD5 hash at the address and put it in an std::string
		std::string md5 = Misc::MD5_Memory(address, size);

		// Send the result of the hash back to the server.
		RakNet::BitStream bitStream;
		bitStream.Write(address);
		bitStream.Write(size);
		bitStream.Write(md5.c_str());

		Network::SendRPC(ON_MD5_CALCULATED, &bitStream);
	}
}

void CRPCCallback::MD5_File(RakNet::BitStream &bsData, int iExtra)
{
	unsigned char file[MAX_PATH+1];

	// Read data sent to us by the server (which in this case is the file name)
	if (bsData.Read(file))
	{
		// MD5 that file and store the result.
		std::string result = CLoader::GtaDirectory.MD5_Specific_File((char*)file);

		// Convert the file into a std::string, so we can remove any macros that were sent in the file name.
		std::string szFile(reinterpret_cast<char*>(file));
		
		// Find the occurand of $(GtaDirectory) macro.
		int i = szFile.find("$(GtaDirectory)/");

		RakNet::BitStream bsData;
		
		// Cut out the $(GtaDirectory) macro when we send it back to the server.
		bsData.Write(szFile.substr(i+16).c_str());

		// and of course, send the MD5 as a const char*
		bsData.Write(result.c_str());

		// Call the RPC.
		Network::SendRPC(ON_FILE_CALCULATED, &bsData);
	}
}
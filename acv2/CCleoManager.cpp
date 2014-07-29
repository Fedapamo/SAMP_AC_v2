#include "CCleoManager.h"
#include "CPatch.h"
#include "Addresses.h"
#include "CMem.h"
#include <Windows.h>

static DWORD LoadScriptsJmpBack = 0x05DE687;

void CCleoManager::Load()
{
	DWORD dwOldProt;

	// Prevent CLEO 4 from loading scripts
	VirtualProtect(FUNC_Init_SCM1, 5, PAGE_EXECUTE_READWRITE, &dwOldProt);
	memcpy(FUNC_Init_SCM1, "\xE8\x74\xCF\xF2\xFF", 5);

	VirtualProtect(FUNC_Init_SCM2, 5, PAGE_EXECUTE_READWRITE, &dwOldProt);
	memcpy(FUNC_Init_SCM2, "\xE8\x0B\xEA\xEA\xFF", 5);

	// Prevent CLEO 3's loading functions
	VirtualProtect((void*)FUNC_Scripts_Init1, 5, PAGE_EXECUTE_READWRITE, &dwOldProt);
	CPatch::RedirectCall(FUNC_Scripts_Init1, LoadScripts);

	VirtualProtect((void*)FUNC_Scripts_Init2, 5, PAGE_EXECUTE_READWRITE, &dwOldProt);
	CPatch::RedirectCall(FUNC_Scripts_Init2, LoadScripts);
}

HOOK CCleoManager::LoadScripts()
{
	__asm
	{
		xor eax, eax
		add ecx, 16h
		jmp[LoadScriptsJmpBack]
	}
}
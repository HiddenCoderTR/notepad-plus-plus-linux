#include "verifySignedfile.h"

SecurityMode SecurityGuard::_securityMode = sm_sha256;

SecurityGuard::SecurityGuard() {}

bool SecurityGuard::checkModule(const std::wstring&, NppModule)
{
	return true;
}

bool SecurityGuard::checkSha256(const std::wstring&, NppModule) const
{
	return true;
}

bool SecurityGuard::verifySignedBinary(const std::wstring&) const
{
	return true;
}

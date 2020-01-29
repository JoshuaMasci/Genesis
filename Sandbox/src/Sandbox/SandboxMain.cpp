#pragma once

#include "Sandbox/SandboxApplication.hpp"

int main(int argc, char** argv)
{
	GENESIS_ENGINE_DEBUG_START();

	Genesis::Logging::inti_client_logging("Sandbox", "");
	GENESIS_INFO("Sandbox Started");

	GENESIS_PROFILE_BLOCK_START("Sandbox_Load");
	//ALPHA
	SandboxApplication* sandbox = new SandboxApplication();
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_PROFILE_BLOCK_START("Sandbox_Loop");
	sandbox->run();
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_PROFILE_BLOCK_START("Sandbox_Exit");
	delete sandbox;
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_INFO("Sandbox Closed");

	GENESIS_PROFILE_WRITE_TO_FILE("Genesis_profile.prof");

	//Wait till enter
	getchar();

	return 0;
	//OMEGA
}
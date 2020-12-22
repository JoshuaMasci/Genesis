#pragma once

#include "Sandbox/SandboxApplication.hpp"

int main(int argc, char** argv)
{
	GENESIS_PROFILE_START(); 
	Genesis::Logging::inti_engine_logging();
	Genesis::Logging::inti_client_logging("Sandbox");

	//ALPHA
	Genesis::SandboxApplication* sandbox = new Genesis::SandboxApplication();
	GENESIS_INFO("Sandbox Started");

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
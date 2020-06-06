#pragma once

#include "Sandbox/SandboxApplication.hpp"

int main(int argc, char** argv)
{
	GENESIS_PROFILE_START(); 
	Genesis::Logging::inti_engine_logging("");
	Genesis::Logging::inti_client_logging("Sandbox", "");

	Genesis::TransformD transform_a = Genesis::TransformD(Genesis::vector3D(10.0, 2.0, -5.0), glm::angleAxis(glm::radians(45.0), glm::normalize(Genesis::vector3D(1.0, 1.0, 1.0))), Genesis::vector3D(6.0, 1.0, 0.5));
	Genesis::TransformD transform_b = Genesis::TransformD(Genesis::vector3D(1.2, 2.2, 3.2), glm::angleAxis(glm::radians(45.0), glm::normalize(Genesis::vector3D(0.0, 1.0, 0.0))), Genesis::vector3D(2.1));
	Genesis::TransformD transform_c = Genesis::TransformD();
	Genesis::TransformD transform_d = Genesis::TransformD();
	Genesis::TransformUtils::transformByInplace(transform_c, transform_a, transform_b);
	Genesis::TransformUtils::untransformByInplace(transform_d, transform_a, transform_c);
	//ALPHA
	SandboxApplication* sandbox = new SandboxApplication();
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
	//getchar();

	return 0;
	//OMEGA
}
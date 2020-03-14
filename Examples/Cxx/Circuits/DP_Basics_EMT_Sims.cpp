/**
 * @copyright 2017, Institute for Automation of Complex Power Systems, EONERC
 *
 * DPsim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************/

#include <DPsim.h>

using namespace DPsim;
using namespace CPS::EMT;
using namespace CPS::EMT::Ph1;

void EMT_VS_RL_f60_largeTs();
void EMT_VS_RL_f60();
void EMT_VS_RL_f500();

int main(int argc, char* argv[]) {
	EMT_VS_RL_f60_largeTs();
	EMT_VS_RL_f60();
	EMT_VS_RL_f500();
}

void EMT_VS_RL_f60_largeTs() {
	Real timeStep = 0.01;
	Real finalTime = 0.2;
	String simName = "EMT_VS_RL_f60_largeTs";
	Logger::setLogDir("logs/"+simName);

	// Nodes
	auto n1 = SimNode::make("n1");
	auto n2 = SimNode::make("n2");
	auto n3 = SimNode::make("n3");

	// Components
	auto vs = VoltageSource::make("vs");
	vs->setParameters(Complex(230, 0), 60);
	auto rline = Resistor::make("r_line");
	rline->setParameters(1);
	auto lline = Inductor::make("l_line");
	lline->setParameters(0.02);
	auto rload = Resistor::make("r_load");
	rload->setParameters(10);

	// Connections
	vs->connect({ SimNode::GND, n1 });
	rline->connect({ n1, n2 });
	lline->connect({ n2, n3 });
	rload->connect({ n3, SimNode::GND });

	// Define system topology
	auto sys = SystemTopology(50,
		SystemNodeList{n1, n2, n3},
		SystemComponentList{vs, rline, lline, rload});

	// Logger
	auto logger = DataLogger::make(simName);
	logger->addAttribute("v1", n1->attributeMatrixReal("v"));
	logger->addAttribute("v2", n2->attributeMatrixReal("v"));
	logger->addAttribute("v3", n3->attributeMatrixReal("v"));
	logger->addAttribute("i_line", rline->attributeMatrixReal("i_intf"));

	Simulation sim(simName, Logger::Level::info);
	sim.setSystem(sys);
	sim.setTimeStep(timeStep);
	sim.setFinalTime(finalTime);
	sim.setDomain(Domain::EMT);
	sim.initialize();

	sim.addLogger(logger);

	sim.run();
}

void EMT_VS_RL_f60() {
	Real timeStep = 0.0001;
	Real finalTime = 0.2;
	String simName = "EMT_VS_RL_f60";
	Logger::setLogDir("logs/"+simName);

	// Nodes
	auto n1 = SimNode::make("n1");
	auto n2 = SimNode::make("n2");
	auto n3 = SimNode::make("n3");

	// Components
	auto vs = VoltageSource::make("vs");
	vs->setParameters(Complex(230, 0), 60);
	auto rline = Resistor::make("r_line");
	rline->setParameters(1);
	auto lline = Inductor::make("l_line");
	lline->setParameters(0.02);
	auto rload = Resistor::make("r_load");
	rload->setParameters(10);

	// Connections
	vs->connect({ SimNode::GND, n1 });
	rline->connect({ n1, n2 });
	lline->connect({ n2, n3 });
	rload->connect({ n3, SimNode::GND });

	// Define system topology
	auto sys = SystemTopology(50,
		SystemNodeList{n1, n2, n3},
		SystemComponentList{vs, rline, lline, rload});

	// Logger
	auto logger = DataLogger::make(simName);
	logger->addAttribute("v1", n1->attributeMatrixReal("v"));
	logger->addAttribute("v2", n2->attributeMatrixReal("v"));
	logger->addAttribute("v3", n3->attributeMatrixReal("v"));
	logger->addAttribute("i_line", rline->attributeMatrixReal("i_intf"));

	Simulation sim(simName, Logger::Level::info);
	sim.setSystem(sys);
	sim.setTimeStep(timeStep);
	sim.setFinalTime(finalTime);
	sim.setDomain(Domain::EMT);
	sim.initialize();

	sim.addLogger(logger);

	sim.run();
}

void EMT_VS_RL_f500() {
	Real timeStep = 0.00001;
	Real finalTime = 0.2;
	String simName = "EMT_VS_RL_f500";
	Logger::setLogDir("logs/"+simName);

	// Nodes
	auto n1 = SimNode::make("n1");
	auto n2 = SimNode::make("n2");
	auto n3 = SimNode::make("n3");

	// Components
	auto vs = VoltageSource::make("vs");
	vs->setParameters(Complex(230, 0), 500);
	auto rline = Resistor::make("r_line");
	rline->setParameters(1);
	auto lline = Inductor::make("l_line");
	lline->setParameters(0.02);
	auto rload = Resistor::make("r_load");
	rload->setParameters(10);

	// Connections
	vs->connect({ SimNode::GND, n1 });
	rline->connect({ n1, n2 });
	lline->connect({ n2, n3 });
	rload->connect({ n3, SimNode::GND });

	// Define system topology
	auto sys = SystemTopology(50,
		SystemNodeList{n1, n2, n3},
		SystemComponentList{vs, rline, lline, rload});

	// Logger
	auto logger = DataLogger::make(simName);
	logger->addAttribute("v1", n1->attributeMatrixReal("v"));
	logger->addAttribute("v2", n2->attributeMatrixReal("v"));
	logger->addAttribute("v3", n3->attributeMatrixReal("v"));
	logger->addAttribute("i_line", rline->attributeMatrixReal("i_intf"));

	Simulation sim(simName, Logger::Level::info);
	sim.setSystem(sys);
	sim.setTimeStep(timeStep);
	sim.setFinalTime(finalTime);
	sim.setDomain(Domain::EMT);
	sim.initialize();

	sim.addLogger(logger);

	sim.run();
}



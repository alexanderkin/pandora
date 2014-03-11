
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Pandora Sequential Unit Tests"

#include "TestAgent.hxx"
#include "TestWorld.hxx"
#include <Simulation.hxx>
#include <World.hxx>
#include <Point2D.hxx>

#include <boost/test/unit_test.hpp>

namespace Test
{

BOOST_AUTO_TEST_SUITE( PandoraBasicUse )

BOOST_AUTO_TEST_CASE( testEqualityPoint ) 
{
	Engine::Point2D<int> point1(4,5);
	Engine::Point2D<int> point2(2,5);

	BOOST_CHECK(point1!=point2);
	point2._x = 4;
	BOOST_CHECK_EQUAL(point1, point2);
	point2._y = 100;
	BOOST_CHECK(point1!=point2);
}

BOOST_AUTO_TEST_CASE( testSimulationSize ) 
{	
	Engine::Simulation mySimulation(Engine::Point2D<int>(10,10), 1);
	Engine::Point2D<int> size(9,10);
	
	BOOST_CHECK(size!=mySimulation.getSize());
	size._x = 10;
	BOOST_CHECK(size==mySimulation.getSize());
}

BOOST_AUTO_TEST_CASE( testExecuteWorldTwice ) 
{	
	Engine::Simulation mySimulation(Engine::Point2D<int>(10,10), 1);
	TestWorld myWorld(mySimulation);
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);
	myWorld.run();

	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);
	myWorld.run();
}

BOOST_AUTO_TEST_CASE( testAgentRemovedIsNotExecuted ) 
{	
	Engine::Simulation mySimulation(Engine::Point2D<int>(10,10), 1);
	TestWorld myWorld(mySimulation);
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);

	TestAgent * myAgent = new TestAgent("agent_0");
	myWorld.addAgent(myAgent);
	myAgent->setRandomPosition();
	BOOST_CHECK_EQUAL(myAgent->exists(), true);
	myAgent->remove();
	BOOST_CHECK_EQUAL(myAgent->exists(), false);
	myWorld.run();
}

BOOST_AUTO_TEST_CASE( testAgentRemovedIsNotInInsideNeighbours ) 
{
	Engine::Simulation mySimulation(Engine::Point2D<int>(10,10), 1);
	TestWorld myWorld(mySimulation);
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);

	TestAgent * myAgent0 = new TestAgent("agent_0");
	TestAgent * myAgent1 = new TestAgent("agent_1");
	TestAgent * myAgent2 = new TestAgent("agent_2");
	myWorld.addAgent(myAgent0);
	myWorld.addAgent(myAgent1);
	myWorld.addAgent(myAgent2);
	myAgent0->setRandomPosition();
	myAgent1->setRandomPosition();
	myAgent2->setRandomPosition();

	Engine::World::AgentsVector neighbours = myWorld.getNeighbours(myAgent0, 20);
	BOOST_CHECK_EQUAL(neighbours.size(), 2);
	myAgent1->remove();
	myAgent2->remove();
	Engine::World::AgentsVector neighbours2 = myWorld.getNeighbours(myAgent0, 20);
	BOOST_CHECK_EQUAL(neighbours2.size(), 0);

}

BOOST_AUTO_TEST_SUITE_END()

} // namespace Test


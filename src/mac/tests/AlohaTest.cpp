/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/mac/Aloha.hpp>

#include <WNS/ldk/tests/FUTestBase.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>

namespace glue { namespace mac { namespace tests {

	class AlohaTest:
		public wns::ldk::tests::FUTestBase
	{
		CPPUNIT_TEST_SUITE(AlohaTest);
		CPPUNIT_TEST ( testBackoff );
		CPPUNIT_TEST_SUITE_END();

		virtual void
		setUpTestFUs()
		{
			wns::pyconfig::Parser parser;
			parser.loadString(
					  "from glue.Glue import Aloha\n"
					  "aloha = Aloha(commandName = 'he', maximumWaitingTime = 0.01)\n");
			this->testee = new Aloha(this->getFUN(), parser.get("aloha"));
		}

		virtual void
		tearDownTestFUs()
		{
			this->testee = NULL;
		}

		virtual wns::ldk::FunctionalUnit*
		getUpperTestFU() const
		{
			return this->testee;
		}

		virtual wns::ldk::FunctionalUnit*
		getLowerTestFU() const
		{
			return this->testee;
		}

		void
		testBackoff()
		{
			wns::ldk::tools::Stub* lower = this->getLowerStub();
			wns::ldk::tools::Stub* upper = this->getUpperStub();
			lower->setStepping(false);
			upper->setStepping(false);
			wns::Average<double> average;
			for(int ii = 0; ii < 7500; ++ii)
			{
				wns::ldk::CompoundPtr compound = this->newFakeCompound();
				this->sendCompound(compound);
				wns::simulator::getEventScheduler()->processOneEvent();
				average.put(this->getSojurnTime(compound));
			}
			CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(7500), lower->sent.size() );
			WNS_ASSERT_MAX_REL_ERROR(0.005, average.get(), 1E-2);
		}

	private:
		Aloha* testee;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( AlohaTest );

} // tests
} // mac
} // glue

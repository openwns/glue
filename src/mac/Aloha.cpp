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
#include <WNS/container/UntypedRegistry.hpp>
#include <WNS/events/MemberFunction.hpp>
#include <WNS/simulator/ISimulator.hpp>

using namespace glue;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Aloha,
	wns::ldk::FunctionalUnit,
	"glue.mac.Aloha",
	wns::ldk::FUNConfigCreator);


Aloha::Aloha(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
	wns::ldk::CommandTypeSpecifier<AlohaCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<Aloha>(),
	logger(config.get("logger")),
	compound(),
	maxWait(config.get<wns::simulator::Time>("maximumWaitingTime")),
	uniform(0.0, 1.0, wns::simulator::getRNG())
{
	MESSAGE_SINGLE(NORMAL, this->logger, "created")
}


Aloha::~Aloha()
{

}


bool
Aloha::doIsAccepting(const wns::ldk::CompoundPtr& /*_compound*/) const
{
	if (this->compound == wns::ldk::CompoundPtr())
	{
		// there is room for exactly one compound
		return true;
	}
	else
	{
		return false;
	}
}


void
Aloha::doSendData(const wns::ldk::CompoundPtr& _compound)
{
	assure(this->compound == wns::ldk::CompoundPtr(), "already got a compound!");
	assure(_compound != wns::ldk::CompoundPtr(), "Invalid compound (NULL)");
	this->compound = _compound;
	// delay by random backoff
	wns::simulator::Time randomBackoff = uniform() * this->maxWait;
	MESSAGE_SINGLE(
		NORMAL,
		this->logger,
		"Compound ("<< *(this->compound) <<") will be sent in " << randomBackoff << " seconds");
	wns::events::MemberFunction<Aloha> ev (this, &Aloha::sendCompoundAfterElapsedBackoff);
	wns::simulator::getEventScheduler()->scheduleDelay(ev, randomBackoff);
}

void
Aloha::doWakeup()
{
	// simply forward the wakeup call
	this->getReceptor()->wakeup();
}


void
Aloha::doOnData(const wns::ldk::CompoundPtr& compound)
{
	this->getDeliverer()->getAcceptor(compound)->onData(compound);
}


void
Aloha::sendCompoundAfterElapsedBackoff()
{
	assure(this->getConnector()->hasAcceptor(compound) == true, "Not able to send data. Aloha may only be used over NON-BLOCKING PHYs");
	MESSAGE_SINGLE(NORMAL, this->logger, "Sending compound after elapsed backoff:" << *(this->compound));
	this->getConnector()->getAcceptor(compound)->sendData(compound);
	this->compound = wns::ldk::CompoundPtr();
	// wakeup: we are ready to receive another compound
	this->getReceptor()->wakeup();
}

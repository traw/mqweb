/*
 * Copyright 2010 MQWeb - Franky Braem
 *
 * Licensed under the EUPL, Version 1.1 or � as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * http://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef _MQWeb_AuthorityServiceController_h
#define _MQWeb_AuthorityServiceController_h

#include "MQ/Web/MQController.h"
#include "MQ/Web/MapInitializer.h"

namespace MQ {
namespace Web {

class AuthorityServiceController : public MQController
	/// Controller that shows the status of queues
{
public:
	AuthorityServiceController();
		/// Constructor

	virtual ~AuthorityServiceController();
		/// Destructor

	virtual const std::map<std::string, Controller::ActionFn>& getActions() const;
		/// Returns all available actions

	void inquire();
		/// See: http://www.mqweb.org/api/authservice.html#inquire

private:
};


inline const Controller::ActionMap& AuthorityServiceController::getActions() const
{
	static Controller::ActionMap actions 
		= MapInitializer<std::string, Controller::ActionFn>
			("inquire", static_cast<ActionFn>(&AuthorityServiceController::inquire))
		;
	return actions;
}


} } // Namespace MQ::Web

#endif // _MQWeb_AuthorityServiceController_h

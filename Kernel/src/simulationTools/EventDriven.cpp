/* Siconos-Kernel version 1.2.0, Copyright INRIA 2005-2006.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
*/

#include "EventDriven.h"
using namespace std;

// --- Default constructor ---
EventDriven::EventDriven(Model* newModel): Strategy(newModel, "EventDriven")
{}

// --- From Model ---
EventDriven::EventDriven(Model& newModel): Strategy(newModel, "EventDriven")
{}

// --- XML constructor ---
EventDriven::EventDriven(StrategyXML* strxml, Model *newModel): Strategy(strxml, newModel, "EventDriven")
{}

// --- Destructor ---
EventDriven::~EventDriven()
{}

void EventDriven::setEventsManagerPtr(EventsManager*)
{
  // TODO IF NECESSARY?
}

void EventDriven::initialize()
{
  Strategy::initialize();
  eventsManager = new EventsManager(DEFAULT_TICK, this); //
  eventsManager->initialize();
}

// Run the whole simulation
void EventDriven::run()
{

  unsigned int count = 0; // events counter.
  // do simulation while events remains in the "future events" list of events manager.
  cout << " ==== Start of Event Driven simulation - This may take a while ... ====" << endl;
  while (eventsManager->hasNextEvent())
  {
    // Integrate system between "current" and "next" event of events manager.
    advanceToEvent();
    // update events
    eventsManager->processEvents();
    count++;
  }
  cout << "===== End of Event Driven simulation. " << count << " events have been processed. ==== " << endl;
}

void EventDriven::computeOneStep()
{
  cout << "EventDriven, compute One Step, not yet implemented." << endl;
  //  advanceToEvent();
  // update events
  //eventsManager->processEvents();
}

void EventDriven::advanceToEvent()
{
  // Get double value of init time (currentEvent time) and final time (nextEvent time)

  double tinit = eventsManager->getCurrentTime();
  double tend =  eventsManager->getNextTime();
  double tout, ttmp = tend;
  bool isNewEventOccur = false;  // set to true if a new event occur during integration
  // call integrate method of each OSI, between tinit and tend.
  OSIIterator it;
  for (it = allOSI.begin(); it != allOSI.end(); ++it)
  {
    bool iout = false;
    (*it)->integrate(tinit, tend, ttmp, iout); // integrate must return a flag telling if tend has been reached or not.
    // If not, ttmp is the real reached time.
    if (!iout)
    {
      tout = min(tout, ttmp);
      isNewEventOccur = true;
    }

    // Update DS state. Here?
    //(*it)->getDynamicalSystemPtr()->computeRhs(tend);

  }

  if (isNewEventOccur)
  {
    // Add an event into the events manager list
    bool isScheduleOk = eventsManager->scheduleEvent("NonSmoothEvent", tout);
    if (!isScheduleOk) cout << " EventDriven advanceToEvent warning: try to add an already existing event" << endl;
    else
    {
      bool iout;
      // restart integration from tinit to tout
      for (it = allOSI.begin(); it != allOSI.end(); ++it)
      {
        // Add something to "reset" the OSI state as it was a tinit.

        iout = false;
        (*it)->integrate(tinit, tout, ttmp, iout); // integrate must return a flag telling if tend has been reached or not.
        // If not, ttmp is the real reached time.
        if (!iout)
          RuntimeException::selfThrow("EventDriven advanceToEvent, event management problem.");
      }
    }
  }

}

EventDriven* EventDriven::convert(Strategy *str)
{
  EventDriven* ed = dynamic_cast<EventDriven*>(str);
  return ed;
}


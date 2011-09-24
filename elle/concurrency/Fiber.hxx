//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue mar 23 14:55:13 2010]
//

#ifndef ELLE_CONCURRENCY_FIBER_HXX
#define ELLE_CONCURRENCY_FIBER_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method wraps a fiber entry point setting the fiber's state
    /// in order to distinguish the thread returning from ::getcontext()
    /// and jumping back.
    ///
    template <typename... T>
    Void		Fiber::Launch(Closure<Status, T...>*	closure)
    {
      enter();

      log_here;
      // trigger the closure and, should there are errors, display them.
      if (closure->Call() == StatusError)
	yield(_(), "an error occured in the fiber");
      log_here;

      // set the fiber state.
      Fiber::Current->state = Fiber::StateCompleted;

      // remove the parent fiber from the container since it is going
      // to be scheduled as soon as this function returns.
      if (Fiber::Remove(Fiber::Current->link) == StatusError)
	yield(_(), "unable to remove the fiber");

      // set the state of the parent's fiber as awaken.
      Fiber::Current->link->state = Fiber::StateAwaken;

      log_here;
      // swap to the link as we don't rely on uc_link
      ::swapcontext(&Fiber::Current->context, &Fiber::Current->link->context);
      log_here;

      // release the resources.
      release();
      fail("this code should never be reached");
    }

    ///
    /// this method spawns a fiber.
    ///
    template <typename... T>
    Status		Fiber::Spawn(Closure<Status, T...>&	closure)
    {
      enter();

      // get the current context in order to resume execution for this point
      // in the future.
      if (::getcontext(&Fiber::Current->context) == -1)
	escape("unable to get the context");

      log_here;
      Fiber::Current->Dump();

      // if we are in the fiber spawning a new fiber...
      if (Fiber::Current->state == Fiber::StateActive)
	{
	  // declare a launch function pointer in order to bypass the type
	  // checking system through casts.
	  Void		(*launch)(Closure<Status, T...>*) = &Fiber::Launch;
	  Fiber*	fiber;

	  // set the current fiber as suspended.
	  Fiber::Current->state = Fiber::StateSuspended;

	  // the current fiber is waiting for its child fiber to complete.
	  Fiber::Current->type = Fiber::TypeFiber;

	  // save the environment.
	  if (Fiber::Trigger(PhaseSave) == StatusError)
	    escape("unable to save the environment");

	  // add the current fiber to the container.
	  if (Fiber::Add(Fiber::Current) == StatusError)
	    escape("unable to add the fiber to the container");

	  // allocate a new fiber.
	  if (Fiber::New(fiber) == StatusError)
	    escape("unable to allocate a new fiber");

	  // set the parent fiber.
	  fiber->link = Fiber::Current;

	  // get the context in order to create a new one.
	  if (::getcontext(&fiber->context) == -1)
	    escape("unable to get the context");

	  // modify the context manually so that, once completed, the
	  // execution comes back to the parent fiber i.e the current fiber.
	  fiber->context.uc_link = NULL;
	  fiber->context.uc_stack.ss_sp = fiber->frame->stack;
	  fiber->context.uc_stack.ss_size = fiber->frame->size;
	  fiber->context.uc_flags = 0;

	  // create a context for the new fiber, with the Fiber::Launch
	  // as entry point.
	  ::makecontext(&fiber->context,
			reinterpret_cast<void (*)()>(launch),
			1,
			&closure);

          log_here;

	  // set the fiber state.
	  fiber->state = Fiber::StateActive;

	  // set the new fiber as the current one.
	  Fiber::Current = fiber;

	  // initialize the environment.
	  if (Fiber::Trigger(PhaseInitialize) == StatusError)
	    escape("unable to initialize the environment");

          log_here;
	  // set the new context.
	  if (::setcontext(&Fiber::Current->context) == -1)
	    escape("unable to set the context");
          log_here;

	  //
	  // should never reach this point since ::setcontext() never
	  // returns.
	  //
	  release();
	  fail("this code should never have been reached");
	}
      else
	{
	  Fiber*	fiber;

	  //
	  // at this point, we just came back from a fiber.
	  //

	  // perform an action depending on the state of the fiber.
	  switch (Fiber::Current->state)
	    {
	    case Fiber::StateCompleted:
	      {
		// select the parent fiber for scheduling which is
		// referenced through the link.
		fiber = Fiber::Current->link;

		// clean the environment.
		if (Fiber::Trigger(PhaseClean) == StatusError)
		  escape("unable to initialize the environment");

		// if the fiber has completed, delete it.
		if (Fiber::Delete(Fiber::Current) == StatusError)
		  escape("unable to delete the fiber");

		break;
	      }
	    case Fiber::StateSuspended:
	      {
		//
		// do not delete this fiber as it will be resumed later.
		//

		// select the program fiber.
		fiber = Fiber::Program;

		break;
	      }
	    case Fiber::StateActive:
	    case Fiber::StateUnknown:
	    default:
	      {
		escape("at this point a fiber cannot be in an unknown or "
		       "active state");
	      }
	    }

	  // set the current fiber as being the selected fiber.
	  Fiber::Current = fiber;

	  // set the fiber as active.
	  Fiber::Current->state = Fiber::StateActive;

	  // schedule the awaken fibers, only if we are in the program
	  // fiber i.e the root fiber. otherwise just come back to the
	  // fiber's execution.
	  if (Fiber::Current == Fiber::Program)
	    {
	      if (Fiber::Schedule() == StatusError)
		escape("unable to schedule the awaken fibers");
	    }

	  // restore the environment.
	  if (Fiber::Trigger(PhaseRestore) == StatusError)
	    escape("unable to restore the environment");

	  leave();
	}
    }

    ///
    /// this method takes the current fiber and sets the event it is
    /// expected to continue.
    ///
    template <typename T>
    Status		Fiber::Wait(const Event&		event,
				    T*&				data)
    {
      enter();

      // check if the current fiber is the program.
      if (Fiber::Current == Fiber::Program)
	escape("unable to wait while in the program fiber");

      // set the fiber has been suspended.
      Fiber::Current->state = Fiber::StateSuspended;

      // set the type.
      Fiber::Current->type = Fiber::TypeEvent;

      // set the event.
      Fiber::Current->event = new Event(event);

      // save the environment.
      if (Fiber::Trigger(PhaseSave) == StatusError)
	escape("unable to save the environment");

      // add the current fiber to the container.
      if (Fiber::Add(Fiber::Current) == StatusError)
	escape("unable to add the fiber to the container");

      // set the state of the program's fiber as awaken as we
      // are about to come back to it.
      Fiber::Program->state = Fiber::StateAwaken;

      // switch to the program's context and save the current one
      // in order to carry on at this point when woken up.
      if (::swapcontext(&Fiber::Current->context,
			&Fiber::Program->context) == -1)
	escape("unable to swap to the program context");

      // retrieve the data.
      data = static_cast<T*>(Fiber::Current->data);

      // reset the data.
      Fiber::Current->data = NULL;

      leave();
    }

    ///
    /// this method takes the current fiber and sets the resource it is
    /// expected to continue.
    ///
    /// note that resources are expected to be objects located in main
    /// memory i.e memory addresses. since memory addresses are unique,
    /// this simple scheme prevents conflicts.
    ///
    template <typename T>
    Status		Fiber::Wait(const Resource*		resource,
				    T*&				data)
    {
      enter();

      // check if the current fiber is the program.
      if (Fiber::Current == Fiber::Program)
	escape("unable to wait while in the program fiber");

      // set the fiber has been suspended.
      Fiber::Current->state = Fiber::StateSuspended;

      // set the type.
      Fiber::Current->type = Fiber::TypeResource;

      // set the resource.
      Fiber::Current->resource = resource;

      // save the environment.
      if (Fiber::Trigger(PhaseSave) == StatusError)
	escape("unable to save the environment");

      // add the current fiber to the container.
      if (Fiber::Add(Fiber::Current) == StatusError)
	escape("unable to add the fiber to the container");

      // set the state of the program's fiber as awaken as we
      // are about to come back to it.
      Fiber::Program->state = Fiber::StateAwaken;

      // switch to the program's context and save the current one
      // in order to carry on at this point when woken up.
      if (::swapcontext(&Fiber::Current->context,
			&Fiber::Program->context) == -1)
	escape("unable to swap to the program context");

      // retrieve the data.
      data = static_cast<T*>(Fiber::Current->data);

      leave();
    }

    ///
    /// this method wakes up the fiber waiting for the given event.
    ///
    template <typename T>
    Status		Fiber::Awaken(const Event&		event,
				      T*			data)
    {
      Fiber::F::Iterator	iterator;
      Boolean			awaken;

      enter();

      // check if there are blocked fibers.
      if (Fiber::Fibers.empty() == true)
	false();

      // set the boolean to false meaning that no fiber has been woken up.
      awaken = false;

      // locate, awaken and remove fibers as long as found.
      while (Fiber::Locate(event, iterator) == true)
	{
	  Fiber*	fiber = *iterator;

	  // set the boolean to true.
	  awaken = true;

	  // set the data.
	  fiber->data = static_cast<Meta*>(data);

	  // set the state as awaken.
	  fiber->state = Fiber::StateAwaken;

	  // reset the type.
	  fiber->type = Fiber::TypeNone;

	  // delete and reset the event.
	  delete fiber->event;
	  fiber->event = NULL;
	}

      // return true if at least one fiber has been awaken.
      if (awaken == true)
	true();

      false();
    }

    ///
    /// this method wakes up the fibers waiting for the given resource.
    ///
    template <typename T>
    Status		Fiber::Awaken(const Resource*		resource,
				      T*			data)
    {
      Fiber::F::Iterator	iterator;
      Boolean			awaken;

      enter();

      // check if there are blocked fibers.
      if (Fiber::Fibers.empty() == true)
	false();

      // set the boolean to false meaning that no fiber has been woken up.
      awaken = false;

      // locate, awaken and remove fibers as long as found.
      while (Fiber::Locate(resource, iterator) == true)
	{
	  Fiber*	fiber = *iterator;

	  // set the boolean to true.
	  awaken = true;

	  // set the data.
	  fiber->data = static_cast<Meta*>(data);

	  // set the state as awaken.
	  fiber->state = Fiber::StateAwaken;

	  // reset the type.
	  fiber->type = Fiber::TypeNone;

	  // reset the resource.
	  fiber->resource = NULL;
	}

      // return true if at least one fiber has been awaken.
      if (awaken == true)
	true();

      false();
    }

  }
}

#endif

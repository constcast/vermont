#ifndef CONNECTIONQUEUE_H
#define CONNECTIONQUEUE_H

/**
	@author Peter Baumann <siprbaum@users.berlios.de>
*/

#include "reconf/Destination.h"
#include "reconf/Module.h"
#include "reconf/Source.h"

#include "common/ConcurrentQueue.h"
#include "common/msg.h"
#include "common/Thread.h"
#include "sampler/Packet.h"

template <class T>
class ConnectionQueue
	: public Source<T>, public Destination<T>, public Module
{
public:
	ConnectionQueue(int maxEntries = ConcurrentQueue<T*>::DEFAULT_QUEUE_SIZE)
		: queue(maxEntries), thread(process), exitFlag(false)
	{
		thread.run(this); 
	}

	virtual ~ConnectionQueue() { exitFlag = true; }

	virtual void receive(T* packet)
	{
		msg(MSG_INFO, "receive(Packet*)");
		queue.push(packet);
	}

private:
	static void* process(void *arg)
	{
		ConnectionQueue* self = (ConnectionQueue*)arg;
		T* packet = NULL;

		while(!self->exitFlag) {
			self->queue.pop(&packet);
			self->send(packet);
		}
		
		msg(MSG_INFO, "terminating queue thread");
		return NULL;
	}

	ConcurrentQueue<T*> queue;
	Thread thread;

	bool exitFlag;
};

#endif

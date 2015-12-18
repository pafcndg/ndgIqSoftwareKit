@addtogroup cfw
@{

# Component Framework (CFW)

## Terminology

- Node: a node is an instantiation of the component framework. It corresponds to
a CPU of the hardware platform.
- Master: the master node of the platform, it instantiates the service manager.
- Slave: the slaves nodes of the platform, each one instantiates a service
manager proxy.

## Introduction

CFW is the core of the Thunderdome software stack. It allows to define services
and client applications than can run virtually on any CPU of the hardware
platform.

It consists of:
- The Service Manager API
- The sum of services APIs
- The 'default' main loop handler that will wait for messages to come in a
queue and call the appropriate message handler.



    +---------------------+
    |     Client          |
    +---------------------+

    +---------------------+   +-----------------+
    |     Services        |   | Service Manager |
    +---------------------+   +-----------------+

    +---------------------+   +-----------------+
    |    Drivers          |   |    INFRA        |
    +---------------------+   +-----------------+


## Service Manager / Service Manager Proxy

The Service Manager is the core entity of the component framework, it is
responsible for centralizing all services on the platform.

All services should register to the service manager, and all clients that need
to open services should call the Service Manager in order to open the specified
service.

When a service is not in the same node as the Service manager, the service
manager APIs are implemented by a Service Manager Proxy that relays the requests
to the actual Service Manager.


### Service Open Connection

\msc
 Client,API,SM,Service;
 |||;
 Client=>API [label="cfw_register_svc_available", URL="\ref cfw_register_svc_available"];
 API->SM [label="CFW_REGISTER_SVC_AVAILABLE REQ", URL="\ref cfw_register_svc_avail_req_msg_t"];
 Client<<API ;
 Client<=SM [label="CFW_REGISTER_SVC_AVAILABLE RSP", URL="\ref cfw_register_svc_avail_rsp_msg_t"];
 Service=>SM [label="register_service()", URL="\ref cfw_register_service"];
 SM=>Client [label="CFW_SVC_AVAIL_EVT", URL="\ref cfw_svc_available_evt_msg_t"];
 Client=>API [label="cfw_open_service_conn", URL="\ref cfw_open_service_conn"];
 API->SM [label="CFW_OPEN_SERVICE REQ", URL="\ref cfw_open_conn_req_msg_t"];
 SM->Service [label="client_connected"];
 Client<<API ;
 Client<-SM [label="CFW_OPEN_SERVICE RSP", URL="\ref cfw_open_conn_rsp_msg_t"];
\endmsc



### Remote Service Open Connection


\msc
 Client,API,SM,SMProxy,Service;
 |||;
 Client=>API [label="cfw_register_svc_available", URL="\ref cfw_register_svc_available"];
 API->SM [label="CFW_REGISTER_SVC_AVAILABLE", URL="\ref cfw_register_svc_avail_req_msg_t"];
 Client<<API;
 Client<=SM [label="CFW_REGISTER_SVC_AVAILABLE RSP", URL="\ref cfw_register_svc_avail_rsp_msg_t"];
 Service=>SMProxy [label="register_service()", URL="\ref cfw_register_service"];
 SMProxy=>SM [label="CFW_REGISTER_SVC", URL=""];
 SM=>Client [label="CFW_SVC_AVAIL_EVT", URL="\ref cfw_svc_available_evt_msg_t"];
 Client=>API [label="cfw_open_service_conn", URL="\ref cfw_open_service_conn"];
 API->SM [label="CFW_OPEN_SERVICE REQ", URL="\ref cfw_open_conn_req_msg_t"];
 Client<<API;
 SM->SMProxy [label="CFW_OPEN_SERVICE REQ"];
 SMProxy->Service [label="client_connected"];
 Client<-SMProxy [label="CFW_OPEN_SERVICE RSP", URL="\ref cfw_open_conn_rsp_msg_t"];
\endmsc


### Register for Local Service events

\msc
 Client,API,SM,Service;
 |||;
 Client box Service [label="Client is connected to service"];
 Client=>API [label="cfw_register_events", URL="\ref cfw_register_events"];
 API->SM [label="CFW_REGISTER_EVENTS REQ", URL="\ref cfw_register_evt_req_msg_t"];
 SM note SM [label="Service is local, store registration locally"];
 Client<<API;
 Client<-SM [label="CFW_REGISTER_EVENTS RSP", URL="\ref cfw_register_evt_rsp_msg_t"];
\endmsc


### Register for Distant Service events

\msc
 Client,API,SM,SMProxy,Service;
 |||;
 Client box Service [label="Client is connected to service"];
 Client=>API [label="cfw_register_events", URL="\ref cfw_register_events"];
 API->SM [label="CFW_REGISTER_EVENTS REQ", URL="\ref cfw_register_evt_req_msg_t"];
 Client<<API;
 SM note SM [label="Service is distant, this indication not registered locally yet, fw to proxy so SM gets the indication from service. Other local registrations will be stored locally"];
 SM->SMProxy [label="CFW_REGISTER_EVENTS REQ", URL="\ref cfw_register_evt_req_msg_t"];
 Client<-SMProxy [label="CFW_REGISTER_EVENTS RSP", URL="\ref cfw_register_evt_rsp_msg_t"];
\endmsc




## Service

A _service_ is a software component that offers a service (a set of features)
to applications. A service must be multi-client, and should work on an
asynchronous manner.

The service is composed of :
- an _API_ whose purpose is to generate the request messages
- and an _implementation_ to
  + handle the request messages
  + and send the response message when available.

Services also allow defining some events that can be broadcasted to registered
clients/applications.

Service implementation example:

~~~~~{.c}

static void my_service_client_connected(conn_handle_t * conn)
{
	// Do whatever needed for client management.
	conn->priv_data = state;
}

static void my_service_client_disconnected(conn_handle_t * conn)
{
	// Free any per-client state data
}

static void my_service_handle_message(struct cfw_message * msg, void *param)
{
	switch(CFW_MESSAGE_ID(msg)) {
		case MSG_ID_MY_SERVICE_REQUEST_REQ: {
			my_service_request_req_msg_t * req = (my_service_request_req_msg_t *) msg;
			my_service_request_rsp_msg_t * rsp = cfw_alloc_rsp_msg(req, MSG_ID_MY_SERVICE_REQUEST_RSP, sizeof(*rsp));
			// Fill rsp fields
			cfw_send_message(rsp);
		}
		break;
	}
	message_free(msg);
}

static service_t my_service = {
	.service_id = MY_SERVICE_ID,
	.client_connected = my_service_client_connected,
	.client_disconnected = my_service_client_disconnected,
};

void my_service_init(QUEUE_T queue)
{
	cfw_register_service(queue, &my_service, my_service_handle_message, NULL);
}
~~~~~

## Client

The client/application is the entity that implements the features of the device.
It generally uses services, infra and sometimes drivers in order to implement
its features.
As the services and drivers all implements asynchronous APIs, the
client/application is a message/event handler.

Client implementation example:

~~~~~{.c}

static cfw_client_t * client = NULL;
static cfw_service_conn_t * my_service_conn = NULL;

static void client_handle_cb(struct cfw_message * msg, void * parm)
{
	switch(CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_SVC_AVAIL_EVT_RSP: {
		cfw_svc_available_evt_msg_t * evt = (cfw_svc_available_evt_msg_t*) msg;
		if (msg->service_id == MY_SERVICE_ID) {
			cfw_open_service_conn(client, MY_SERVICE_ID, (void*)MY_SERVICE_ID);
		}
	}
	break;
	case MSG_ID_CFW_OPEN_SERVICE_RSP: {
		cfw_open_conn_rsp_msg_t * rsp = (cfw_open_conn_rsp_msg_t*) msg;
		if ((int) CFW_MESSAGE_PRIV(msg) == MY_SERVICE_ID) {
			// Service connected, start using the service
			my_service_conn = rsp->service_conn;
			my_service_request(my_service_conn, my_service_param, NULL);
		}
	}
	break;
	case MSG_ID_MY_SERVICE_REQUEST_RSP: {
		my_service_request_rsp_msg_t * rsp = (my_service_request_rsp_msg_t *) msg;
		// Do what's needed with rsp
	}
	break;
	}
	message_free(msg);
}

void client_init(QUEUE_T queue)
{
	client = cfw_client_init(queue, client_message_cb, NULL);
	cfw_register_svc_available(client, MY_SERVICE_ID, NULL);
}

~~~~~

## Execution contexts

### Tasks

Tasks are preemptible execution contexts, they are threads with a given priority.
Whenever a task with higher priority than current is ready, it will preempt the
current task.
- The microkernel allows multiple tasks creation.
- The nanokernel has only one task that calls the main() function.

### Fiber

A fiber is like a thread that is not preemptible. Its execution will continue
until it releases the CPU by sleeping, waiting on a semaphore / mutex.
Only interrupts can interrupt a fiber. All fibers are of higher priority than
the task.

### Interrupt

#### Signaling mechanisms
An interrupt is executed in a specific context, no processing should be done
inside the interrupt function.

The generic behavior of an interrupt is either:
- _to signal a semaphore_ to wakeup a sleeping fiber / task if there is no data to
pass to the non-interrupt context;
- or _to allocate and send a message_ to the non-interrupt context when there is
data to pass.

#### Driver callbacks

Driver implementation generally allows callback functions to be defined.

These callbacks are _always called in the interrupt context_.
A driver user should only use the mechanisms described above to signal the
application (or service) of the event that occurred.

#### Limited resources
As the system resources are small in the Quark SE 1 SoC (Socket on Chip),
the number of tasks / fibers should be limited to the minimum.

Sharing a queue for all the logical applications of the platform is a way to
minimize the resource usage.
This means that all applications are running in the same thread,
by using the same queue for CFW client definition.
All events will be multiplexed to the same queue.

If an application needs to implement heavy processing of data, then it could
make sense to create a dedicated task for this processing with a low priority
so it runs when no other service / applications needs the CPU.

@}

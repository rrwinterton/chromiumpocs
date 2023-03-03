/******************************************************************************
** Copyright (c) 2013-2022 Intel Corporation All Rights Reserved
**
** Licensed under the Apache License, Version 2.0 (the "License"); you may not
** use this file except in compliance with the License.
**
** You may obtain a copy of the License at
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
** WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**
** See the License for the specific language governing permissions and
** limitations under the License.
**
******************************************************************************/

//
// Include header specifying the API exported by IPFLIB.LIB
//
#include "ipf_core_api.h"
#include "ipf_sdk_version.h"

///////////////////////////////////////////////////////////////////////////////
// OS Abstraction Layer
///////////////////////////////////////////////////////////////////////////////

// OS Agnostic
#include <memory.h>
static ESIF_INLINE void* ipf_os_malloc(size_t len)
{
	return malloc(len);
}
static ESIF_INLINE void ipf_os_free(void* ptr)
{
	if (ptr) {
		free(ptr);
	}
}

// Helper macros for ptr/context conversion
#define ipf_os_ptr2context(p)	((esif_context_t)(size_t)(p))
#define ipf_os_context2ptr(u)	((void *)(size_t)(u))

// Windows OS
#define ipf_os_sleep(sec)					Sleep((sec) * 1000)
#define ipf_os_strncmp(s1, s2, count)		strncmp(s1, s2, count)
#define ipf_os_strnicmp(s1, s2, cnt)		_strnicmp(s1, s2, cnt)
#define ipf_os_strpbrk(str, set)			strpbrk(str, set)

static ESIF_INLINE char* ipf_os_strcpy(
	char* dst,
	const char* src,
	size_t siz
)
{
	if (siz) {
		strncpy_s(dst, siz, src, _TRUNCATE);
	}
	return dst;
}



///////////////////////////////////////////////////////////////////////////////
// General Definitions
///////////////////////////////////////////////////////////////////////////////

#define PROGRAM_NAME			"sampleapp2"
#define PROGRAM_DESCRIPTION		"IPF Core API Sample Application"
#define	PROGRAM_VERSION			IPF_APP_VERSION
#define PROGRAM_SERVERADDR		ELEVATED_SERVERADDR		// Elevated Access is required to execute SET primitives, otherwise use DEFAULT_SERVERADDR
#define IPF_SESSION_START_RETRIES		1		// By default, we do not retry connection
#define SAMPLE_ITERATION_DELAY_TIME	1		    // seconds

#define TRACE_FATAL		0
#define TRACE_ERROR		1
#define TRACE_WARNING	2
#define TRACE_INFO		3
#define TRACE_DEBUG		4

#define TRACEMSG_FATAL(msg, ...)	do { if (g_tracelevel >= TRACE_FATAL)	{ fprintf(stderr, msg, ##__VA_ARGS__); } } while (0)
#define TRACEMSG_ERROR(msg, ...)	do { if (g_tracelevel >= TRACE_ERROR)	{ fprintf(stderr, msg, ##__VA_ARGS__); } } while (0)
#define TRACEMSG_WARNING(msg, ...)	do { if (g_tracelevel >= TRACE_WARNING)	{ fprintf(stderr, msg, ##__VA_ARGS__); } } while (0)
#define TRACEMSG_INFO(msg, ...)		do { if (g_tracelevel >= TRACE_INFO)	{ fprintf(stderr, msg, ##__VA_ARGS__); } } while (0)
#define TRACEMSG_DEBUG(msg, ...)	do { if (g_tracelevel >= TRACE_DEBUG)	{ fprintf(stderr, msg, ##__VA_ARGS__); } } while (0)

int g_tracelevel = TRACE_ERROR;

typedef enum PowerLimitInstance {
	PL1,
	PL2,
	POWER_LIMIT_INSTANCE_MAX
} PowerLimitInstance;

u32 power_limit_defaults[POWER_LIMIT_INSTANCE_MAX] = { 0, 0 };

///////////////////////////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////////////////////////

static esif_error_t ParseCmdLineArgs(int argc, char *argv[]);
static esif_error_t ExerciseInterface(IpfSession_t session);
static esif_error_t ExercisePowerLimits(IpfSession_t session);
static esif_error_t DisplayParticipantList(IpfSession_t session);
static esif_error_t DisplayCpuTemp(IpfSession_t session);
static esif_error_t DisplayCpuPower(IpfSession_t session);
static esif_error_t GetPlatformPowerLimit(IpfSession_t session, PowerLimitInstance limit_instance, u32 *power_limit);
static esif_error_t SetPlatformPowerLimit(IpfSession_t session, PowerLimitInstance limit_instance, u32 power_limit);
static esif_error_t RegisterEvents(IpfSession_t session);
static void UnregisterEvents(IpfSession_t session);
static void DisplayHelpMsg();

static void ESIF_CALLCONV EventCallback(
	IpfSession_t session,
	esif_event_type_t eventType,
	char *participantName,
	EsifData *dataPtr,
	esif_context_t context
);

///////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////
#define RUN_FOREVER	((UInt32)(-1))	 // -1=Run Application Forever
#define DEFAULT_RUNTIME 10           // Default Iterations (seconds) app will run before exiting or RUN_FOREVER

UInt32	g_runTime = DEFAULT_RUNTIME; // Number of iterations (seconds) the sample will run before termination; Override with -time:<seconds> option
int		g_exitFlag = 0;				 // Exit Application Flag

IpfSessionInfo sessionInfo = {
	.v1 = {
		.revision = IPF_SESSIONINFO_REVISION,
		.appName = PROGRAM_NAME,
		.appVersion = PROGRAM_VERSION,
		.appDescription = PROGRAM_DESCRIPTION,
		.serverAddr = PROGRAM_SERVERADDR
	}
};


///////////////////////////////////////////////////////////////////////////////
//
// This sample code is provided to demonstrate basic usage of the IPF API
// exported by IPFLIB.LIB.  The API allows the user to interface with Intel(R)
// Innovation Platform Framework to obtain state information and events related
// to the capabilities it provides.
//
// A basic understanding of Intel(R) Innovation Platform Framework, its concepts,
// and terminology is expected.  (Refer to the IPF CORE User's Guide for additional
// information.)
//
// The code provided uses each of API functions provided in order to
// demonstrate basic usage and API order requirements.
//
// The code performs the following major tasks:
// 1) CORE API initialization
// 2) Creation of a session
// 3) Starting the session
//    a) Poll for server presence until timeout
// 4) Example use of the API
//    a) Obtain and display the participant list
//    b) Register for a small set of events
//    c) Loop executing a set of primitives every 5s
//        i) Display the processor temperature
//        ii) Display the CPU power
//        iii) Set the platform power PL1 and PL2 limits, alternating values each iteration
//        iv) Get the platform power PL1 and PL2 limits
//    d) Unregister from events
// 7) Stopping the session
// 8) Destruction of the session
// 9) CORE API tear-down
//
///////////////////////////////////////////////////////////////////////////////

int main(
	int argc,
	char *argv[]
)
{
	esif_error_t rc = ESIF_OK;
	int appErr = 0;
	IpfSession_t session = IPF_INVALID_SESSION;
	int startRetries = 0;

	//
	// Parse command line options
	// (Options are set in global variables.)
	//
	rc = ParseCmdLineArgs(argc, argv);
	if (ESIF_OK != rc) {
		return appErr;
	}

	//
	// Initialize the CORE API for use.
	// IpfCore_Init must be called before any other CORE API function.
	//
	rc = IpfCore_Init();

	if (ESIF_OK == rc) {
		//
		// Creates a session object for use when connecting to the server using
		// IpfCore_SessionConnect.
		//
		// SessionInfo parameter contains the AppName, which must be unique within ESIF.
		// If AppName is blank, pass NULL as sessionInfo, which will use the IPF defaults.
		// The Server SDK Version will be returned to the caller if SessionInfo is not NULL.
		//
		IpfSessionInfo *info = (sessionInfo.v1.appName[0] ? &sessionInfo : NULL);
		session = IpfCore_SessionCreate(info);
		if (session != IPF_INVALID_SESSION) {
			//
			// Establish a connection with the server using the specified session object
			//
			// We try multiple times in case the server is not running initially
			// Note:  Default is to try only once!
			//
			fprintf(stderr, "Connecting to %s (SDK %s)...", (info && info->v1.serverAddr[0] ? info->v1.serverAddr : DEFAULT_SERVERADDR), (info && info->v1.sdkVersion[0] ? info->v1.sdkVersion : "NA"));
			do {
				rc = IpfCore_SessionConnect(session);
			} while ((rc != ESIF_OK) && (++startRetries < IPF_SESSION_START_RETRIES));

			//
			// Connection Established. Execute the Interface Application until it exits
			// on its own or a SESSION_DISCONNECTED Event is received, indicating that
			// the connection has closed, either due the Server closing it gracefully
			// or due to a Server crash or Network error.
			//
			// The code should clean up as needed and restart the session if desired.
			// This sample simply sets a flag for the exercise thread to exit its
			// loop and then exit.  A more complex sample could re-establish the
			// connection as needed.
			//
			if (ESIF_OK == rc) {
				fprintf(stderr, "Connected\n");
				rc = ExerciseInterface(session);
			}

			//
			// Stop the session and close the connection associated with the session
			//
			// After IpfCore_SessionDisconnect is called, the session object may not be used
			// for access until IpfCore_SessionConnect is called again with the object to open
			// a new session
			//
			IpfCore_SessionDisconnect(session);

			//
			// Permanently stops a connection and destroys the session object
			//
			// After IpfCore_SessionDestroy is called, the session object may not be used
			// again for any other API calls; including IpfCore_SessionConnect
			//
			IpfCore_SessionDestroy(session);
			session = IPF_INVALID_SESSION;
		}
		//
		// Clean up the CORE API after use.
		// IpfCore_Exit should be called after all other CORE API functions.
		//
		IpfCore_Exit();
	}

	if (rc != ESIF_OK) {
		fprintf(stderr, "%s\n", esif_rc_str(rc));
		appErr = -1;
	}
	return appErr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Main IPF Application function that demonstrates using the IPF Core Interface.
// This function is called after successfully establishing a connection with
// the IPF Server and runs until either g_runTime reaches 0 or g_exitFlag is
// set, due to either the Server closing the connection gracefully, or by the
// connection being terminated unexpectedly due to Server crash or Network error.
// Both conditions will result in a SESSION_DISCONNECTED event being generated.
//
///////////////////////////////////////////////////////////////////////////////
static esif_error_t ExerciseInterface(IpfSession_t session)
{
	esif_error_t rc = ESIF_OK;
	u32 power_limit[POWER_LIMIT_INSTANCE_MAX] = { 0, 0 };
	PowerLimitInstance pl_inst = PL1;

	//
	// Register for required events
	//
	rc = RegisterEvents(session);

	//
	// Display a list of the current active participants
	//
	if (ESIF_OK == rc) {
		rc = DisplayParticipantList(session);
	}

	//
	// Save the current power limits
	//
	if (ESIF_OK == rc) {
		for (pl_inst = PL1; pl_inst < POWER_LIMIT_INSTANCE_MAX; ++pl_inst) {
			if (power_limit_defaults[pl_inst] > 0) {
				power_limit[pl_inst] = power_limit_defaults[pl_inst];
				rc = ESIF_OK;
			}
			else {
				rc = GetPlatformPowerLimit(session, pl_inst, &power_limit[pl_inst]);
			}
			if (ESIF_OK == rc) {
				printf("Saved initial PL%d power limit of %d\n", pl_inst + 1, power_limit[pl_inst]);
			}
			else {
				printf("Unable to read platform power PL%d; %s(%d)\n", pl_inst + 1, esif_rc_str(rc), rc);
			}
		}
	}


	//
	// Loop displaying the processor temperature and power and exercising set / get power limits
	//
	if (ESIF_OK == rc) {
		while ((rc != ESIF_E_SESSION_DISCONNECTED) && (g_runTime) && (!g_exitFlag)) {

			rc = DisplayCpuTemp(session);

			if (rc != ESIF_E_SESSION_DISCONNECTED) {
				rc = DisplayCpuPower(session);
			}

			if (rc != ESIF_E_SESSION_DISCONNECTED) {
				rc = ExercisePowerLimits(session);
			}

			if (g_runTime != RUN_FOREVER) {
				g_runTime -= SAMPLE_ITERATION_DELAY_TIME;
			}
			if (g_runTime) {
				ipf_os_sleep(SAMPLE_ITERATION_DELAY_TIME);
			}
		}
	}


	//
	// Restore the power limits that existed at runtime
	//
	if (ESIF_OK == rc) {
		for (pl_inst = PL1; pl_inst < POWER_LIMIT_INSTANCE_MAX; ++pl_inst) {
			rc = SetPlatformPowerLimit(session, pl_inst, power_limit[pl_inst]);
			if (ESIF_OK == rc) {
				printf("Restored initial PL%d power limit of %d\n", pl_inst + 1, power_limit[pl_inst]);
			}
			else {
				printf("Unable to set platform power PL%d; %s(%d)\n", pl_inst + 1, esif_rc_str(rc), rc);
			}
		}
	}

	//
	// Unregister from events before exit
	//
	UnregisterEvents(session);

	return rc;
}

static esif_error_t ExercisePowerLimits(IpfSession_t session) {
	esif_error_t rc = ESIF_OK;
	static int power_limit_selector = 1;
	u32 power_limit[POWER_LIMIT_INSTANCE_MAX] = { 0, 0 };
	PowerLimitInstance pl_inst = PL1;
	u32 response_value = 0;

	// Toggle betwen min / max values as given below :
	// Package PL1 : 15 - 30 W
	// Package PL2 : 40 - 64 W
	if (power_limit_selector) {
		power_limit[PL1] = 15000;  // mW
		power_limit[PL2] = 40000;  // mW
	}
	else {
		power_limit[PL1] = 30000;  // mW
		power_limit[PL2] = 64000;  // mW
	}
	power_limit_selector = !power_limit_selector;

	for (pl_inst = PL1; pl_inst < POWER_LIMIT_INSTANCE_MAX; ++pl_inst) {
		if (rc != ESIF_E_SESSION_DISCONNECTED) {
			rc = SetPlatformPowerLimit(session, pl_inst, power_limit[pl_inst]);
			if (ESIF_OK == rc) {
				printf("Set platform power PL%d to %d mW\n", pl_inst + 1, power_limit[pl_inst]);
			}
			else {
				printf("Unable to set platform power PL%d; %s(%d)\n", pl_inst + 1, esif_rc_str(rc), rc);
			}
		}

		if (rc != ESIF_E_SESSION_DISCONNECTED) {
			rc = GetPlatformPowerLimit(session, pl_inst, &response_value);
			if (ESIF_OK == rc) {
				printf("Platform power PL%d = %d mW\n", pl_inst + 1, response_value);
			}
			else {
				printf("Unable to read platform power PL%d; %s(%d)\n", pl_inst + 1, esif_rc_str(rc), rc);
			}
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// This sample code contains a single event handler for all event types;
// however; the developer may use multiple event handlers for their purposes
// as the handler is specified when register for a give event type.
//
///////////////////////////////////////////////////////////////////////////////
static void ESIF_CALLCONV EventCallback(
	IpfSession_t session,
	esif_event_type_t eventType,
	char *participantName,
	EsifData *dataPtr,
	esif_context_t context
)
{
	UNREFERENCED_PARAMETER(session);
	UNREFERENCED_PARAMETER(context);

	switch (eventType) {
	case ESIF_EVENT_SESSION_DISCONNECTED:
		g_exitFlag = 1;
		fprintf(stderr, "Disconnected\n");
		break;
	case ESIF_EVENT_LOG_VERBOSITY_CHANGED:
		if ((dataPtr != NULL) && (dataPtr->buf_ptr != NULL) && (dataPtr->buf_len >= sizeof(int))) {
			g_tracelevel = *(int *)dataPtr->buf_ptr;
			printf("Log verbosity changed to %d\n\n", *(int *)dataPtr->buf_ptr);
		}
		break;
	case ESIF_EVENT_PARTICIPANT_SUSPEND:
		if (participantName) {
			printf("Participant %s suspended\n", participantName);
		}
		break;
	case ESIF_EVENT_PARTICIPANT_RESUME:
		if (participantName) {
			printf("Participant %s resumed\n", participantName);
		}
		break;
	default:
		break;
	}
	return;
}


//
// The events registered here are just a sample of the events that may be registered for.
// See the IPF CORE writer's guide for supported events available in esif_sdk_event_type.h
//
static esif_error_t RegisterEvents(IpfSession_t session)
{
	esif_error_t rc = ESIF_OK;

	rc = IpfCore_SessionRegisterEvent(session, ESIF_EVENT_SESSION_DISCONNECTED, NULL, EventCallback, ipf_os_ptr2context(NULL));

	if (rc == ESIF_OK) {
		rc = IpfCore_SessionRegisterEvent(session, ESIF_EVENT_LOG_VERBOSITY_CHANGED, NULL, EventCallback, ipf_os_ptr2context(NULL));
	}
	if (ESIF_OK == rc) {
		rc = IpfCore_SessionRegisterEvent(session, ESIF_EVENT_OS_POWER_SOURCE_CHANGED, NULL, EventCallback, ipf_os_ptr2context(NULL));
	}

	if (rc != ESIF_OK) {
		fprintf(stderr, "Failed to register event; %s(%d)\n\n", esif_rc_str(rc), rc);
	}
	return rc;
}


static void UnregisterEvents(IpfSession_t session)
{
	IpfCore_SessionUnregisterEvent(session, ESIF_EVENT_OS_POWER_SOURCE_CHANGED, NULL, EventCallback, ipf_os_ptr2context(NULL));
	IpfCore_SessionUnregisterEvent(session, ESIF_EVENT_LOG_VERBOSITY_CHANGED, NULL, EventCallback, ipf_os_ptr2context(NULL));
	IpfCore_SessionUnregisterEvent(session, ESIF_EVENT_SESSION_DISCONNECTED, NULL, EventCallback, ipf_os_ptr2context(NULL));
}


static esif_error_t DisplayParticipantList(IpfSession_t session)
{
	esif_error_t rc = ESIF_E_NO_MEMORY;
	char command_buffer[] = "send-command partlist";
	EsifData command = { ESIF_DATA_STRING , NULL, sizeof(command_buffer), sizeof(command_buffer) };
	char *response_buffer = NULL;
	size_t response_buflen = 4096;
	EsifData response = { ESIF_DATA_STRING , NULL, 0, 0 };

	//
	// Set the command buffer pointer to the desired command string
	//
	command.buf_ptr = command_buffer;

	//
	// Allocate a buffer to receive the part list
	//
	response_buffer = ipf_os_malloc(response_buflen);
	if (response_buffer != NULL) {
		//
		// Initialize the response buffer to use the allocated buffer
		//
		response.buf_ptr = response_buffer;
		response.buf_len = (u32)response_buflen;

		rc = IpfCore_SessionExecute(session, &command, NULL, &response);
		if (rc == ESIF_E_NEED_LARGER_BUFFER) {

			ipf_os_free(response_buffer);
			response_buffer = ipf_os_malloc((size_t)response.data_len);
			rc = ESIF_E_NO_MEMORY;

			if (response_buffer) {
				response.buf_ptr = response_buffer;
				response.buf_len = response.data_len;
				response.data_len = 0;
				rc = IpfCore_SessionExecute(session, &command, NULL, &response);
			}
		}

		if ((ESIF_OK == rc) && (response_buffer != NULL)) {
			printf("%s\n\n", response_buffer);
		}
		else {
			printf("Unable to get participant list; %s(%d)\n\n", esif_rc_str(rc), rc);
		}
	}

	ipf_os_free(response_buffer);
	return rc;
}


static esif_error_t DisplayCpuTemp(IpfSession_t session)
{
	esif_error_t rc = ESIF_E_NO_MEMORY;
	char command_buffer[] = "execute-primitive TCPU GET_TEMPERATURE";
	EsifData command = { ESIF_DATA_STRING , NULL, sizeof(command_buffer), sizeof(command_buffer) };
	u32 response_value = 0;
	/* Note use of ESIF_DATA_TEMPERATURE as the type.  This is required for correct units for the temperature */
	EsifData response = { ESIF_DATA_TEMPERATURE , NULL, sizeof(response_value), 0 };

	//
	// Set the command buffer pointer to the desired command string
	//
	command.buf_ptr = command_buffer;

	//
	// Initialize the response buffer to point to the retturn value
	//
	response.buf_ptr = &response_value;

	rc = IpfCore_SessionExecute(session, &command, NULL, &response);
	if (ESIF_OK == rc) {
		printf("CPU temperature = %d deci-Kelvin\n", response_value);
	}
	else {
		TRACEMSG_WARNING("Unable to read TCPU temperature; %s(%d)\n", esif_rc_str(rc), rc);
	}
	return rc;
}


static esif_error_t DisplayCpuPower(IpfSession_t session)
{
	esif_error_t rc = ESIF_E_NO_MEMORY;
	char command_buffer[] = "execute-primitive TCPU GET_RAPL_POWER";
	EsifData command = { ESIF_DATA_STRING , NULL, sizeof(command_buffer), sizeof(command_buffer) };
	u32 response_value = 0;
	/* Note use of ESIF_DATA_POWER as the type.  This is required for correct units for the power */
	EsifData response = { ESIF_DATA_POWER , NULL, sizeof(response_value), 0 };

	//
	// Set the command buffer pointer to the desired command string
	//
	command.buf_ptr = command_buffer;

	//
	// Initialize the response buffer to point to the retturn value
	//
	response.buf_ptr = &response_value;

	rc = IpfCore_SessionExecute(session, &command, NULL, &response);
	if (ESIF_OK == rc) {
		printf("CPU power = %d mW\n", response_value);
	}
	else {
		TRACEMSG_WARNING("Unable to read TCPU power; %s(%d)\n", esif_rc_str(rc), rc);
	}
	return rc;
}



static esif_error_t GetPlatformPowerLimit(IpfSession_t session, PowerLimitInstance limit_instance, u32 *power_limit) {
	esif_error_t rc = ESIF_E_NO_MEMORY;

	EsifData command;
	char command_buffer_get_pl1[] = "execute-primitive TCPU GET_PLATFORM_POWER_LIMIT_1";
	char command_buffer_get_pl2[] = "execute-primitive TCPU GET_PLATFORM_POWER_LIMIT_2";

	//
	// Set the command buffer pointer to the desired command string
	//
	if (PL1 == limit_instance) {
		command.type = ESIF_DATA_STRING;
		command.buf_ptr = &command_buffer_get_pl1;
		command.buf_len = sizeof(command_buffer_get_pl1);
		command.data_len = sizeof(command_buffer_get_pl1);
	}
	else if (PL2 == limit_instance) {
		command.type = ESIF_DATA_STRING;
		command.buf_ptr = &command_buffer_get_pl2;
		command.buf_len = sizeof(command_buffer_get_pl2);
		command.data_len = sizeof(command_buffer_get_pl2);
	}
	else {
		rc = ESIF_E_NOT_SUPPORTED;
		goto exit;
	}

	/* Note use of ESIF_DATA_POWER as the type.  This is required for correct units for the power */
	EsifData response = { ESIF_DATA_POWER , NULL, sizeof(*power_limit), 0 };

	//
	// Initialize the response buffer to point to the return value
	//
	response.buf_ptr = power_limit;

	rc = IpfCore_SessionExecute(session, &command, NULL, &response);

exit:
	return rc;
}


static esif_error_t SetPlatformPowerLimit(IpfSession_t session, PowerLimitInstance limit_instance, u32 power_limit) {
	esif_error_t rc = ESIF_E_NO_MEMORY;

	EsifData command;
	char command_buffer_set_pl1[] = "execute-primitive TCPU SET_PLATFORM_POWER_LIMIT_1";
	char command_buffer_set_pl2[] = "execute-primitive TCPU SET_PLATFORM_POWER_LIMIT_2";

	//
	// Set the command buffer pointer to the desired command string
	//
	if (PL1 == limit_instance) {
		command.type = ESIF_DATA_STRING;
		command.buf_ptr = &command_buffer_set_pl1;
		command.buf_len = sizeof(command_buffer_set_pl1);
		command.data_len = sizeof(command_buffer_set_pl1);
	}
	else if (PL2 == limit_instance) {
		command.type = ESIF_DATA_STRING;
		command.buf_ptr = &command_buffer_set_pl2;
		command.buf_len = sizeof(command_buffer_set_pl2);
		command.data_len = sizeof(command_buffer_set_pl2);
	}
	else {
		rc = ESIF_E_NOT_SUPPORTED;
		goto exit;
	}

	u32 request_value = 0;

	/* Note use of ESIF_DATA_POWER as the type.  This is required for correct units for the power */
	EsifData request = { ESIF_DATA_POWER , NULL, sizeof(request_value), sizeof(request_value) };

	//
	// Initialize the request buffer to point to the limit settings
	//
	request.buf_ptr = &power_limit;

	rc = IpfCore_SessionExecute(session, &command, &request, NULL);

exit:
	return rc;
}



static esif_error_t ParseCmdLineArgs(
	int argc,
	char *argv[]
)
{
	esif_error_t rc = ESIF_OK;
	int displayHelp = 0;
	char *curArg = NULL;
	char *curOpt = NULL;

	for (int i = 1; i < argc; i++) {
		// Support --opt, -opt, and /opt
		if (ipf_os_strncmp(argv[i], "--", 2) == 0) {
			curArg = &argv[i][2];
		}
		else if (argv[i][0] == '-' || argv[i][0] == '/') {
			curArg = &argv[i][1];
		}
		else {
			displayHelp = 1;
			break;
		}

		// Optional --arg[:<option>] or --arg[=option]
		if ((curOpt = ipf_os_strpbrk(curArg, ":=")) != NULL && curOpt[1]) {
			curOpt = (curOpt[0] && curOpt[1] ? curOpt + 1 : NULL);
		}

		// --time:<seconds>	[-1 = Forever]
		if ((ipf_os_strnicmp(curArg, "time", 1) == 0 || ipf_os_strnicmp(curArg, "sample", 3) == 0) && curOpt) {
			g_runTime = (UInt32)atoi(curOpt);
		}
		// --name:<name>
		else if (ipf_os_strnicmp(curArg, "name", 4) == 0 && curOpt) {
			ipf_os_strcpy(sessionInfo.v1.appName, curOpt, sizeof(sessionInfo.v1.appName));
		}
		// --desc:<desc>	[Enclose "<desc>" in Quotes if it contains spaces]
		else if (ipf_os_strnicmp(curArg, "desc", 4) == 0 && curOpt) {
			ipf_os_strcpy(sessionInfo.v1.appDescription, curOpt, sizeof(sessionInfo.v1.appDescription));
		}
		// --srv:<address>
		else if ((ipf_os_strnicmp(curArg, "srv", 3) == 0 || ipf_os_strnicmp(curArg, "server", 4) == 0) && curOpt) {
			ipf_os_strcpy(sessionInfo.v1.serverAddr, curOpt, sizeof(sessionInfo.v1.serverAddr));
		}
		// --pl<1|2>:<value>
		else if (ipf_os_strnicmp(curArg, "pl", 2) == 0 && curOpt) {
			int pl = atoi(&curArg[2]);
			if (pl > 0 && pl <= sizeof(power_limit_defaults) / sizeof(power_limit_defaults[0])) {
				power_limit_defaults[pl - 1] = atoi(curOpt);
			}
		}
		// Invalid Options
		else {
			displayHelp = 1;
			break;
		}
	}

	if (displayHelp) {
		DisplayHelpMsg();
		rc = ESIF_E_UNSPECIFIED;
	}
	return rc;
}


static void DisplayHelpMsg()
{
	printf(
		PROGRAM_DESCRIPTION ", Version " PROGRAM_VERSION " (SDK " IPF_SDK_VERSION ")\n"
		"Copyright (c) 2013-2022 Intel Corporation All Rights Reserved\n"
		"\n"
		"Usage:\n"
		"    " PROGRAM_NAME " [options]\n"
		"\n"
		"Options:\n"
		"  --time:<sec>     Time in seconds after which the application exits.\n"
		"  --name:<name>    App Name to use when connecting to IPF Server\n"
		"  --desc:\"<desc>\"  App Description to use when connecting to IPF Server\n"
		"  --srv:<address>  Specify Server Address [Default=%s]\n"
		"  --pl<id>:<value> Use <value> for initial PL1 or PL2 values\n"
		"  --?              Display this help message.\n"
		"\n",
		PROGRAM_SERVERADDR
	);
}


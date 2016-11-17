#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include "key_array.h"

#define LOGFILE_PATH 			"/var/log/keyboard_stats_daemon.log"
#define STATISTICS_PATH 		"/var/log/keyboard.stats"

#define KEYBOARD_EVENTS_PATH	"/dev/input/by-path/platform-i8042-serio-0-event-kbd"
#define KEYS_COUNT 				243
#define EV_KEY					1
#define EV_KEY_RELEASE_VALUE	0
#define EV_KEY_PRESS_VALUE		1
#define EV_KEY_AUTOREPEAT_VALUE 2

const int statsTimeout = 10;

struct KeyboardInputEvent {
	struct timeval time;
	unsigned short type;
	unsigned short code;
	unsigned int value;
};

#define Event struct KeyboardInputEvent
#define EVENT_SIZE sizeof(Event)

void daemonInit() {
	pid_t pid, sid;

	pid = fork();
	if (pid < 0) {
	    exit(EXIT_FAILURE);
	}

	if (pid > 0) {
	    exit(EXIT_SUCCESS);
	}

	umask(0);
	            	            
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	if ((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

void demonDeinit(FILE *logFile) {
	fclose(logFile);
}

char *keyname(int code) {
	return KEY_NAMES[code];
}

void logEvent(Event *event, FILE *logFile) {
	fprintf(
		logFile,
		"Key press | %-15s | Time: %ld.%ld, Type: %d, Code: %d, Value: %d\n",
		keyname(event->code), event->time.tv_sec, event->time.tv_usec, event->type, event->code, event->value
	);
}

int main(void) {
	daemonInit();

	int i; 

	FILE *logFile = fopen(LOGFILE_PATH, "w");
	if (logFile == NULL) {
		exit(EXIT_FAILURE);
	}

	FILE *statsFile = NULL;

	long keys_press_count = 0;
	long key_press_count[KEYS_COUNT] = {0};
	size_t readEventSize;

	FILE *eventsFile = fopen(KEYBOARD_EVENTS_PATH, "r");
	if (eventsFile == NULL) {
		fputs("Events file is not open!", logFile);
		exit(EXIT_FAILURE);
	}

	fputs("Start daemon", logFile);
	
	Event *tmpEvent = (Event *) malloc(EVENT_SIZE);
	time_t startStatsCollectTime = time(NULL);
	time_t currentTime;
	while (1) {		
		readEventSize = fread(tmpEvent, EVENT_SIZE, 1, eventsFile);

		if ((tmpEvent->type == EV_KEY) && (tmpEvent->value == EV_KEY_PRESS_VALUE)) {
			logEvent(tmpEvent, logFile);
			keys_press_count += 1;
			key_press_count[tmpEvent->code] += 1;
		}

		currentTime = time(NULL);
		if (currentTime - startStatsCollectTime > statsTimeout) {
			statsFile = fopen(STATISTICS_PATH, "w");

			fprintf(statsFile, "\n\nTOTAL PRESS COUNT: %ld\n", keys_press_count);
			fprintf(statsFile, "KEY\t\tPRESS_COUNT\t\tPERCENT\n");

			for (i = 0; i < KEYS_COUNT; i++) {
				if (key_press_count[i] != 0) {
					fprintf(
						statsFile, 
						"%-15s\t\t%ld\t\t%.2f\n", 
						keyname(i), key_press_count[i], (key_press_count[i] / (double) keys_press_count) * 100
					);
					key_press_count[i] = 0;	
				}
			}

			startStatsCollectTime = currentTime;
			keys_press_count = 0;
			fclose(statsFile);
		}
	}

	fclose(eventsFile);
	free(tmpEvent);
	demonDeinit(logFile);
	exit(EXIT_SUCCESS);
}

#include <common-ee.h>
#include <kernel.h>
#include <string.h>

#define STACK_SIZE 0x8000
#define SPR_BEGIN 0x70000000
#define TEST_THREAD_PRIORITY 0x40

char nullThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void nullThreadProc(u32) {
	//Do nothing
}

char sleepingThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void sleepingThreadProc(u32) {
	SleepThread();
}

char waitingThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void waitingThreadProc(u32) {
	ee_sema_t dummySemaInfo;
	dummySemaInfo.init_count = 0;
	dummySemaInfo.max_count = 1;
	dummySemaInfo.option = 0;
	u32 dummySema = CreateSema(&dummySemaInfo);
	WaitSema(dummySema);
}

int createTestThread(void *entry, int prio, void *stack, u32 stackSize) {
	ee_thread_t threadParam;
	memset(&threadParam, 0, sizeof(ee_thread_t));
	threadParam.func = entry;
	threadParam.initial_priority = prio;
	threadParam.stack = stack;
	threadParam.stack_size = stackSize;
	return CreateThread(&threadParam);
}

void printThreadStatus(int threadId) {
	ee_thread_status_t threadStat;
	memset(&threadStat, 0, sizeof(ee_thread_status_t));
	int result = ReferThreadStatus(threadId, &threadStat);
	if(result >= 0) {
		schedf("succeeded -> result: %02x, init prio: %02x, current prio: %02x, status: %02x, wait type: %d\n", 
			result, threadStat.initial_priority, threadStat.current_priority, threadStat.status, threadStat.waitType);
	} else {
		schedf("failed -> result: %d\n", result);
	}
}

int main(int argc, char *argv[]) {

	printf("-- TEST BEGIN\n");

	{
		schedf("self thread:\n");

		schedf("  stat (with tid 0): ");
		printThreadStatus(0);

		schedf("  stat (with current tid): ");
		printThreadStatus(GetThreadId());
		
		flushschedf();
	}

	{
		schedf("low prio thread:\n");
		
		int threadId = createTestThread((void*)&nullThreadProc, TEST_THREAD_PRIORITY + 0x10, nullThreadStack, STACK_SIZE);
		schedf("  stat before start: ");
		printThreadStatus(threadId);

		StartThread(threadId, 0);
		schedf("  stat after start: ");
		printThreadStatus(threadId);
		
		SuspendThread(threadId);
		schedf("  stat after suspend: ");
		printThreadStatus(threadId);

		ResumeThread(threadId);
		schedf("  stat after resume: ");
		printThreadStatus(threadId);
		
		TerminateThread(threadId);
		schedf("  stat after terminate: ");
		printThreadStatus(threadId);
		
		DeleteThread(threadId);

		flushschedf();
	}
	
	{
		schedf("sleeping thread:\n");
		
		int threadId = createTestThread((void*)&sleepingThreadProc, TEST_THREAD_PRIORITY - 0x10, sleepingThreadStack, STACK_SIZE);

		StartThread(threadId, 0);
		schedf("  stat after start: ");
		printThreadStatus(threadId);
		
		SuspendThread(threadId);
		schedf("  stat after suspend: ");
		printThreadStatus(threadId);
		
		ResumeThread(threadId);
		schedf("  stat after resume: ");
		printThreadStatus(threadId);
		
		TerminateThread(threadId);
		DeleteThread(threadId);

		flushschedf();
	}
	
	//Waiting thread
	{
		schedf("waiting thread:\n");

		int threadId = createTestThread((void*)&waitingThreadProc, TEST_THREAD_PRIORITY - 0x10, waitingThreadStack, STACK_SIZE);

		StartThread(threadId, 0);
		schedf("  stat after start: ");
		printThreadStatus(threadId);
		
		SuspendThread(threadId);
		schedf("  stat after suspend: ");
		printThreadStatus(threadId);

		ResumeThread(threadId);
		schedf("  stat after resume: ");
		printThreadStatus(threadId);

		TerminateThread(threadId);
		DeleteThread(threadId);

		flushschedf();
	}

	//Various corner cases
	{
		schedf("corner cases:\n");
		
		schedf("  thread stat with invalid tid: ");
		printThreadStatus(~0);

		{
			int result = ReferThreadStatus(0, NULL);
			schedf("  self thread stat with null struct: %02x\n", result);
		}

		{
			ee_thread_status_t* threadStat = (ee_thread_status_t*)SPR_BEGIN;
			memset(threadStat, 0, sizeof(ee_thread_status_t));
			int result = ReferThreadStatus(0, threadStat);
			schedf("  self thread stat with struct in spr -> result: %02x, status: %02x\n", 
				result, threadStat->status);
		}
		
		flushschedf();
	}
	
	printf("-- TEST END\n");
	
	return 0;
}

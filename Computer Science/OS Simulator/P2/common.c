#include "common.h"

void process (ShmPTR_t *ShmPTR, int i) {
	int k, j = 0;										 // Local to each process 0..n
	int n = ShmPTR->nProc;
	do {
		ShmPTR->flag[i] = want_in;			 // Raise my flag		
		j  = ShmPTR->turn;						// Next Turn is selected from previous iteration
		while (j != i) {								// Check if you are eligible to have "turn" title 
			if(ShmPTR->flag[j] != idle) 			// Stay in the loop untill all other flags becomes idle till your 'i' j
				j = ShmPTR->turn;					// If they found non-idle flag, reset the j to go though the loop again
			else
				j = (j + 1)%n;						// If they found idle flag, move to next flag
		}
		ShmPTR->flag[i] = in_cs;				// Since all of flag are idle, declear you will be in CS

		for (j = 0; j < n; j++)					// But one more time to check whole flag before actually getting in
			if ((j != i)&& (ShmPTR->flag[j] == in_cs)) break;				// If they found process in CS, this, do while loop continues
	// This loop is to make sure that non of flags are in CS to avoid the infinite loop
	} while ((j < n) || (ShmPTR->turn != i && ShmPTR->flag[ShmPTR->turn] != idle));
	ShmPTR->turn = i;								// Declear it's your turn

	criticalSection(i);								// Entering ciritcal section);
	
	j = ((ShmPTR->turn)+1)%n;				// Move to next j to check if there is any non idle flag
	while (ShmPTR->flag[j] == idle) 		// Just skip idle flag
		j = (j+1)%n;
	
	ShmPTR->turn = j; 							// Found non idle flag, and store its j to 'turn' for next iteration
	ShmPTR->flag[i] = idle;						 // Make myself idle.
}

// Modifying File
void criticalSection (int curId) {
	char msg[50] = "";
	  
	FILE *fp;
	if (!(fp = fopen("cstest", "a"))) {
		perror("Slave: failed to open cstest");
		exit(1);
	}

	time_t now;
	struct tm *timeinfo;
	time(&now);
	timeinfo = localtime(&now);

	fprintf(fp, "File modified by process number %d at time", curId, msg);
	fprintf(fp, "\t%02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	fclose(fp);
}
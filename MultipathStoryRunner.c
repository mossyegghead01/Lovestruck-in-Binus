#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define RESET       "\x1b[0m"
#define BOLD        "\x1b[1m"
#define UNDERLINE   "\x1b[4m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"

// TODO: Comments...
// Yeah I'm on the verge of breaking down while writing this TODO
// Gosh I missed dynamic sized string

// Single-linked list structure for file line
typedef struct Line{
	char *line;
	struct Line *next;
} Line;

// Double-linked list structure for options
typedef struct Option{
	char *text;
	char *to;
	
	struct Option *next;
	struct Option *prev;
} Option;

// Double-linked list structure for game save
typedef struct Save{
	char *name;
	char *pos;
	
	struct Save *next;
	struct Save *prev;
} Save;

// Just... Settings
typedef struct Settings{
	Line *title;
	char *entryPoint;
} Settings;

// Endings reading
typedef struct Ending{
	char endingName[64];
	char endDesc[128];
	int found;
} Ending;

// Globals
Line *head = NULL;
Option *oHead = NULL;
Option *oTail = NULL;
Save *sHead = NULL;
Save *sTail = NULL;

// Clean the mess that is using malloc
void cleanup(){
	while (head){
		Line *temp = head;
		head = head->next;
		free(temp->line);
		free(temp);
	}
	
	if (oTail){
		// if Tail exist, it probably has link to head.
		// Making it null prevent the pointer from going around and try to free an already freed mem
		oTail->next = NULL;
	}
	
	while (oHead){
		Option *temp = oHead;
		oHead = oHead->next;
		free(temp->text);
		free(temp->to);
		free(temp);
	}
	
	head = NULL;
	oHead = NULL;
	oTail = NULL;
}

// clear!
void clear(){
	printf("\033[H\033[J");
	fflush(stdout);
}

// Wait... You can't pause a game!
// Oh wait it's just online games?
// Silly me :)
void pause(){
	printf("Press any key to continue...");
	getch();
	printf("\n");
}

// Get key and return a more readable value representation
// Only use for some special often-used keys.
// Example: Up and down key
int getKey(){
	int c = getch();
    if (c == 0 || c == 224){
        c = getch();
		if (c == 72){
			return 1; // Up
		}
		if(c == 80){
			return 2; // Down
		}
	}else if(c == 13){
		return 3; // Enter
	}else if (c == 's'){
		return 4; // 's' key
	}else if(c == 27){
		return 5; // Escape
	}
	return 0;
}

// Ask user if they understood the consequences of their action...
int confirmAction(const char *actionName){
	printf("Are you sure you want to %s (Y/N)?\n", actionName);
	char key;
	while (key != 'y' && key != 'n'){
		key = getch();
	}
	
	return (key == 'y') ? 1 : 0;
}

// Read file line and return pointer
// Why pointer you might ask?
// It's easier in some cases
char *readLine(FILE *fp, int replaceSymbols){
	int buffSize = 128;
	
	char *line = malloc(buffSize);
	strcpy(line, "");
	
	int pos = 0;
	int ch = 0;
	int flag = 0;
	while (ch != '\n'){
		ch = fgetc(fp);
		
		// Check if line is comment
		if (ch == '-' && replaceSymbols){
			int ch2 = fgetc(fp);
			if (ch2 == '-'){
				line[pos] = '\0';
				while ((ch = fgetc(fp)) != '\n');
				return line;
			}
			
			ungetc(ch2, fp);
		}
		
		// Just in case we're reading end of file
		if (ch == EOF){
			if (pos == 0){
				free(line);
				return NULL;
			}
			break;
		}
		
		// We need more RAM!
		if (pos + 5 >= buffSize - 1){
			buffSize *= 2; // To infinity and trash bin
			line = realloc(line, buffSize);
		}
		
		line[pos] = '\0';
		
		// printef() but cooler
		if (ch == '#' && !flag && replaceSymbols){
			flag = 1;
		}else if (flag){
			switch (ch){
				case 'r':
					strcat(line, RESET);
					pos += strlen(RESET);
					break;
				case 'b':
					strcat(line, BOLD);
					pos += strlen(BOLD);
					break;
				case 'u':
					strcat(line, UNDERLINE);
					pos += strlen(UNDERLINE);
					break;
				case 'R':
					strcat(line, RED);
					pos += strlen(RED);
					break;
				case 'G':
					strcat(line, GREEN);
					pos += strlen(GREEN);
					break;
				case 'Y':
					strcat(line, YELLOW);
					pos += strlen(YELLOW);
					break;
				case 'B':
					strcat(line, BLUE);
					pos += strlen(BLUE);
					break;
				case 'M':
					strcat(line, MAGENTA);
					pos += strlen(MAGENTA);
					break;
				case 'C':
					strcat(line, CYAN);
					pos += strlen(CYAN);
					break;
				case '#':
					line[pos++] = '#';
					break;
				default:
					line[pos++] = '#';
					line[pos++] = ch;
					break;
			}
			flag = 0;
		}else{
			line[pos++] = ch;
		}
	}
	
	line[pos] = '\0';
	return line;
}

// Pull the lever kronk
// proceed to pull saves instead
void pullSaves(int addEmpty){
	FILE *fp = fopen("saves.txt", "r");
	if (!fp){
		fp = fopen("saves.txt", "w");
		fp = freopen("saves.txt", "r", fp);
	}
	
	char *line;
	while ((line = readLine(fp, 0)) != NULL){
		printf("%s\n", line);
		int nIdx = 1;
		int len = strlen(line);
		char *original = line;
		
		while (*(line) != '#' && *(line) != '\0'){
			nIdx++;
			line++;
		}
		
		line = original;
		
		char *name = malloc(nIdx + 1);
		int i;
		for (i = 0; i < nIdx - 1; i++){
			name[i] = *(line++);
		}
		name[nIdx - 1] = '\0';
		
		line = original;
		
		char *pos = malloc(len - nIdx + 1);
		strcpy(pos, line + nIdx);
		pos[len - nIdx - 1] = '\0';
		
		line = original;
		free(line);
		
		Save *sv = malloc(sizeof(Save));
		sv->name = name;
		sv->pos = pos;
		sv->next = NULL;
		sv->prev = NULL;
		
		if (!sHead && !sTail){
			sHead = sv;
			sTail = sv;
			
			sv->next = sv;
			sv->prev = sv;
		}else{
			sTail->next = sv;
			sv->prev = sTail;
			
			sv->next = sHead;
			sHead->prev = sv;
			
			sTail = sv;
		}
	}
	fclose(fp);
	
	if (addEmpty){
		Save *sv = malloc(sizeof(Save));
		char *tName = malloc(strlen("New Save") + 1);
		strcpy(tName, "New Save");
		sv->name = tName;
		sv->pos = NULL;
		sv->next = NULL;
		sv->prev = NULL;
		
		if (!sHead && !sTail){
			sHead = sv;
			sTail = sv;
			
			sv->next = sv;
			sv->prev = sv;
		}else{
			sTail->next = sv;
			sv->prev = sTail;
			
			sv->next = sHead;
			sHead->prev = sv;
			
			sTail = sv;
		}
	}
}

// Wrong levahhhhhhhh
// And flushed
void saveAndClean(){
	FILE *fp = fopen("saves.txt", "w");
	
	if (sHead != NULL){
		Save *curr = sHead;
		do{
			if (curr->pos){
				fprintf(fp, "%s#%s\n", curr->name, curr->pos);
			}
			curr = curr->next;
		} while (curr != sHead);
		fclose(fp);
	
		if (sTail){
			sTail->next = NULL;
		}
		
		while (sHead){
			Save *temp = sHead;
			sHead = sHead->next;
			free(temp->name);
			free(temp->pos);
			free(temp);
		}
	}
	
	sHead = NULL;
	sTail = NULL;
}

// Save menu
void saveGame(const char *file){
	pullSaves(1);
	
	Save *sel = sHead;
	int r = 1;
	int c = 0;
	while (r){
		clear();
		printf("Save Game\n");
		
		int i = 1;
		Save *curr = sHead;
		do{
			printf("%s %s%d. %s\n" RESET, curr == sel ? GREEN ">>" : "  ", curr == sel ? UNDERLINE : "", i++, curr->name);
			printf("\n%s[Arrow Up/Down]%s: Cycle selection %s[ENTER]%s: Confirm selection %s[Esc]%s: Cancel saving\n", BOLD YELLOW, RESET, BOLD YELLOW, RESET, BOLD YELLOW, RESET);
			curr = curr->next;
		}while (curr != sHead);
		
		switch (getKey()){
			case 1:
				sel = sel->prev;
				break;
			case 2:
				sel = sel->next;
				break;
			case 3:
				if (sel->pos){
					r = !confirmAction("override this save");
				}else{
					r = 0;
				}
				c = !r;
				break;
			case 5:
				c = 0;
				r = 0;
				break;
			case 6:
				break;
		}
	}
	
	if (c){
		printf("Enter save name: ");
		int buff = 128;
		char *name = malloc(buff);
		
		char ch;
		int pos = 0;
		while ((ch = getchar()) != '\n'){
			if (ch == EOF){
				break;
			}
			
			if (pos >= buff - 1){
				buff *= 2;
				
				name = realloc(name, buff);
			}
			
			name[pos++] = ch;
		}
		
		name[pos] = '\0';
		free(sel->name);
		sel->name = name;
		
		char *temp = malloc(strlen(file) + 1);
		strcpy(temp, file);
		sel->pos = temp;
	}
	
	saveAndClean();
}

// discover ending then return its desc
Ending *discoverEnding(const char *endingName){
	FILE *fp = fopen("endings.txt", "r");
	
	int buff = 20;
	Ending **endings = malloc(buff * sizeof(Ending *));
	
	int prevFound = 0;
	int i = 0;
	int f = -1;
	char c;
	while ((c = getc(fp)) != EOF){
		ungetc(c, fp);
		if (i + 2 >= buff){
			buff *= 2;
			realloc(endings, buff * sizeof(Ending *));
		}
		
		endings[i] = malloc(sizeof(Ending));
		
		fscanf(fp, "%[^#]#%[^#]#%d\n", endings[i]->endingName, endings[i]->endDesc, &endings[i]->found);
		// Technically speaking, this is linear search
		// Not in the traditional sense tho
		if (strcmp(endings[i]->endingName, endingName) == 0) {
			f = i;
			prevFound = endings[i]->found;
			endings[i]->found = 1;
		}
		i++;
	}
	
	fclose(fp);
	
	Ending *result;
	if (f != -1){
		result = malloc(sizeof(Ending));
		*result = *endings[f];
		result->found = prevFound;
	}
	
	int j;
	fp = fopen("endings.txt", "w");
	
	for (j = 0; j < i; j++){
		fprintf(fp, "%s#%s#%d\n", endings[j]->endingName, endings[j]->endDesc, endings[j]->found);
		free(endings[j]);
	}
	
	free(endings);
	fclose(fp);
	
	if (f == -1) {
		return NULL;
	}else{
		return result;
	}
}

// The heart of the game
void storyRunner(const char *fileName){
	// Need to re-reserve memory for file name because cleanup will free it
	// It's quite miraculous that the cleanup didn't cause issue earlier tbh
	char *file = malloc(strlen(fileName) + 1);
	strcpy(file, fileName);
	
	clear();
	cleanup(); // Here due to the recursive nature of this function
	
	// Read ending
	if (file[0] == 'e' && file[1] == 'n' && file[2] == 'd'){
		Ending *ending = discoverEnding(file + 4);
		if (ending){
			printf("%s\n%s\n", ending->endingName, ending->endDesc);
			if (!ending->found){
				printf("%s[NEW ENDING!]%s\n", BOLD YELLOW, RESET);
			}
			free(ending);
		}else{ //fallback, read like normal
			printf("%s\nNo Description\n", file + 4);
		}
		pause();
		cleanup(); // Better try cleaning up than not amirite?
		return;
	}
	
	// Construct file full path
	char *fullPath = malloc(strlen(file) + strlen("./story/") + 1);
	strcpy(fullPath, "");
	strcat(fullPath, "./story/");
	strcat(fullPath, file);
	
	// Open the file
	FILE *fp = fopen(fullPath, "r");
	// Well, I'm pretty sure we're going to encounter these a lot...
	if (fp == NULL){
		printf("Seems like this part of the story is missing!\n");
		printf("Why not return later?\n");
		printf("(Hopefully you remembered to save in the previous page)\n");
		printf("Missing file: %s\n", fullPath); // Debug, remove or use flag later
		free(fullPath);
		fclose(fp);
		pause();
		return;
	}
	
	// Reading file
	int optMode = 0;
	char *line;
	while ((line = readLine(fp, !optMode)) != NULL){
		if (strcmp(line, "#opt\n") != 0 && !optMode){ // Read until option
			if (strcmp(line, "") != 0){
				Line *nl = malloc(sizeof(Line));
				
				nl->line = line;
				nl->next = NULL;
				
				// You might be familiar with this
				// Yep, it's pushTail for single-linked list
				// Why not double-linked list?
				// Well, you see, there's no reason for us to get prev
				// We always move forward here
				if (head == NULL){
					head = nl;
				}else{
					Line *curr = head;
					while (curr->next != NULL){
						curr = curr->next;
					}
					
					curr->next = nl;
				}
			}
		}else if (optMode){ // read options
			int txtIdx = 1;
			int len = strlen(line);
			char *original = line;
			
			while (*(line) != '#' && *(line) != '\0'){
				txtIdx++;
				line++;
			}
			
			line = original;
			
			char *txt = malloc(txtIdx + 1);
			int i;
			for (i = 0; i < txtIdx - 1; i++){
				txt[i] = *(line++);
			}
			txt[txtIdx - 1] = '\0';
			
			line = original;
			
			char *dest = malloc(len -  txtIdx + 1);
			strcpy(dest, line + txtIdx);
			dest[len - txtIdx - 1] = '\0';
			
			line = original;
			
			free(line);
			
			Option *opt = malloc(sizeof(Option));
			opt->text = txt;
			opt->to = dest;
			opt->next = NULL;
			opt->prev = NULL;
			
			// This also might be familiar
			// It's still pushTail, but double-linked list!
			if (!oHead && !oTail){
				oHead = opt;
				oTail = opt;
				
				opt->next = opt;
				opt->prev = opt;
			}else{
				oTail->next = opt;
				opt->prev = oTail;
				
				opt->next = oHead;
				oHead->prev = opt;
				
				oTail = opt;
			}
		}else{ // change flag
			optMode = 1;
		}
	}
	fclose(fp);
	
	int sel = 1;
	Option *selCurr = oHead;
	Option *oCurr = oHead;
	int r = 1;
	int q = 0;
	while (r){
		clear();
		
		Line *curr = head;
		while (curr != NULL){
			printf("%s", curr->line);
			curr = curr->next;
		}
		
		int i = 1;
		do {
			printf("%s %s%d. %s\n" RESET, oCurr == selCurr ? GREEN ">>" : "  ", oCurr == selCurr ? UNDERLINE : "", i++, oCurr->text);
			oCurr = oCurr->next;
		} while (oCurr != oHead);
		printf("\n%s[Arrow Up/Down]%s: Cycle selection %s[ENTER]%s: Confirm selection %s[S]%s: Save game %s[Esc]%s: Return to main menu\n", BOLD YELLOW, RESET, BOLD YELLOW, RESET, BOLD YELLOW, RESET, BOLD YELLOW, RESET);
		
		switch (getKey()){
			case 1:
				selCurr = selCurr->prev;
				break;
			case 2:
				selCurr = selCurr->next;
				break;
			case 3:
				r = 0;
				break;
			case 4:
				saveGame(file);
				break;
			case 5:
				q = confirmAction("return to main menu");
				r = !q;
		}
	}
	
	free(file);
	if (!q){
		storyRunner(selCurr->to);
	}
	cleanup(); // Cleaning up for when we no longer need to call recursively
}

// Load me up
// When november ends
void loadGame(){
	pullSaves(0);
	
	Save *sel = sHead;
	int r = 1;
	int c = 0;
	while (r){
		clear();
		printf("Saved Games\n");
		
		int i = 1;
		// I originally forgot to handle the case where no save exist
		// As bandaid as this looks, this is final
		if (sHead != NULL){
			Save *curr = sHead;
			do{
				printf("%s %s%d. %s\n" RESET, curr == sel ? GREEN ">>" : "  ", curr == sel ? UNDERLINE : "", i++, curr->name);
				printf("\n%s[Arrow Up/Down]%s: Cycle selection %s[ENTER]%s: Confirm selection %s[Esc]%s: Return to main menu\n", BOLD YELLOW, RESET, BOLD YELLOW, RESET, BOLD YELLOW, RESET);
				curr = curr->next;
			}while (curr != sHead);
			
			switch (getKey()){
				case 1: // Up
					sel = sel->prev;
					break;
				case 2: // Down
					sel = sel->next;
					break;
				case 3: // Enter
					r = 0;
					c = 1;
					break;
				case 5: // Escape (No loading) :(
					r = 0;
					break;
			}
		}else{
			printf("There are no saved game\n");
			r = 0;
			pause();
		}
	}
	
	char *temp;
	if (c){ // I need to clean sHead and sTail, so I needed to copy the value before passing
		temp = malloc(strlen(sel->pos) + 1);
		strcpy(temp, sel->pos);
	}
	
	saveAndClean();
	
	if (c){ // And into the story we go
		storyRunner(temp);
		free(temp);
	}
}

// NOOOooooo.....
void deleteSave(){
	pullSaves(0);
	
	Save *sel = sHead;
	int r = 1;
	int c = 0;
	while (r){
		clear();
		printf("Saved Games\n");
		
		int i = 1;
		// I originally forgot to handle the case where no save exist
		// As bandaid as this looks, this is final
		if (sHead != NULL){
			Save *curr = sHead;
			do{
				printf("%s %s%d. %s\n" RESET, curr == sel ? GREEN ">>" : "  ", curr == sel ? UNDERLINE : "", i++, curr->name);
				printf("\n%s[Arrow Up/Down]%s: Cycle selection %s[ENTER]%s: Confirm selection %s[Esc]%s: Return to main menu\n", BOLD YELLOW, RESET, BOLD YELLOW, RESET, BOLD YELLOW, RESET);
				curr = curr->next;
			}while (curr != sHead);
			
			switch (getKey()){
				case 1: // Up
					sel = sel->prev;
					break;
				case 2: // Down
					sel = sel->next;
					break;
				case 3: // Left (Wait what... No! This is enter!)
					c = confirmAction("delete this save");
					r = !c;
					break;
				case 5: // Right (Not again... we just simply cancels here)
					r = 0;
					break;
			}
		}else{
			printf("There are no saved game\n");
			r = 0;
			pause();
		}
	}
	
	// No this isn't complicated
	// This is popMid for doubly-linked list
	if (c){
		if (sel == sHead && sel == sTail){
			free(sel);
			sHead = NULL;
			sTail = NULL;
		}else if (sel == sHead){
			sHead = sHead->next;
			sHead->prev = sTail;
			sTail->next = sHead;
			free(sel);
		}else if (sel == sTail){
			sTail = sTail->prev;
			sTail->next = sHead;
			sHead->prev = sTail;
		}else{
			sel->prev->next = sel->next;
			sel->next->prev = sel->prev;
			free(sel);
		}
	}
	
	saveAndClean();
}

Settings readSettings(){
	Settings settings = {NULL, NULL};
	FILE *fp = fopen("settings.txt", "r");
	
	char *line;
	int appendTo = 0;
	
	// Currently disallow replacing symbols due to shared code with path
	while ((line = readLine(fp, 0)) != NULL){
		if (strcmp(line, "#start TITLE\n") == 0){
			appendTo = 1;
        }else if (strcmp(line, "#start ENTRY_POINT\n") == 0){
			appendTo = 2;
		}else if (strcmp(line, "#end\n") == 0){
			appendTo = 0;
		}else{
			if (appendTo == 1) {
				Line *newLine = malloc(sizeof(Line));
				
				newLine->line = line;
				newLine->next = NULL;
				
				if (settings.title == NULL) {
					settings.title = newLine;
				}else{
					Line *curr = settings.title;
					while (curr->next != NULL){
						curr = curr->next;
					}
					curr->next = newLine;
				}
			}else if (appendTo == 2){
				line[strlen(line) - 1] = '\0';
				settings.entryPoint = line;
			}else{
				free(line);
			}
		}
	}
	
	fclose(fp);
	return settings;
}

int main(){
	int s = 0;
	int r = 1;
	
	Settings settings = readSettings();
	
	do {
		clear();
		
		// Title screen
		Line *curr = settings.title;
		while (curr != NULL){
			printf("%s", curr->line);
			curr = curr->next;
		}
		printf("%s  %sNew Game\n" RESET, (s == 0) ? GREEN ">>" : "  ", (s == 0) ? UNDERLINE : "");
		printf("%s  %sLoad Game\n" RESET, (s == 1) ? GREEN ">>" : "  ", (s == 1) ? UNDERLINE : "");
		printf("%s  %sDelete Save\n" RESET, (s == 2) ? GREEN ">>" : "  ", (s == 2) ? UNDERLINE : "");
		printf("%s  %sUnlocked Endings\n" RESET, (s == 3) ? GREEN ">>" : "  ", (s == 3) ? UNDERLINE : "");
		printf("%s  %sQuit\n" RESET, (s == 4) ? GREEN ">>" : "  ", (s == 4) ? UNDERLINE : "");
		printf("\n%s[Arrow Up/Down]%s: Cycle selection %s[ENTER]%s: Confirm selection\n", BOLD YELLOW, RESET, BOLD YELLOW, RESET);
		
		// Wait for user input
		switch(getKey()){
			case 1: // Move menu pointer up
				--s;
				if (s < 0) s = 4;
				break;
			case 2: // Move menu pointer down
				++s;
				if (s > 4) s = 0;
				break;
			case 3: // Enter pressed
				switch (s){
					case 0: // New Game
						storyRunner(settings.entryPoint);
						s = 0;
						break;
					case 1: // Load Game
						loadGame();
						s = 0;
						break;
					case 2: // Delete Save
						deleteSave();
						s = 0;
						break;
					case 3: // Unlocked Endings
						// TODO: Make list of ending
						printf("Coming soon\n");
						pause();
						s = 0;
						break;
					case 4: // Quit
						clear();
						r =  !confirmAction("quit");
						s = 0;
						break;
				}
				break;
			default: // Any other key
				break;
		}
	}while (r);
	
	Line *curr = settings.title;
	while (curr != NULL){
		Line *temp = curr;
		curr = curr->next;
		free(temp);
	}
	
	free(settings.entryPoint);
	
	clear();
	printf(BOLD BLUE "See yoou later!\n" RESET); // Goodbye :)
	
	return 0;
}


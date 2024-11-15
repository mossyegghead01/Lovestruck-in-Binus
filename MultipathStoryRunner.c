#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define pause() system("pause")
#define clear() system("cls")

#define RESET       "\x1b[0m"
#define BOLD        "\x1b[1m"
#define UNDERLINE   "\x1b[4m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"

typedef struct Option{
	char optText[128];
	char optDest[40];
} Option;

typedef struct Save{
	char saveName[128];
	char saveDest[20];
} Save;

typedef struct Setting{
	char title[256];
	char init[50];
} Setting;

int getKey(){
	int c = getch();
    if (c == 0 || c == 224){
        c = getch();
		if (c == 72){
			return 1;
		}
		if(c == 80){
			return 2;
		}
	}else if(c == 13){
		return 3;
	}else if(c == 27){
		return 4;
	}
	return 0;
}

void delay(int ms){
    clock_t st = clock();
    
    while (clock() < st + ms);
}

void printef(char c){
	switch (c){
    	case 'r':
    		printf(RESET);
    		break;
    	case 'b':
    		printf(BOLD);
    		break;
    	case 'u':
    		printf(UNDERLINE);
    		break;
    	case 'R':
    		printf(RED);
    		break;
    	case 'G':
    		printf(GREEN);
    		break;
    	case 'Y':
    		printf(YELLOW);
    		break;
    	case 'B':
    		printf(BLUE);
    		break;
    	case 'M':
    		printf(MAGENTA);
    		break;
    	case 'C':
    		printf(CYAN);
    		break;
    	case '#':
    		printf("#");
    		delay(50);
    		break;
    	default:
    		printf("#");
    		delay(50);
    		printf("%c", c);
    		delay(50);
	}
}

void storyRunner(const char *pos);

void saveGame(const char *pos){
	FILE *fp = fopen("saves.txt", "r");
	if (!fp){
		fp = fopen("saves.txt", "w+");
	}
	int i;
	
	Save saves[5];
	for (i = 0; i < 5; i++){
		strcpy(saves[i].saveDest, "EMPTY");
		strcpy(saves[i].saveName, "EMPTY");
	}
	
	int si = 0;
	int ch;
	while ((ch = fgetc(fp)) != EOF && si < 5){
		ungetc(ch, fp);
		Save save;
		fscanf(fp, "%[^#]#%[^\n]\n", save.saveName, save.saveDest);
		saves[si++] = save;
	}
	
	fclose(fp);
	
	int s = 0;
	int r = 1;
	while (r){
		clear();
		for (i = 0; i < 5; i++){
			if (strcmp(saves[i].saveName, "EMPTY") == 0 && strcmp(saves[i].saveDest, "EMPTY") == 0){
				printf("%s %s%d. Empty Save\n" RESET, (s == i) ? GREEN ">>" : "  ", (s == i) ? UNDERLINE : "", i + 1);
			}else{
				printf("%s %s%d. %s\n" RESET, (s == i) ? GREEN ">>" : "  ", (s == i) ? UNDERLINE : "", i + 1, saves[i].saveName);
			}
		}
		
		int o = 1;
		switch(getKey()){
			case 0:
				break;
			case 1:
				--s;
				if (s < 0) s = 4;
				break;
			case 2:
				++s;
				if (s > 4) s = 0;
				break;
			case 3:
				if (strcmp(saves[s].saveName, "EMPTY") != 0 && strcmp(saves[s].saveDest, "EMPTY") != 0){
					printf("This slot already has save in it, are you sure you want to overwrite it (Y|N)?\n");
					while (1){
						char c = getch();
						if (c == 'y' || c == 'n'){
							o = (c == 'y') ? 1 : 0;
							break;
						}
					}
				}
				if (o){
					r = 0;
				}
				break;
			case 4:
				storyRunner(pos);
				return;
		}
	}
	
	char sName[128];
	while (1){
		printf("Please enter a save name: ");
		scanf(" %[^\n]", sName);
		if (strlen(sName) < 3){
			printf("Save name must be at least 3 characters in length!\n");
			pause();
		}else{
			sName[127] = '\0';
			break;
		}
	}
	strcpy(saves[s].saveName, sName);
	strcpy(saves[s].saveDest, pos);
	
	fp = fopen("saves.txt", "w");
	for (i = 0; i < 5; i++){
		fprintf(fp, "%s#%s\n", saves[i].saveName, saves[i].saveDest);
	}
	
	printf("Your progress have been saved!\n");
	
	fclose(fp);
	pause();
}

void storyRunner(const char *pos) {
	clear();
	if (pos[0] == 'e' && pos[1] == 'n' && pos[2] =='d'){
		printf("You've reached the end of the story!\n");
		printf("Ending reached: %s \n", pos + 4);
		pause();
		return;
	}
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "./story/%s", pos);

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
		printf("The story ends here... for now.\n");
		printf("But, there'll be more content in the future!\n");
		printf("Though I hope you remembered to save!\n");
        pause();
        return;
    }
    
    char lines[512][512];
    int li = 0;
    Option options[10];
    int opti = 0;
    
    int optm = 0;
    int ch;
    while ((ch = fgetc(fp)) != EOF){
    	ungetc(ch, fp);
    	
    	char line[512] = "";
    	if (!optm){
			if (fgets(line, sizeof(line), fp) != NULL) {
				int len = strlen(line);
				if (len > 0 && line[len - 1] != '\n') {
					line[len] = '\n';
					line[len + 1] = '\0';
				}
			}
		}else{
			Option opt;
			fscanf(fp, "%[^#]#%[^\n]\n", opt.optText, opt.optDest);
			options[opti++] = opt;
		}
		
		if (strcmp(line, "#opt\n") == 0){
    		optm = 1;
		}else if (line[0] != '-' && line[1] != '-'){
			strcpy(lines[li++], line);
		}
	}
    
    fclose(fp);
    
    int i;
    int j;
    int formatting = 0;
    int st = 0;
    printf(RESET);
    for (i = 0; i < li; i++){
    	for (j = 0; j < strlen(lines[i]); j++){
    		if (formatting){
    			formatting = 0;
    			printef(lines[i][j]);
			}else if (lines[i][j] == '#'){
    			formatting = 1;
			}else{
				if (kbhit()){
					getch();
					st = 1;
				}
				
				if (st){
					break;
				}
				
				printf("%c", lines[i][j]);
				fflush(stdout);
				delay(50);
			}
		}
	}
	
	
	int s = 0;
	int r = 1;
	printf(RESET);
	while (r){
		clear();
		int i;
		int j;
		int formatting = 0;
		for (i = 0; i < li; i++){
			for (j = 0; j < strlen(lines[i]); j++){
				if (formatting){
					formatting = 0;
					printef(lines[i][j]);
				}else if (lines[i][j] == '#'){
    				formatting = 1;
				}else{
					printf("%c", lines[i][j]);
				}
			}
		}
		
		for (i = 0; i < opti; i++){
			printf("%s %s%d. %s\n" RESET, (s == i) ? GREEN ">>" : "  ", (s == i) ? UNDERLINE : "", i + 1, options[i].optText);
		}
		
		switch(getKey()){
			case 0:
				break;
			case 1:
				--s;
				if (s < 0) s = opti - 1;
				break;
			case 2:
				++s;
				if (s > opti - 1) s = 0;
				break;
			case 3:
				r = 0;
				break;
			case 4:
				r = 0;
				saveGame(pos);
				return;
		}
	}
    storyRunner(options[s].optDest);
}

void loadGame(){
	clear();
	FILE *fp = fopen("saves.txt", "r");
	if (!fp){
		fp = fopen("saves.txt", "w+");
	}
	int i;
	
	Save saves[5];
	for (i = 0; i < 5; i++){
		strcpy(saves[i].saveDest, "EMPTY");
		strcpy(saves[i].saveName, "EMPTY");
	}
	
	int si = 0;
	int ch;
	while ((ch = fgetc(fp)) != EOF && si < 5){
		ungetc(ch, fp);
		Save save;
		fscanf(fp, "%[^#]#%[^\n]\n", save.saveName, save.saveDest);
		saves[si++] = save;
	}
	
	fclose(fp);
	
	int s = 0;
	int r = 1;
	while (r){
		clear();
		for (i = 0; i < 5; i++){
			if (strcmp(saves[i].saveName, "EMPTY") == 0 && strcmp(saves[i].saveDest, "EMPTY") == 0){
				printf("%s %s%d. Empty Save\n" RESET, (s == i) ? GREEN ">>" : "  ", (s == i) ? UNDERLINE : "", i + 1);
			}else{
				printf("%s %s%d. %s\n" RESET, (s == i) ? GREEN ">>" : "  ", (s == i) ? UNDERLINE : "", i + 1, saves[i].saveName);
			}
		}
		switch(getKey()){
			case 0:
				break;
			case 1:
				--s;
				if (s < 0) s = 4;
				break;
			case 2:
				++s;
				if (s > 4) s = 0;
				break;
			case 3:
				if (strcmp(saves[s].saveName, "EMPTY") == 0 && strcmp(saves[s].saveDest, "EMPTY") == 0){
					printf("This slot doesn't have any save!\n");
					pause();
				}else{
					storyRunner(saves[s].saveDest);
					return;
				}
				break;
			case 4:
				return;
		}
	}
}

void deleteSave(){
	clear();
	FILE *fp = fopen("saves.txt", "r");
	if (!fp){
		fp = fopen("saves.txt", "w+");
	}
	int i;
	
	Save saves[5];
	for (i = 0; i < 5; i++){
		strcpy(saves[i].saveDest, "EMPTY");
		strcpy(saves[i].saveName, "EMPTY");
	}
	
	int si = 0;
	int ch;
	while ((ch = fgetc(fp)) != EOF && si < 5){
		ungetc(ch, fp);
		Save save;
		fscanf(fp, "%[^#]#%[^\n]\n", save.saveName, save.saveDest);
		saves[si++] = save;
	}
	
	fclose(fp);
	
	int s = 0;
	int r = 1;
	while (r){
		clear();
		for (i = 0; i < 5; i++){
			if (strcmp(saves[i].saveName, "EMPTY") == 0 && strcmp(saves[i].saveDest, "EMPTY") == 0){
				printf("%s %s%d. Empty Save\n" RESET, (s == i) ? GREEN ">>" : "  ", (s == i) ? UNDERLINE : "", i + 1);
			}else{
				printf("%s %s%d. %s\n" RESET, (s == i) ? GREEN ">>" : "  ", (s == i) ? UNDERLINE : "", i + 1, saves[i].saveName);
			}
		}
		switch(getKey()){
			case 0:
				break;
			case 1:
				--s;
				if (s < 0) s = 4;
				break;
			case 2:
				++s;
				if (s > 4) s = 0;
				break;
			case 3:
				if (strcmp(saves[s].saveName, "EMPTY") == 0 && strcmp(saves[s].saveDest, "EMPTY") == 0){
					printf("This slot doesn't have any save!\n");
					pause();
				}else{
					r = 0;
					break;
				}
				break;
			case 4:
				return;
		}
	}
	
	int o;
	printf("Are you sure you want to delete this save (Y|N)?\n");
	while (1){
		char c = getch();
		if (c == 'y' || c == 'n'){
			o = (c == 'y') ? 1 : 0;
			break;
		}
	}
	if (o){
		strcpy(saves[s].saveDest, "EMPTY");
		strcpy(saves[s].saveName, "EMPTY");
		fp = fopen("saves.txt", "w");
		for (i = 0; i < 5; i++){
			fprintf(fp, "%s#%s\n", saves[i].saveName, saves[i].saveDest);
		}
		
		printf("Your save has been deleted!\n");
		pause();
	}
	fclose(fp);
	return;
}

Setting readSettings(){
	FILE *fp = fopen("settings.txt", "r");
	
	Setting setting;
	strcpy(setting.title, "");
	strcpy(setting.init, "");
	
	if (!fp){
		printf(RED BOLD "WARNING: Could not find game setting!" RESET);
		return setting;
	}
	
	char title[256] = "";
	int titleM = 1;
	int ch;
	while ((ch = fgetc(fp)) != EOF){
		ungetc(ch, fp);
		char temp[256];
		fscanf(fp, "%[^\n]\n", temp);
		
		if (strcmp(temp, "#end") == 0){
			if (titleM){
				titleM = 0;
			}else{
				break;
			}
		}
		
		if (titleM){
			strcat(title, temp);
			strcat(title, "\n");
		}else{
			strcpy(setting.init, temp);
		}
	}
	strcpy(setting.title, title);
	
	fclose(fp);
	
	return setting;
}

int main(){
	int s = 1;
	int i;
	int t = 0;
	
	Setting setting = readSettings();
	
	do{
		clear();
		printf("%s\n", setting.title);
		printf("%s  %sNew Game\n" RESET, (s == 1) ? GREEN ">>" : "  ", (s == 1) ? UNDERLINE : "");
		printf("%s  %sLoad Game\n" RESET, (s == 2) ? GREEN ">>" : "  ", (s == 2) ? UNDERLINE : "");
		printf("%s  %sDelete Save\n" RESET, (s == 3) ? GREEN ">>" : "  ", (s == 3) ? UNDERLINE : "");
		printf("%s  %sQuit\n" RESET, (s == 4) ? GREEN ">>" : "  ", (s == 4) ? UNDERLINE : "");
		
		switch(getKey()){
			case 0:
				break;
			case 1:
				--s;
				if (s < 1) s = 4;
				break;
			case 2:
				++s;
				if (s > 4) s = 1;
				break;
			case 3:
				if (s == 4){
					t = 1;
				}else if (s == 1){
					storyRunner(setting.init);
				}else if (s == 2){
					loadGame();
				}else{
					deleteSave();
				}
				break;
			case 4:
				t = 1;
				break;
		}
	}while (!t);
	clear();
	printf("See you next time!\n");
	
	return 0;
}


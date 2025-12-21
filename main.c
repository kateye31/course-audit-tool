#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_RECORDS 2000
#define LINE_SIZE 600

typedef struct {
    int number;
    char coursePrefix[10];
    int courseCode;
    char courseTitle[200];
    char college[150];
} Course;

typedef struct {
    char acronym[10];
    char fullName[150];
} CollegeMap;

// map of allowed acronyms
CollegeMap collegeList[] = {
    {"DSC",  "Daytona State College"},
    {"SSC",  "Seminole State College"},
    {"LSSC", "Lake-Sumter State College"},
    {"MDC",  "Miami Dade College"},
    {"PPSC", "Polk State College"},
    {"SPC",  "St. Petersburg College"},
    {"HCC",  "Hillsborough Community College"},
    {"IRSC", "Indian River State College"},
    {"CCF", "College of Central Florida"},
    {"V", "Valencia College"},
    {"EFSC", "Eastern Florida State College"}
};
int collegeCount = sizeof(collegeList) / sizeof(collegeList[0]);

void trim(char *s) {
    char *end;
    while (isspace((unsigned char)*s)) s++;
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

char *expandCollege(char *input) {
    for (int i = 0; i < collegeCount; i++) {
        if (strcasecmp(input, collegeList[i].acronym) == 0)
            return collegeList[i].fullName;
    }
    return input; // return original if not acronym
}

int parseLine(char *line, Course *c) {
    char *token, *dash;
    char temp[400];
    char *copy = strdup(line);

    token = strtok(copy, "\t");
    if (!token) return 0;
    c->number = atoi(token);

    token = strtok(NULL, "\t");
    if (!token) return 0;
    strcpy(temp, token);

    sscanf(temp, "%s %d", c->coursePrefix, &c->courseCode);

    dash = strchr(temp, '-');
    if (dash) {
        dash++;
        while (isspace(*dash)) dash++;
        strcpy(c->courseTitle, dash);
        trim(c->courseTitle);
    } else {
        strcpy(c->courseTitle, "");
    }

    strtok(NULL, "\t");
    token = strtok(NULL, "\t");
    if (token) {
        strcpy(c->college, token);
        trim(c->college);
    } else {
        strcpy(c->college, "");
    }

    free(copy);
    return 1;
}

int main() {
    FILE *fp = fopen("courses.txt", "r");
    if (!fp) {
        perror("Could not open courses.txt");
        return 1;
    }

    Course courses[MAX_RECORDS];
    int count = 0;
    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), fp) && count < MAX_RECORDS) {
        if (strlen(line) > 1 && isdigit(line[0])) {
            if (parseLine(line, &courses[count])) count++;
        }
    }
    fclose(fp);

    char prefix[10], collegeInput[150], correctTitle[200];
    int code;
    int validCourse = 0;

    Course *targetCourse = NULL;

    // keep asking until valid course found
    while (!validCourse) {
        printf("\nEnter course prefix (e.g., ACG): ");
        scanf("%s", prefix);

        printf("Enter course code (e.g., 2021): ");
        scanf("%d", &code);
        getchar();

        printf("Enter college (full name or acronym): ");
        fgets(collegeInput, sizeof(collegeInput), stdin);
        trim(collegeInput);

        // expand acronym to full name if needed
        char *collegeFull = expandCollege(collegeInput);

        // search for this course in dataset
        for (int i = 0; i < count; i++) {
            if (strcasecmp(courses[i].coursePrefix, prefix) == 0 &&
                courses[i].courseCode == code &&
                strcasecmp(courses[i].college, collegeFull) == 0) {
                validCourse = 1;
                targetCourse = &courses[i];
                break;
            }
        }

        if (!validCourse) {
            printf("\n⚠️  Course not found in dataset. Please try again.\n");
        } else {
            strcpy(collegeInput, collegeFull);
        }
    }

    printf("\nFor %s %d at %s, provide the correct course title: ",
           prefix, code, collegeInput);
    fgets(correctTitle, sizeof(correctTitle), stdin);
    trim(correctTitle);

    printf("\nChecking for mismatches...\n");
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strcasecmp(courses[i].coursePrefix, prefix) == 0 &&
            courses[i].courseCode == code &&
            strcasecmp(courses[i].college, collegeInput) == 0) {

            if (strcmp(courses[i].courseTitle, correctTitle) != 0) {
                printf("❌ Record #%d mismatch: \"%s\"\n",
                       courses[i].number, courses[i].courseTitle);
                found = 1;
            }
        }
    }

    if (!found)
        printf("✅ All records for %s %d at %s match exactly.\n", prefix, code, collegeInput);

    return 0;
}



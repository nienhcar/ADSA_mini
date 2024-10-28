#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define d 256
#define q 101
#define MAX_TEXT_SIZE 1000
#define MAX_PATTERN_SIZE 100
#define MAX_PATTERNS 10

// Function to calculate potential based on previous hash and comparison counts
int calculatePotential(int hashCalculations, int characterComparisons) {
    return hashCalculations + characterComparisons; // Simple potential function
}

void rabinKarp(const char *text, const char *pattern, int caseSensitive, int *totalHashCalculations, int *totalCharacterComparisons, int *totalPotential) {
    int M = strlen(pattern);
    int N = strlen(text);
    int p = 0, t = 0, h = 1;
    int hashCalculations = 0;
    int characterComparisons = 0;
    int found = 0;

    char *lowerText = (char *)malloc((N + 1) * sizeof(char));
    char *lowerPattern = (char *)malloc((M + 1) * sizeof(char));

    if (!lowerText || !lowerPattern) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    if (!caseSensitive) {
        for (int i = 0; i < N; i++) lowerText[i] = tolower(text[i]);
        for (int i = 0; i < M; i++) lowerPattern[i] = tolower(pattern[i]);
        lowerText[N] = '\0';
        lowerPattern[M] = '\0';
        text = lowerText;
        pattern = lowerPattern;
    } else {
        strcpy(lowerText, text);
        strcpy(lowerPattern, pattern);
    }

    for (int i = 0; i < M - 1; i++) h = (h * d) % q;

    for (int i = 0; i < M; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
        hashCalculations += 2;
    }

    for (int i = 0; i <= N - M; i++) {
        if (p == t) {
            if (strncmp(text + i, pattern, M) == 0) {
                printf("Pattern '%s' found at index %d\n", pattern, i);
                found = 1;
            }
            characterComparisons += M;
        }

        if (i < N - M) {
            t = (d * (t - text[i] * h) + text[i + M]) % q;
            if (t < 0) t += q;
            hashCalculations++;
        }
    }

    if (!found) {
        printf("No matches found for pattern '%s'.\n", pattern);
    }

    *totalHashCalculations += hashCalculations;
    *totalCharacterComparisons += characterComparisons;

    // Update total potential
    *totalPotential += calculatePotential(hashCalculations, characterComparisons);

    printf("Hash calculations for pattern '%s': %d\n", pattern, hashCalculations);
    printf("Character comparisons for pattern '%s': %d\n", pattern, characterComparisons);

    free(lowerText);
    free(lowerPattern);
}

int main() {
    char *text = (char *)malloc(MAX_TEXT_SIZE * sizeof(char));

    if (!text) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    printf("Enter the text: ");
    fgets(text, MAX_TEXT_SIZE, stdin);
    text[strcspn(text, "\n")] = '\0';

    int multiplePatterns;
    printf("Do you want to search for multiple patterns? (1 for Yes, 0 for No): ");
    scanf("%d", &multiplePatterns);
    getchar();

    int caseSensitive;
    printf("Do you want a case-sensitive search? (1 for Yes, 0 for No): ");
    scanf("%d", &caseSensitive);
    getchar();

    int totalHashCalculations = 0;
    int totalCharacterComparisons = 0;
    int totalPotential = 0;
    int patternCount = 0;

    if (multiplePatterns) {
        char *patterns[MAX_PATTERNS];

        printf("Enter the number of patterns (up to %d): ", MAX_PATTERNS);
        scanf("%d", &patternCount);
        getchar();

        if (patternCount > MAX_PATTERNS) {
            printf("Exceeding maximum patterns allowed (%d).\n", MAX_PATTERNS);
            free(text);
            return 1;
        }

        for (int i = 0; i < patternCount; i++) {
            patterns[i] = (char *)malloc(MAX_PATTERN_SIZE * sizeof(char));
            printf("Enter pattern %d: ", i + 1);
            fgets(patterns[i], MAX_PATTERN_SIZE, stdin);
            patterns[i][strcspn(patterns[i], "\n")] = '\0';
        }

        clock_t start = clock();
        for (int i = 0; i < patternCount; i++) {
            rabinKarp(text, patterns[i], caseSensitive, &totalHashCalculations, &totalCharacterComparisons, &totalPotential);
            free(patterns[i]);
        }
        clock_t end = clock();

        printf("Time taken for multiple patterns: %lf seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
    } else {
        char pattern[MAX_PATTERN_SIZE];
        printf("Enter the pattern: ");
        fgets(pattern, MAX_PATTERN_SIZE, stdin);
        pattern[strcspn(pattern, "\n")] = '\0';

        clock_t start = clock();
        rabinKarp(text, pattern, caseSensitive, &totalHashCalculations, &totalCharacterComparisons, &totalPotential);
        clock_t end = clock();

        printf("Time taken for single pattern: %lf seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
        patternCount = 1;
    }

    // Aggregate analysis
    printf("Total hash calculations: %d\n", totalHashCalculations);
    printf("Total character comparisons: %d\n", totalCharacterComparisons);

    if (patternCount > 0) {
        printf("Aggregate amortized hash calculations: %lf\n", (double)totalHashCalculations / patternCount);
        printf("Aggregate amortized character comparisons: %lf\n", (double)totalCharacterComparisons / patternCount);
    }

    // Potential analysis
    printf("Total potential: %d\n", totalPotential);
    if (patternCount > 0) {
        printf("Amortized potential cost: %lf\n", (double)totalPotential / patternCount);
    }


    // Accounting analysis
    printf("Total accounting costs: %d\n", totalHashCalculations + totalCharacterComparisons);
    if (patternCount > 0) {
        printf("Amortized accounting cost: %lf\n", (double)(totalHashCalculations + totalCharacterComparisons) / patternCount);
    }

    free(text);
    return 0;
}

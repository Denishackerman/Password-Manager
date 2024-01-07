#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>

#define FILE_NAME "parole.txt"

#define MAX_PASSWORDS 100

struct stPasswordEntry
{
    char website[50];
    char username[50];
    char password[50];
};

struct stPasswordEntry *vec = NULL;
size_t vecSize = 0; // Elemente din vector

void setColor(int text, int background)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, text | (background << 4));
}

void setBackgroundColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set the screen buffer size to the current window size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD bufferSize = {csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // Set the entire screen buffer to the specified background color
    DWORD written;
    FillConsoleOutputAttribute(hConsole, color, bufferSize.X * bufferSize.Y, (COORD){0, 0}, &written);
}

// Functie generare parola
void generatePassword(int length, int uppercase, int lowercase, int digits, int symbols, char *password)
{
    const char *uppercaseChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *lowercaseChars = "abcdefghijklmnopqrstuvwxyz";
    const char *digitChars = "0123456789";
    const char *symbolChars = "!@#$%^&*()-=_+[]{}|;:',.<>?";

    char *pool = malloc((uppercase * 26 + lowercase * 26 + digits * 10 + symbols * 22 + 1) * sizeof(char));

    if (pool == NULL)
    {
        printf("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    int poolIndex = 0;

    if (uppercase)
    {
        for (int i = 0; i < 26; i++)
        {
            pool[poolIndex++] = uppercaseChars[i];
        }
    }

    if (lowercase)
    {
        for (int i = 0; i < 26; i++)
        {
            pool[poolIndex++] = lowercaseChars[i];
        }
    }

    if (digits)
    {
        for (int i = 0; i < 10; i++)
        {
            pool[poolIndex++] = digitChars[i];
        }
    }

    if (symbols)
    {
        for (int i = 0; i < 22; i++)
        {
            pool[poolIndex++] = symbolChars[i];
        }
    }

    pool[poolIndex] = '\0';

    srand((unsigned int)time(NULL));

    for (int i = 0; i < length; i++)
    {
        int randomIndex = rand() % poolIndex;
        password[i] = pool[randomIndex];
    }

    password[length] = '\0';

    // Goleste memoria
    free(pool);
}

// Copiaza string
void copyToClipboard(const char *text)
{
    if (!OpenClipboard(NULL))
    {
        printf("Nu s-a putut deschide clipboard-ul");
        return;
    }

    // Goleste clipboard
    if (!EmptyClipboard())
    {
        printf("Nu s-a putut goli clipboard-ul");
        CloseClipboard();
        return;
    }

    // Aloca memorie globala pentru text
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (strlen(text) + 1) * sizeof(char));
    if (hMem == NULL)
    {
        printf("Nu s-a putut aloca memorie globala");
        CloseClipboard();
        return;
    }

    // Blocheaza memoria globala
    char *memData = (char *)GlobalLock(hMem);
    if (memData == NULL)
    {
        printf("Nu s-a putut bloca memoria globala");
        GlobalFree(hMem);
        CloseClipboard();
        return;
    }

    strcpy(memData, text);

    GlobalUnlock(hMem);

    // Data din clipboard
    if (SetClipboardData(CF_TEXT, hMem) == NULL)
    {
        printf("Nu s-au putut seta datele din clipboard");
        GlobalFree(hMem);
        CloseClipboard();
        return;
    }

    // Inchide clipboard
    CloseClipboard();
}

void readFromFile()
{
    FILE *pFile;
    pFile = fopen(FILE_NAME, "r");

    char szLine[100];
    while (fgets(szLine, sizeof(szLine), pFile) != NULL)
    {
        vec = realloc(vec, (vecSize + 1) * sizeof(struct stPasswordEntry));
        sscanf(szLine, "%[^|]|%[^|]|%s", vec[vecSize].website, vec[vecSize].username, vec[vecSize].password);
        vecSize++;
    }
    fclose(pFile);
}

void savePasswordToFile(const char *filename, const char *website, const char *username, const char *password)
{
    FILE *pFile;
    pFile = fopen(filename, "a+");
    char szLine[150];
    sprintf(szLine, "%s|%s|%s\n", website, username, password);
    fprintf(pFile, szLine);
    fclose(pFile);
}

void addEntry(const char *website, const char *username, const char *password)
{
    // Creste marimea vector
    vecSize++;
    vec = realloc(vec, vecSize * sizeof(struct stPasswordEntry));

    // Adauga valoare noua
    struct stPasswordEntry newEntry;
    strncpy(newEntry.website, website, sizeof(newEntry.website) - 1);
    strncpy(newEntry.username, username, sizeof(newEntry.username) - 1);
    strncpy(newEntry.password, password, sizeof(newEntry.password) - 1);

    vec[vecSize - 1] = newEntry;

    savePasswordToFile(FILE_NAME, website, username, password);
}

void printEntries()
{
    printf("\nParole stocate:\n");
    for (size_t i = 0; i < vecSize; i++)
        printf("Website: %s,Nume Utilizator: %s, Parola: %s\n", vec[i].website, vec[i].username, vec[i].password);
}

int getMenuChoice()
{

    int choice;
    printf("\nMeniu manager parole:\n");
    printf("1. Creeaza o parola noua\n");
    printf("2. Salveaza parola\n");
    printf("3. Afiseaza parolele salvate\n");
    printf("4. Inchide programul\n");

    setBackgroundColor(2);
    setColor(11, 0);
    printf("\nScrieti numarul corespunzator al optiunii dorite: \n");

    scanf("%d", &choice);

    return choice;
}

void clearConsole()
{
    system("cls");
}

void displayEntriesMenu()
{
    while (1)
    {
        setBackgroundColor(0);
        setColor(2, 0);

        // arata parolele salvate
        printEntries();

        setBackgroundColor(7);
        setColor(8, 0);

        printf("\nIntoarcere la primul meniu? 1 pentru da, 0 pentru nu: \n");

        int confirm;
        scanf("%d", &confirm);

        if (confirm == 1)
        {
            break; // iesi din loop
        }
        else
        {
            printf("Operatiune anulata\n");
            return;
        }
    }
}

int main()
{
    setBackgroundColor(0);
    setColor(2, 0);
    readFromFile();
    int length, uppercase, lowercase, digits, symbols, passwords;
    while (1)
    {
        int choice = getMenuChoice();
        switch (choice)
        {
        case 1:
        {
            setBackgroundColor(0);
            setColor(4, 0);
            printf("Scrie lungimea parolei: ");
            scanf("%d", &length);

            if (length <= 0)
            {
                printf("Lungime parola invalida!\n");
                return EXIT_FAILURE;
            }

            printf("Includere litere mari: (1 pentru da, 0 pentru nu): ");
            scanf("%d", &uppercase);

            printf("Includere litere mici: (1 pentru da, 0 pentru nu): ");
            scanf("%d", &lowercase);

            printf("Includere cifre: (1 pentru da, 0 pentru nu): ");
            scanf("%d", &digits);

            printf("Includere simboluri: (1 pentru da, 0 pentru nu): ");
            scanf("%d", &symbols);

            setBackgroundColor(0);
            setColor(7, 0);
            clearConsole();

            // afiseaza ce s-a selectat
            printf("\nCriterii alese:\n");
            printf("Lungimea parolei: %d\n", length);
            printf("Litere mari: %s\n", uppercase ? "Da" : "No");
            printf("Litere mici: %s\n", lowercase ? "Da" : "Nu");
            printf("Cifre: %s\n", digits ? "Da" : "Nu");
            printf("Simboluri: %s\n", symbols ? "Da" : "Nu");

            setBackgroundColor(7);
            setColor(5, 0);
            // Confirma inaintea generarii parolei
            char confirmation;
            printf("\nGenerezi parola cu setarile de mai sus? (y/n): ");
            scanf(" %c", &confirmation);

            if (confirmation != 'y' && confirmation != 'Y')
            {
                printf("Generarea parolei anulata.\n");
                return EXIT_SUCCESS;
            }

            // Genereaza si afiseaza parola
            char *password = malloc((length + 1) * sizeof(char));

            if (password == NULL)
            {
                perror("Eroare alocare memorie");
                return EXIT_FAILURE;
            }
            setBackgroundColor(0);
            setColor(2, 0);

            generatePassword(length, uppercase, lowercase, digits, symbols, password);
            printf("\nParola creata este: %s\n", password);

            // copiaza clipboard
            char copyOption;
            printf("Doriti sa copiati parola in clipboard? (y/n): ");
            scanf(" %c", &copyOption);

            if (copyOption == 'y' || copyOption == 'Y')
            {
                copyToClipboard(password);
                printf("Parola a fost copiata!\n");
            }
            setBackgroundColor(0);
            setColor(2, 0);
            clearConsole();
            break;
        }
            setBackgroundColor(0);
            setColor(2, 0);
        case 2:
        {
            setBackgroundColor(0);
            setColor(9, 0);

            // Cod salvare parola
            char website[50], username[50], password[50];
            printf("\nIntrodu website-ul: ");
            scanf("%s", website);
            printf("Introdu numele de utilizator: ");
            scanf("%s", username);

            setBackgroundColor(0);
            setColor(6, 0);

            // generare parola
            char *newPassword = malloc((length + 1) * sizeof(char));
            generatePassword(length, uppercase, lowercase, digits, symbols, newPassword);
            printf("Parola creata este: %s\n", newPassword);

            setBackgroundColor(0);
            setColor(6, 0);

            // salvare
            addEntry(website, username, newPassword);

            // golire memorie
            free(newPassword);

            setBackgroundColor(0);
            setColor(2, 0);

            clearConsole();
            break;
        }
        case 3:
            clearConsole();

            // parole
            displayEntriesMenu();

            clearConsole();
            break;
        case 4:
            // iesire
            printf("Iesire Password Manager.\n");
            return EXIT_SUCCESS;
        default:
            printf("Optiune invalida. Alegeti o optiune valida.\n");
        }
    }
    // goleste memoria alocata
    free(vec);

    return EXIT_SUCCESS;
}

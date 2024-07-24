#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "string.h"

#define MAX_CONTACTS 100
#define MAX_LENGTH 50
#define FILENAME "contacts.txt"

typedef struct {
    char work[MAX_LENGTH];
    char home[MAX_LENGTH];
} Email;

typedef struct {
    char social_network[MAX_LENGTH];
    char social_address[MAX_LENGTH];
    char nickname[MAX_LENGTH];
} SocialProfile;

typedef struct {
    char name[MAX_LENGTH];
    char job[MAX_LENGTH];
    char phone[MAX_LENGTH];
    Email email;
    SocialProfile* social;
    int social_count;
} Contact;

void cleanBuff() {
    fseek(stdin, 0, SEEK_END);
}

void readContacts(Contact contacts[], int* count) {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        return;
    }

    fscanf(file, "%d\n", count);

    for (int i = 0; i < *count; i++) {
        fgets(contacts[i].name, MAX_LENGTH, file);
        contacts[i].name[strcspn(contacts[i].name, "\n")] = '\0';
        fgets(contacts[i].job, MAX_LENGTH, file);
        contacts[i].job[strcspn(contacts[i].job, "\n")] = '\0';
        fgets(contacts[i].phone, MAX_LENGTH, file);
        contacts[i].phone[strcspn(contacts[i].phone, "\n")] = '\0';
        fgets(contacts[i].email.work, MAX_LENGTH, file);
        contacts[i].email.work[strcspn(contacts[i].email.work, "\n")] = '\0';
        fgets(contacts[i].email.home, MAX_LENGTH, file);
        contacts[i].email.home[strcspn(contacts[i].email.home, "\n")] = '\0';

        fscanf(file, "%d\n", &contacts[i].social_count);
        if (contacts[i].social_count > 0) {
            contacts[i].social = malloc(contacts[i].social_count * sizeof(SocialProfile));
            for (int j = 0; j < contacts[i].social_count; j++) {
                fgets(contacts[i].social[j].social_network, MAX_LENGTH, file);
                contacts[i].social[j].social_network[strcspn(contacts[i].social[j].social_network, "\n")] = '\0';
                fgets(contacts[i].social[j].social_address, MAX_LENGTH, file);
                contacts[i].social[j].social_address[strcspn(contacts[i].social[j].social_address, "\n")] = '\0';
                fgets(contacts[i].social[j].nickname, MAX_LENGTH, file);
                contacts[i].social[j].nickname[strcspn(contacts[i].social[j].nickname, "\n")] = '\0';
            }
        }
        else {
            contacts[i].social = NULL;
        }
    }

    fclose(file);
}

void writeContacts(Contact contacts[], int count) {
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error: failed to open file.\n");
        return;
    }

    fprintf(file, "%d\n", count);

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n", contacts[i].name);
        fprintf(file, "%s\n", contacts[i].job);
        fprintf(file, "%s\n", contacts[i].phone);
        fprintf(file, "%s\n", contacts[i].email.work);
        fprintf(file, "%s\n", contacts[i].email.home);
        fprintf(file, "%d\n", contacts[i].social_count);
        for (int j = 0; j < contacts[i].social_count; j++) {
            fprintf(file, "%s\n", contacts[i].social[j].social_network);
            fprintf(file, "%s\n", contacts[i].social[j].social_address);
            fprintf(file, "%s\n", contacts[i].social[j].nickname);
        }
    }

    fclose(file);
}

void addContact(Contact contacts[], int* count) {
    if (*count >= MAX_CONTACTS) {
        printf("Error: The maximum number of contacts has been reached.\n");
        return;
    }

    Contact newContact;
    Email newEmail;
    SocialProfile* newSocial = NULL;
    int social_count = 0;
    char moreSocial;

    printf("Enter name: ");
    scanf("%s", newContact.name);

    printf("Enter workplace: ");
    cleanBuff();
    fgets(newContact.job, sizeof(newContact.job), stdin);
    newContact.job[strcspn(newContact.job, "\n")] = '\0';

    printf("Enter phone number: ");
    cleanBuff();
    fgets(newContact.phone, sizeof(newContact.phone), stdin);
    newContact.phone[strcspn(newContact.phone, "\n")] = '\0';

    printf("Enter your work email address: ");
    cleanBuff();
    fgets(newEmail.work, sizeof(newEmail.work), stdin);
    newEmail.work[strcspn(newEmail.work, "\n")] = '\0';

    printf("Enter your home email address: ");
    cleanBuff();
    fgets(newEmail.home, sizeof(newEmail.home), stdin);
    newEmail.home[strcspn(newEmail.home, "\n")] = '\0';

    newContact.email = newEmail;

    do {
        newSocial = realloc(newSocial, (social_count + 1) * sizeof(SocialProfile));
        if (newSocial == NULL) {
            printf("Error allocating memory for social networks.\n");
            return;
        }

        printf("Enter social network: ");
        cleanBuff();
        fgets(newSocial[social_count].social_network, sizeof(newSocial[social_count].social_network), stdin);
        newSocial[social_count].social_network[strcspn(newSocial[social_count].social_network, "\n")] = '\0';

        printf("Enter the address on this social network: ");
        cleanBuff();
        fgets(newSocial[social_count].social_address, sizeof(newSocial[social_count].social_address), stdin);
        newSocial[social_count].social_address[strcspn(newSocial[social_count].social_address, "\n")] = '\0';

        printf("Enter your nickname on this social network: ");
        cleanBuff();
        fgets(newSocial[social_count].nickname, sizeof(newSocial[social_count].nickname), stdin);
        newSocial[social_count].nickname[strcspn(newSocial[social_count].nickname, "\n")] = '\0';

        social_count++;

        printf("Want to add another social network? (y/n): ");
        moreSocial = getchar();
        cleanBuff;

    } while (moreSocial == 'y' || moreSocial == 'Y');

    newContact.social = newSocial;
    newContact.social_count = social_count;

    contacts[*count] = newContact;
    (*count)++;

    printf("The contact was successfully added.\n\n");
}

void editContact(Contact contacts[], int count) {
    if (count == 0) {
        printf("Error: The phone book is empty.\n");
        return;
    }

    char nameToEdit[MAX_LENGTH];
    SocialProfile* newSocial = NULL;
    int social_count = 0;
    char moreSocial;

    printf("Enter the contact name to edit: ");
    scanf("%s", nameToEdit);

    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].name, nameToEdit) == 0) {
            printf("Enter a new name: ");
            scanf("%s", contacts[i].name);

            printf("Enter your new place of work: ");
            cleanBuff();
            fgets(contacts[i].job, sizeof(contacts[i].job), stdin);
            contacts[i].job[strcspn(contacts[i].job, "\n")] = '\0';

            printf("Enter a new phone number: ");
            cleanBuff();
            fgets(contacts[i].phone, sizeof(contacts[i].phone), stdin);
            contacts[i].phone[strcspn(contacts[i].phone, "\n")] = '\0';

            printf("Enter your new work email address: ");
            cleanBuff();
            fgets(contacts[i].email.work, sizeof(contacts[i].email.work), stdin);
            contacts[i].email.work[strcspn(contacts[i].email.work, "\n")] = '\0';

            printf("Enter your new home email address: ");
            cleanBuff();
            fgets(contacts[i].email.home, sizeof(contacts[i].email.home), stdin);
            contacts[i].email.home[strcspn(contacts[i].email.home, "\n")] = '\0';

            do {
                newSocial = realloc(newSocial, (social_count + 1) * sizeof(SocialProfile));
                if (newSocial == NULL) {
                    printf("Error allocating memory for social networks.\n");
                    return;
                }

                printf("Enter social network: ");
                cleanBuff();
                fgets(newSocial[social_count].social_network, sizeof(newSocial[social_count].social_network), stdin);
                newSocial[social_count].social_network[strcspn(newSocial[social_count].social_network, "\n")] = '\0';

                printf("Enter the address on this social network: ");
                cleanBuff();
                fgets(newSocial[social_count].social_address, sizeof(newSocial[social_count].social_address), stdin);
                newSocial[social_count].social_address[strcspn(newSocial[social_count].social_address, "\n")] = '\0';

                printf("Enter your nickname on this social network: ");
                cleanBuff();
                fgets(newSocial[social_count].nickname, sizeof(newSocial[social_count].nickname), stdin);
                newSocial[social_count].nickname[strcspn(newSocial[social_count].nickname, "\n")] = '\0';

                social_count++;

                printf("Want to add another social network? (y/n): ");
                moreSocial = getchar();
                cleanBuff;

            } while (moreSocial == 'y' || moreSocial == 'Y');

            contacts[i].social = newSocial;
            contacts[i].social_count = social_count;

            printf("The contact has been successfully edited.\n\n");
            return;
        }
    }

    printf("Error: Contact named '%s' was not found.\n", nameToEdit);
}

void deleteContact(Contact contacts[], int* count) {
    if (*count == 0) {
        printf("Error: The phone book is empty.\n");
        return;
    }

    char nameToDelete[MAX_LENGTH];
    printf("Enter the name of the contact to delete: ");
    scanf("%s", nameToDelete);

    for (int i = 0; i < *count; i++) {
        if (strcmp(contacts[i].name, nameToDelete) == 0) {
            (*count)--;
            for (int j = i; j < *count; j++) {
                contacts[j] = contacts[j + 1];
            }
            printf("The contact was successfully deleted.\n\n");
            return;
        }
    }

    printf("Error: Contact named '%s' was not found.\n", nameToDelete);
}

void printContact(Contact contact) {
    printf("Name: %s\n", contact.name);
    printf("Place of work: %s\n", contact.job);
    printf("Phone number: %s\n", contact.phone);
    printf("Work email address: %s\n", contact.email.work);
    printf("Home email address: %s\n", contact.email.work);
    for (int i = 0; i < contact.social_count; i++) {
        printf("Social network name: %s\n", contact.social[i].social_network);
        printf("Social network link: %s\n", contact.social[i].social_address);
        printf("Nickname: %s\n\n", contact.social[i].nickname);
    }
}

int main() {
    Contact contacts[MAX_CONTACTS];
    int count = 0;
    int tmp = 0;

    readContacts(contacts, &count);

    do {
        printf("Phone book\n");
        printf("1. Add contact\n");
        printf("2. Edit contact\n");
        printf("3. Delete contact\n");
        printf("4. Display all contacts\n");
        printf("5. Exit\n");
        printf("Choose an operation: ");
        scanf("%d", &tmp);

        switch (tmp) {
        case 1:
            printf("\033[0d\033[2J");
            addContact(contacts, &count);
            break;
        case 2:
            printf("\033[0d\033[2J");
            editContact(contacts, &count);
            break;
        case 3:
            printf("\033[0d\033[2J");
            deleteContact(contacts, &count);
            break;
        case 4:
            printf("\033[0d\033[2J");
            for (int i = 0; i < count; i++) {
                printContact(contacts[i]);
            }
            break;
        case 5:
            writeContacts(contacts, count);
            printf("\033[0d\033[2J");
            printf("Exit...\n");
            break;
        default:
            printf("Error: Incorrect choice.\n");
            break;
        }
    } while (tmp != 5);

    return 0;
}
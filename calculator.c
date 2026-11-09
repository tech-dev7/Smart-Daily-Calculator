#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define HIST_MAX 10
#define HIST_FILE "history.txt"

static char history[HIST_MAX][100];
static int hist_count = 0;

/* ---------- helpers ---------- */
void line(void) { printf("+------------------------------------------+\n"); }

void header(const char *title) {
    printf("\n");
    line();
    printf("|  %-40s|\n", title);
    line();
}

/* safe number input: keeps asking until a valid number is typed */
double read_double(const char *prompt) {
    char buf[64], *end;
    double v;
    while (1) {
        printf("  %s", prompt);
        if (fgets(buf, sizeof buf, stdin) == NULL) exit(0);
        v = strtod(buf, &end);
        if (end != buf && (*end == '\n' || *end == '\0')) return v;
        printf("  Invalid number, try again.\n");
    }
}

int read_choice(const char *prompt, int lo, int hi) {
    char buf[16];
    int v;
    while (1) {
        printf("  %s", prompt);
        if (fgets(buf, sizeof buf, stdin) == NULL) exit(0);
        v = atoi(buf);
        if (v >= lo && v <= hi) return v;
        printf("  Please enter a number from %d to %d.\n", lo, hi);
    }
}

void add_history(const char *entry) {
    if (hist_count == HIST_MAX) {                 /* drop the oldest */
        for (int i = 1; i < HIST_MAX; i++)
            strcpy(history[i - 1], history[i]);
        hist_count--;
    }
    strncpy(history[hist_count], entry, 99);
    history[hist_count][99] = '\0';
    hist_count++;
    FILE *f = fopen(HIST_FILE, "a");              /* keep a permanent copy */
    if (f) { fprintf(f, "%s\n", entry); fclose(f); }
}

/* ---------- 1. basic calculator ---------- */
void basic_calculator(void) {
    char text[100];
    double a, b, r;
    header("BASIC CALCULATOR");
    printf("  1. Add        2. Subtract    3. Multiply\n");
    printf("  4. Divide     5. Remainder   6. Power\n");
    printf("  7. Square root\n");
    int op = read_choice("Choose operation (1-7): ", 1, 7);

    a = read_double(op == 7 ? "Enter number: " : "Enter first number: ");
    if (op == 7) {
        if (a < 0) { printf("  Error: square root of a negative number.\n"); return; }
        r = sqrt(a);
        printf("\n  Result: sqrt(%g) = %g\n", a, r);
        snprintf(text, sizeof text, "sqrt(%g) = %g", a, r);
        add_history(text);
        return;
    }
    b = read_double("Enter second number: ");

    switch (op) {
        case 1: r = a + b; snprintf(text, sizeof text, "%g + %g = %g", a, b, r); break;
        case 2: r = a - b; snprintf(text, sizeof text, "%g - %g = %g", a, b, r); break;
        case 3: r = a * b; snprintf(text, sizeof text, "%g x %g = %g", a, b, r); break;
        case 4:
            if (b == 0) { printf("  Error: cannot divide by zero.\n"); return; }
            r = a / b; snprintf(text, sizeof text, "%g / %g = %g", a, b, r); break;
        case 5:
            if (b == 0) { printf("  Error: cannot divide by zero.\n"); return; }
            r = fmod(a, b); snprintf(text, sizeof text, "%g mod %g = %g", a, b, r); break;
        default: r = pow(a, b); snprintf(text, sizeof text, "%g ^ %g = %g", a, b, r); break;
    }
    printf("\n  Result: %s\n", text);
    add_history(text);
}

/* ---------- 2. daily tools ---------- */
void daily_tools(void) {
    char text[100];
    header("DAILY TOOLS");
    printf("  1. Discount        2. GST / Tax\n");
    printf("  3. Loan EMI        4. Bill split\n");
    printf("  5. Percentage\n");
    int t = read_choice("Choose tool (1-5): ", 1, 5);

    if (t == 1) {
        double price = read_double("Original price (Rs): ");
        double d = read_double("Discount (%): ");
        double save = price * d / 100.0;
        printf("\n  You save   : Rs %.2f\n  Final price: Rs %.2f\n", save, price - save);
        snprintf(text, sizeof text, "Discount %g%% on Rs %.2f = Rs %.2f", d, price, price - save);
    } else if (t == 2) {
        double amt = read_double("Amount before tax (Rs): ");
        double rate = read_double("GST rate (%): ");
        double gst = amt * rate / 100.0;
        printf("\n  GST amount : Rs %.2f\n  Total      : Rs %.2f\n", gst, amt + gst);
        snprintf(text, sizeof text, "GST %g%% on Rs %.2f = Rs %.2f", rate, amt, amt + gst);
    } else if (t == 3) {
        double p = read_double("Loan amount (Rs): ");
        double rate = read_double("Yearly interest rate (%): ");
        double years = read_double("Time (years): ");
        double r = rate / 12.0 / 100.0;               /* monthly rate */
        int n = (int)(years * 12);                    /* number of months */
        if (n <= 0 || p <= 0) { printf("  Error: enter a positive amount and time.\n"); return; }
        double emi = (r == 0) ? p / n : p * r * pow(1 + r, n) / (pow(1 + r, n) - 1);
        printf("\n  Monthly EMI   : Rs %.2f\n  Total payable : Rs %.2f\n  Total interest: Rs %.2f\n",
               emi, emi * n, emi * n - p);
        snprintf(text, sizeof text, "EMI on Rs %.0f for %g yr @ %g%% = Rs %.2f/month", p, years, rate, emi);
    } else if (t == 4) {
        double bill = read_double("Total bill (Rs): ");
        int people = read_choice("Number of people (1-50): ", 1, 50);
        double tip = read_double("Tip (%): ");
        double total = bill + bill * tip / 100.0;
        printf("\n  Total with tip: Rs %.2f\n  Each person   : Rs %.2f\n", total, total / people);
        snprintf(text, sizeof text, "Split Rs %.2f among %d = Rs %.2f each", total, people, total / people);
    } else {
        double pct = read_double("Percentage (%): ");
        double of = read_double("Of the number: ");
        printf("\n  %g%% of %g = %g\n", pct, of, pct * of / 100.0);
        snprintf(text, sizeof text, "%g%% of %g = %g", pct, of, pct * of / 100.0);
    }
    add_history(text);
}

/* ---------- 3. unit converter ---------- */
void converter(void) {
    char text[100];
    header("UNIT CONVERTER");
    printf("  1. Celsius to Fahrenheit   2. Fahrenheit to Celsius\n");
    printf("  3. Kilometre to Mile       4. Mile to Kilometre\n");
    printf("  5. Kilogram to Pound       6. Pound to Kilogram\n");
    int c = read_choice("Choose conversion (1-6): ", 1, 6);
    double v = read_double("Enter value: ");
    double r;
    const char *from[] = {"C", "F", "km", "mi", "kg", "lb"};
    const char *to[]   = {"F", "C", "mi", "km", "lb", "kg"};
    switch (c) {
        case 1: r = v * 9.0 / 5.0 + 32; break;
        case 2: r = (v - 32) * 5.0 / 9.0; break;
        case 3: r = v * 0.621371; break;
        case 4: r = v * 1.609344; break;
        case 5: r = v * 2.204623; break;
        default: r = v * 0.453592; break;
    }
    printf("\n  Result: %g %s = %.2f %s\n", v, from[c - 1], r, to[c - 1]);
    snprintf(text, sizeof text, "%g %s = %.2f %s", v, from[c - 1], r, to[c - 1]);
    add_history(text);
}

/* ---------- 4. history ---------- */
void show_history(void) {
    header("RECENT HISTORY (last 10)");
    if (hist_count == 0) { printf("  No calculations yet.\n"); return; }
    for (int i = hist_count - 1; i >= 0; i--)
        printf("  %2d. %s\n", hist_count - i, history[i]);
}

/* ---------- main ---------- */
int main(void) {
    int choice;
    printf("\n");
    line();
    printf("|          SMART DAILY CALCULATOR          |\n");
    printf("|        simple  |  fast  |  reliable      |\n");
    line();
    do {
        printf("\n  1. Basic Calculator\n  2. Daily Tools\n  3. Unit Converter\n");
        printf("  4. History\n  5. Exit\n");
        choice = read_choice("Select option (1-5): ", 1, 5);
        switch (choice) {
            case 1: basic_calculator(); break;
            case 2: daily_tools();      break;
            case 3: converter();        break;
            case 4: show_history();     break;
        }
    } while (choice != 5);
    printf("\n  Thank you for using Smart Daily Calculator!\n\n");
    return 0;
}

